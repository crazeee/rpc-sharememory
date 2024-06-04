#pragma once

#include "Singleton.h"
#include "com.unity.polyspatial.ipc.h"
#include "ISharedMemory.h"
#ifdef _WIN32
#include "sharedmemory_windows.h"
#elif ANDROID
#include "sharedmemory_android.h"
#endif
#include <map>

namespace PolySpatialIpc
{
    class PolySpatialSharedMemoryManager : public Singleton<PolySpatialSharedMemoryManager>
    {
	public:
		PolySpatialSharedMemoryManager()
		{
			
		}

		~PolySpatialSharedMemoryManager()
		{
			for (auto var : m_memorymap)
			{
				m_memorymap.at(var.first)->Destroy();
			}
			m_memorymap.clear();
		}

		bool Initialize()
		{
#ifdef _WIN32
			return PolySpatialSharedMemoryWin::InitializeSharedMemory();
#elif ANDROID
			return PolySpatialSharedMemoryAndroid::InitializeSharedMemory();
#endif
				//LOG_ERROR("Local Memory initialize failed!");
		}

		int RegisterLocalMemory(const char* memName, uint32_t size, int sock)
		{
#ifdef _WIN32
			ISharedMemory* sm = new PolySpatialSharedMemoryWin();
#else
			ISharedMemory* sm = new PolySpatialSharedMemoryAndroid(sock);
#endif
			int fd = sm->RegisterMemory(memName, size);
			if (fd <= 0)
				return 0;
			auto result = m_memorymap.emplace(std::make_pair(fd, sm) );
			if (!result.second)
				return 0;

			return fd;
		}

		int OpenHostMemory(int fd)
		{
#ifdef _WIN32
			PolySpatialSharedMemoryWin* sma = new PolySpatialSharedMemoryWin();
#else
			PolySpatialSharedMemoryAndroid* sma = new PolySpatialSharedMemoryAndroid(fd);
#endif
			fd = sma->OpenMemory(fd);
			if (fd == 0)
				return 0;
			auto result = m_memorymap.emplace(std::make_pair(fd, sma));
			if (!result.second || !m_memorymap.at(fd)->IsValid())
				return 0;

			return fd;
		}

		void DestroyMemory(int fd)
		{
			if (m_memorymap.find(fd) == m_memorymap.end())
				return;

			m_memorymap.at(fd)->Destroy();
			m_memorymap.erase(fd);
		}

		bool WriteBuffer(int fd, void* pBuffer, uint32_t size, uint32_t offset)
		{
			return m_memorymap.at(fd)->WriteBuffer(pBuffer, size, offset);
		}

		bool ReadBuffer(int fd, void* pBuffer, uint32_t size, uint32_t offset)
		{
			return m_memorymap.at(fd)->ReadBuffer(pBuffer, size, offset);
		}

		bool HasValidMemory(int fd)
		{
			auto iter = m_memorymap.find(fd);
			if (iter == m_memorymap.end())
				return false;

			return iter->second->IsValid();
		}

	private:
		std::map<int, ISharedMemory*> m_memorymap;
    };
    
}
