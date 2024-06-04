#ifndef POLYSPATIAL_SERVICE
#define POLYSPATIAL_SERVICE

#include <stdio.h>
#include <sys/stat.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/IBinder.h>
#include <binder/Binder.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include "../sharedmemory/sharedmemory_android.h"

using namespace android;
namespace android
{
    class IPolySpatialService : public IInterface
    {
      public:
        static PolySpatialIpc::PolySpatialSharedMemoryAndroid *m_sharedMemory;
        DECLARE_META_INTERFACE(PolySpatialService); 


        virtual void sayHello()=0; //
        virtual void shareMemory()=0;
        long getFileSize(int fd) {
          struct stat file_stat;
 
          // 获取文件描述符的状态信息
          if (fstat(fd, &file_stat) == -1) {
            perror("fstat");
            return -1;
          }
 
          // 打印文件的大小（单位：字节）
          printf("File size: %ld bytes\n", file_stat.st_size);
          return file_stat.st_size;
        };
        bool isFdValid(int fd) {
          // 尝试一个无风险的操作来检查文件描述符的有效性
          // 使用F_GETFL获取文件状态标志
          if (fcntl(fd, F_GETFL) == -1 && errno == EBADF) {
          // EBADF表示文件描述符无效
            return false;
          }
          return true;
	      };

    };


   




    
    class BpPolySpatialService: public BpInterface<IPolySpatialService> {
    public:
      BpPolySpatialService(const sp<IBinder>& impl):BpInterface<IPolySpatialService>(impl) {
      }
    	virtual void sayHello();
      virtual void shareMemory();
    };

		//
		class BnPolySpatialService: public BnInterface<IPolySpatialService> {
		public:
      BnPolySpatialService(){
      }
			status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
					uint32_t flags = 0);
			virtual void sayHello();
      virtual void shareMemory();
      static void readMemory(int fd);
		};




     class ISubPolySpatialService : public IInterface
    {
      public:
        //static PolySpatialIpc::PolySpatialSharedMemoryAndroid *m_sharedMemory;
        DECLARE_META_INTERFACE(SubPolySpatialService); 
        virtual void sayHello()=0; //
        //virtual void shareMemory()=0;
    };


    enum
    {
        HELLO = 1,
        MEMORY_SHARE = 2,
    };


    class BpSubPolySpatialService: public BpInterface<ISubPolySpatialService> {
    public:
      BpSubPolySpatialService(const sp<IBinder>& impl):BpInterface<ISubPolySpatialService>(impl) {
      }
    	virtual void sayHello();
      //virtual void shareMemory();
    };

		//
		class BnSubPolySpatialService: public BnInterface<ISubPolySpatialService> {
		public:
      BnSubPolySpatialService(){
      }
			status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
					uint32_t flags = 0);
			virtual void sayHello();
      //virtual void shareMemory();
      //static void readMemory(int fd);
		};

}

#endif