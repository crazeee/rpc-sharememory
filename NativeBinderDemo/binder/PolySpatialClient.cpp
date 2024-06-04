#include "IPolySpatialService.h"


namespace android
{



	PolySpatialIpc::PolySpatialSharedMemoryAndroid *IPolySpatialService::m_sharedMemory = new PolySpatialIpc::PolySpatialSharedMemoryAndroid();
	void BpPolySpatialService::sayHello() {
		printf("BpPolySpatialService::sayHello\n");
		Parcel data, reply;
		data.writeInterfaceToken(IPolySpatialService::getInterfaceDescriptor());
		 
		int fd = open("/sdcard/window_dump.xml", O_RDWR);
		printf("open BpPolySpatialService fd = %d\n", fd);
		data.writeFileDescriptor(fd);
		sp<BnSubPolySpatialService> bn= new BnSubPolySpatialService();
		sp<IBinder> binder = BnSubPolySpatialService::asBinder(bn);
		data.writeStrongBinder(binder);

		remote()->transact(HELLO, data, &reply);
		printf("get num from BnPolySpatialService: %d\n", reply.readInt32());
	};


	void BpPolySpatialService::shareMemory() {
		printf("BpPolySpatialService::shareMemory\n");
		int fd = m_sharedMemory->RegisterMemory("shared_memory", 1024*1024*8);
		printf("shareMemory fd=%d,valid = %d\n",fd,isFdValid(fd));

		char buffer[1024*1024];
		memset(buffer,6,sizeof(char)*1024*1024);
		m_sharedMemory->WriteBuffer(buffer, 1024, 1024);
		printf("BpPolySpatialService::shareMemory buffer[0]=%d\n",buffer[1023]);
		Parcel data, reply;
		data.writeInterfaceToken(IPolySpatialService::getInterfaceDescriptor());
		data.writeFileDescriptor(fd);
		remote()->transact(MEMORY_SHARE, data, &reply);

	};


	void BnSubPolySpatialService::sayHello() {
		printf("BnSubPolySpatialService::sayHello\n");
		//Parcel data, reply;
		//remote()->transact(HELLO, data, &reply);
	};


	status_t BnSubPolySpatialService::onTransact(uint_t code, const Parcel& data,
			Parcel* reply, uint32_t flags) {
		switch (code) {
		case HELLO: {    
			printf("BnSubPolySpatialService:: got the client hello\n");
			//must check the interface
			CHECK_INTERFACE(IPolySpatialService, data, reply);
			sayHello();
			return NO_ERROR;
		}
			break;
		default:
			break;
		}
		return NO_ERROR;
	};




}

int main() {
	sp < IServiceManager > sm = defaultServiceManager(); // 
	sp < IBinder > binder = sm->getService(String16("service.PolySpatialService"));//

	if(binder){
		sp<IPolySpatialService> cs = interface_cast <IPolySpatialService> (binder);//
		cs->sayHello();
		//
		ProcessState::self()->startThreadPool();
		IPCThreadState::self()->joinThreadPool();
	}else{
		printf("BnPolySpatialService is null\n");
	}

	
	return 0;
}
