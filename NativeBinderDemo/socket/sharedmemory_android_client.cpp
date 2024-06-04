#ifdef ANDROID
#include "../sharedmemory/sharedmemory_android.h"
#include <android/sharedmem.h>
#include <android/log.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <strings.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "polyspatial_sharedmemory", __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, "polyspatial_sharedmemory", __VA_ARGS__)
#ifdef NDEBUG
#define ALOGV(...) 
#define ALOGI(...) 
#else
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "polyspatial_sharedmemory", __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, "polyspatial_sharedmemory", __VA_ARGS__)
#endif

namespace PolySpatialIpc
{
	int send_fd(int sock, int fd_to_send)
	{
		// This function does the arcane magic for sending
		// file descriptors over unix domain sockets
		struct msghdr msg;
		struct iovec iov[1];
		struct cmsghdr* cmsg = NULL;
		char ctrl_buf[CMSG_SPACE(sizeof(int))];
		char data[1];

		memset(&msg, 0, sizeof(struct msghdr));
		memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

		data[0] = 'H';
		iov[0].iov_base = data;
		iov[0].iov_len = sizeof(data);

		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;
		msg.msg_controllen = CMSG_SPACE(sizeof(int));
		msg.msg_control = ctrl_buf;

		cmsg = CMSG_FIRSTHDR(&msg);
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));

		*((int*)CMSG_DATA(cmsg)) = fd_to_send;

		return sendmsg(sock, &msg, 0);
	}

	int recv_fd(int sock) {
		// This function does the arcane magic recving
		// file descriptors over unix domain sockets
		struct msghdr msg;
		struct iovec iov[1];
		struct cmsghdr* cmsg = NULL;
		char ctrl_buf[CMSG_SPACE(sizeof(int))];
		char data[1];

		memset(&msg, 0, sizeof(struct msghdr));
		memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

		iov[0].iov_base = data;
		iov[0].iov_len = sizeof(data);

		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		msg.msg_control = ctrl_buf;
		msg.msg_controllen = CMSG_SPACE(sizeof(int));
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;

		auto ret = recvmsg(sock, &msg, 0);
		ALOGV("PolySpatial ReceiveFileDescriptor recv_fd sock: %d, data: %c, receive: %ld\n", sock, data[0], ret);
		if (ret <= 0)
			return 0;

		cmsg = CMSG_FIRSTHDR(&msg);
		if (cmsg == NULL)
		{
			ALOGV("PolySpatial ReceiveFileDescriptor recv_fd sock: %d, no passed fd, %zu\n", sock, msg.msg_controllen);
			return 0;
		}

		return *((int*)CMSG_DATA(cmsg));
	}

	int send_fd2(int sock, int fd_to_send)
	{
		// This function does the arcane magic for sending
		// file descriptors over unix domain sockets
		struct msghdr msg;
		struct iovec iov[1];
		char buf[2];
		buf[0] = 'H';

		iov[0].iov_base = buf;
		iov[0].iov_len = sizeof(buf);
		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;

		struct cmsghdr cmsg;
		cmsg.cmsg_level = SOL_SOCKET;
		cmsg.cmsg_type = SCM_RIGHTS;
		cmsg.cmsg_len = CMSG_LEN(sizeof(int));
		*((int*)CMSG_DATA(&cmsg)) = fd_to_send;

		msg.msg_controllen = CMSG_LEN(sizeof(int));
		msg.msg_control = &cmsg;

		return sendmsg(sock, &msg, 0);
	}

	int recv_fd2(int sock) {
		// This function does the arcane magic recving
		// file descriptors over unix domain sockets
		struct msghdr msg;
		struct iovec iov[1];
		char buf[2];

		iov[0].iov_base = buf;
		iov[0].iov_len = sizeof(buf);

		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;

		struct cmsghdr cmsg;
		msg.msg_control = &cmsg;
		msg.msg_controllen = CMSG_LEN(sizeof(int));

		auto ret = recvmsg(sock, &msg, 0);
		ALOGV("PolySpatial ReceiveFileDescriptor recv_fd sock: %d, data: %c, receive: %ld\n", sock, buf[0], ret);
		if (ret <= 0)
			return 0;

		return *((int*)CMSG_DATA(&cmsg));
	}

	int getFileSize(int fd) {
          struct stat file_stat;
 
          // 获取文件描述符的状态信息
          if (fstat(fd, &file_stat) == -1) {
            perror("fstat");
            return -1;
          }
 
          // 打印文件的大小（单位：字节）
          return file_stat.st_size;
        };

	int socket_send(const char* socket_addr, int fd)
	{
		struct sockaddr_un addr;
		int sock;
		int conn;

		// Create a unix domain socket
		sock = socket(AF_UNIX, SOCK_STREAM, 0);

		// Bind it to a abstract address
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(&addr.sun_path[1], socket_addr);
		bind(sock, (struct sockaddr*)&addr, sizeof(addr));

		// Listen
		listen(sock, 1);

		int ret = 0;
		// Just send the file descriptor to anyone who connects
		for (;;) {
			int size = getFileSize(fd);
			ALOGE("PolySpatial socket_send sock: %d,fd = %d,size = %d\n", sock,fd,size);
			conn = accept(sock, NULL, 0);
			ALOGE("PolySpatial socket_send sock: %d, conn: %ld\n", sock, conn);
			ret = send_fd(conn, fd);
			ALOGE("PolySpatial socket_send sock: %d, ret: %ld\n", sock, ret);
			if (ret > 0)
				break;
		}
		close(conn);
		return ret;
	}

	int socket_receive(const char* socket_addr)
	{
		struct sockaddr_un addr;
		int sock;

		// Create and connect a unix domain socket
		sock = socket(AF_UNIX, SOCK_STREAM, 0);
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(&addr.sun_path[1], socket_addr);
		connect(sock, (struct sockaddr*)&addr, sizeof(addr));

		// Recvive a file descriptor, and make it our CWD
		return recv_fd(sock);
	}
}

int main() {
	PolySpatialIpc::PolySpatialSharedMemoryAndroid m_sharedMemory;
	int fd = m_sharedMemory.RegisterMemory("shared_memory", 1024*1024);
		printf("shareMemory fd=%d\n",fd);
		char buffer[1024*1024];
		memset(buffer,6,sizeof(char)*1024);
		m_sharedMemory.WriteBuffer(buffer, 1024*1024, 0);
		printf("BpPolySpatialService::shareMemory buffer[0]=%d\n",buffer[1023]);

	PolySpatialIpc::socket_send("PolySpatialIpc",fd);
	
	return 0;
}

#endif //ANDROID