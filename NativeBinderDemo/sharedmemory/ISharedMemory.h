#pragma once

#include <cstdint>
#include <string>

namespace PolySpatialIpc
{
    class ISharedMemory
    {
	public:
		virtual ~ISharedMemory() {}
		virtual int RegisterMemory(const char* memName, uint32_t size) = 0;

		virtual int OpenMemory(int fd) = 0;

		virtual bool IsValid() const = 0;

		virtual void Destroy() = 0;

		virtual bool WriteBuffer(void* pBuffer, uint32_t size, uint32_t offset) = 0;

		virtual bool ReadBuffer(void* pBuffer, uint32_t size, uint32_t offset) = 0;
    };
    
}
