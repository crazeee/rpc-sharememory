#include "IPolySpatialService.h"
#include <thread>


namespace android{

	// void BpPolySpatialService::sayHello(){
	// };
    // void BpPolySpatialService::shareMemory(){

	// };
	PolySpatialIpc::PolySpatialSharedMemoryAndroid *IPolySpatialService::m_sharedMemory = new PolySpatialIpc::PolySpatialSharedMemoryAndroid();


	void BnPolySpatialService::sayHello() {
		printf("BnPolySpatialService::sayHello\n");
	};

	status_t BnPolySpatialService::onTransact(uint_t code, const Parcel& data,
			Parcel* reply, uint32_t flags) {
		switch (code) {
		case HELLO: {    
			printf("BnPolySpatialService:: got the client hello\n");
			CHECK_INTERFACE(IPolySpatialService, data, reply);
			int fd = data.readFileDescriptor();
			//getFileSize(fd);
			//printf("BnPolySpatialService:: readFileDescriptor fd=%d,isvalid= %d\n",fd,isFdValid(fd));
			sayHello();
			flags = NO_ERROR;
			//reply->writeInt32(2015);

			//getclient ibinder
			sp<IBinder> client = data.readStrongBinder();
			if(client.get()){
				printf("BnPolySpatialService:: readStrongBinder\n");
				sp<ISubPolySpatialService> subp = interface_cast <ISubPolySpatialService> (client);//
				subp->sayHello();
			}else{
				printf("BnPolySpatialService:: readStrongBinder =null \n");
			}
			//share memory
			//shareMemory();
			return NO_ERROR;
		}
			break;
		case MEMORY_SHARE:{
			printf("BnPolySpatialService:: got the client memory share\n");
			CHECK_INTERFACE(IPolySpatialService, data, reply);
			int fd = data.readFileDescriptor();
			int duped_fd = dup(fd);
			std::thread t1(readMemory,duped_fd);
			t1.detach();
			//readMemory(fd);
			sleep(5);
			break;
		}
		default:
			break;
		}
		return NO_ERROR;
	};

	void BnPolySpatialService::readMemory(int fd){
		for(int i =0;i<10;i++){
			printf("readMemory fd=%d\n",fd);
			m_sharedMemory->OpenMemory(fd);
			char buffer[1024];
			memset(buffer,8,sizeof(char)*1024);
			printf("readMemory data[0]=%d\n",buffer[1023]);
			m_sharedMemory->ReadBuffer(buffer,1024,4096);
			printf("readMemory data[0]=%d\n",buffer[1023]);
			sleep(2);
		}
	};

	void BnPolySpatialService::shareMemory() {
		printf("BnPolySpatialService::shareMemory\n");
		int fd = m_sharedMemory->RegisterMemory("shared_memory", 1024*1024);
		printf("shareMemory fd=%d,valid = %d\n",fd,isFdValid(fd));
	};




	void BpSubPolySpatialService::sayHello(){
		Parcel data, reply;
		remote()->transact(HELLO, data, &reply);
	};
}

int main() {
	sp < IServiceManager > sm = defaultServiceManager();
	BnPolySpatialService* service =  new BnPolySpatialService();
	sm->addService(String16("service.PolySpatialService"),service);

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
	return 0;
}
