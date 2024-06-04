#pragma once

#ifdef _WIN32
#include "ISharedMemory.h"
#include <windows.h>
#include <cstdint>
#include <string>

namespace PolySpatialIpc
{
    class PolySpatialSharedMemoryWin : public ISharedMemory
    {
	public:
		PolySpatialSharedMemoryWin() : m_hMapFile(NULL), m_MapAccess(0) {}
		PolySpatialSharedMemoryWin(const char* memName, uint32_t size) : m_hMapFile(NULL), m_MapAccess(0) {
			RegisterMemory(memName, size);
		}
		~PolySpatialSharedMemoryWin() {
			Destroy();
		}

		static bool InitializeSharedMemory();

		virtual int RegisterMemory(const char* memName, uint32_t size) override;

		virtual int OpenMemory(int fd) override;

		virtual bool IsValid() const  override {
			return m_hMapFile != NULL;
		}

		virtual void Destroy() override;

		virtual bool WriteBuffer(void* pBuffer, uint32_t size, uint32_t offset) override;

		virtual bool ReadBuffer(void* pBuffer, uint32_t size, uint32_t offset) override;

	protected:

	private:
		HANDLE m_hMapFile;
		DWORD m_MapAccess;
    };
    
}
#endif //_WIN32