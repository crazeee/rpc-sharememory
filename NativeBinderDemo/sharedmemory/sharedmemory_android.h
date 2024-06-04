#pragma once

#ifdef ANDROID
#include "ISharedMemory.h"
#include <cstdint>
#include <string>

namespace PolySpatialIpc
{
    class PolySpatialSharedMemoryAndroid : public ISharedMemory
    {
	public:
		PolySpatialSharedMemoryAndroid() :  m_fd(0){}
		~PolySpatialSharedMemoryAndroid() {
			Destroy();
		}

		static bool InitializeSharedMemory() {
			return true;
		}

		virtual int RegisterMemory(const char* memName, uint32_t size) override;

		virtual int OpenMemory(int fd) override;

		virtual bool IsValid() const  override {
			return m_fd > 0;
		}

		virtual void Destroy() override;

		virtual bool WriteBuffer(void* pBuffer, uint32_t size, uint32_t offset) override;

		virtual bool ReadBuffer(void* pBuffer, uint32_t size, uint32_t offset) override;

	private:
		int m_fd = 0;
    };
    
}
#endif //ANDROID