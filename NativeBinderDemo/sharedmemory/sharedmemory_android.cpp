#ifdef ANDROID
#include "sharedmemory_android.h"


#include  <utils/Log.h>
#include <android/sharedmem.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <strings.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


namespace PolySpatialIpc
{
    int PolySpatialSharedMemoryAndroid::RegisterMemory(const char* memName, uint32_t size)
    {
        int fd = ASharedMemory_create(memName, size);
        if (0 == fd)
        {
            ALOGE("PolySpatial Failed to RegisterMemory: %s", memName);
            return fd;
        }
        uint32_t memSize = ASharedMemory_getSize(fd);
		if (size > memSize)
		{
            ALOGW("PolySpatial The size (%d) of RegisterMemory (%s) is above the required size (%d).", memSize, memName, size);
		}
        
        m_fd = fd;

		// limit access to read only
		int ret = ASharedMemory_setProt(fd, PROT_READ | PROT_WRITE);

        ALOGV("PolySpatial RegisterMemory ret:%d, fd: %d, name: %s, size: %d\n", ret, m_fd, memName, memSize);

		//ret = send_fd(m_sock, m_fd);

        return m_fd;
    }

	int PolySpatialSharedMemoryAndroid::OpenMemory(int fd)
	{
        m_fd = fd;
        //m_fd = open(memName, O_RDONLY | O_CREAT | O_TRUNC);
        //struct stat statbuf;
        //fstat(m_fd, &statbuf);
        ////ftruncate(m_fd, statbuf.st_size - 1);

		//m_fd = recv_fd(m_sock);
		return m_fd;
	}

    void PolySpatialSharedMemoryAndroid::Destroy()
    {
        //close(m_fd);
        //m_fd = 0;
    }

    bool PolySpatialSharedMemoryAndroid::WriteBuffer(void* pBuffer, uint32_t size, uint32_t offset)
	{
		if (!IsValid())
			return false;

        char* pMapBuffer = (char*)mmap(NULL, size, PROT_WRITE, MAP_SHARED, m_fd, offset);
        if (MAP_FAILED == pMapBuffer)
        {
            ALOGE("PolySpatial WriteBuffer failed to mmap error: %s, pointer: %p, size: %d, offset: %d\n", strerror(errno), pMapBuffer, size, offset);
            return false;
		}

		if (NULL == pMapBuffer)
		{
            ALOGE("PolySpatial WriteBuffer failed to mmap buffer: %p, size: %d, offset: %d\n", pBuffer, size, offset);
			return false;
		}

		ALOGV("PolySpatial WriteBuffer: %p, size: %d, offset: %d\n", pMapBuffer, size, offset);

		memcpy(pMapBuffer, pBuffer, size);

		munmap(pMapBuffer, size);

        return true;
    }

    bool PolySpatialSharedMemoryAndroid::ReadBuffer(void* pBuffer, uint32_t size, uint32_t offset)
	{
        if (!IsValid())
			return false;

        char* pMapBuffer = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, m_fd, offset);
		if (MAP_FAILED == pMapBuffer)
		{
            ALOGE("PolySpatial ReadBuffer failed to mmap error: %s, pointer: %p, fd: %d, size: %d, offset: %d\n", strerror(errno), pMapBuffer, m_fd, size, offset);
			return false;
		}

		if (NULL == pMapBuffer)
		{
            ALOGE("PolySpatial ReadBuffer failed to mmap buffer: %p, size: %d, offset: %d\n", pBuffer, size, offset);
			return false;
		}

		ALOGV("PolySpatial ReadBuffer: %p, fd: %d, size: %d, offset: %d\n", pMapBuffer, m_fd, size, offset);

		memcpy(pBuffer, pMapBuffer, size);

        munmap(pMapBuffer, size);

        return true;
    }
}
#endif //ANDROID