#ifdef _WIN32
#include "sharedmemory_windows.h"
#include <tchar.h>
#include <stdio.h>
#include <atlstr.h>
#include <assert.h>
#include <sstream>

namespace PolySpatialIpc
{
    typedef int (*GETLARGEPAGEMINIMUM)(void);

    static void DisplayError(const wchar_t* pszAPI, DWORD dwError)
    {
        LPVOID lpvMessageBuffer;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, dwError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpvMessageBuffer, 0, NULL);

        //... now display this string
        _tprintf(TEXT("ERROR: API        = %s\n"), pszAPI);
        _tprintf(TEXT("       error code = %d\n"), dwError);
        _tprintf(TEXT("       message    = %s\n"), lpvMessageBuffer);

        // Free the buffer allocated by the system
        LocalFree(lpvMessageBuffer);

        //ExitProcess(GetLastError());
    }

    void Privilege(const wchar_t* pszPrivilege, BOOL bEnable)
    {
        HANDLE           hToken;
        TOKEN_PRIVILEGES tp;
        BOOL             status;
        DWORD            error;

        // open process token
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            DisplayError(TEXT("OpenProcessToken"), GetLastError());

        // get the luid
        if (!LookupPrivilegeValue(NULL, pszPrivilege, &tp.Privileges[0].Luid))
            DisplayError(TEXT("LookupPrivilegeValue"), GetLastError());

        tp.PrivilegeCount = 1;

        // enable or disable privilege
        if (bEnable)
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        else
            tp.Privileges[0].Attributes = 0;

        // enable or disable privilege
        status = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

        // It is possible for AdjustTokenPrivileges to return TRUE and still not succeed.
        // So always check for the last error value.
        error = GetLastError();
        if (!status || (error != ERROR_SUCCESS))
            DisplayError(TEXT("AdjustTokenPrivileges"), GetLastError());

        // close the handle
        if (!CloseHandle(hToken))
            DisplayError(TEXT("CloseHandle"), GetLastError());
    }

    bool PolySpatialSharedMemoryWin::InitializeSharedMemory()
    {
        // call succeeds only on Windows Server 2003 SP1 or later
		HINSTANCE  hDll = LoadLibrary(TEXT("kernel32.dll"));
		if (hDll == NULL)
		{
			DisplayError(TEXT("LoadLibrary"), GetLastError());
			return false;
		}

		GETLARGEPAGEMINIMUM pGetLargePageMinimum;

		pGetLargePageMinimum = (GETLARGEPAGEMINIMUM)GetProcAddress(hDll,
			"GetLargePageMinimum");
		if (pGetLargePageMinimum == NULL)
			DisplayError(TEXT("GetProcAddress"), GetLastError());

		DWORD largePageMimSize = (*pGetLargePageMinimum)();

		FreeLibrary(hDll);

		_tprintf(TEXT("Large Page Minimum Size: %u\n"), largePageMimSize);

		SYSTEM_INFO siSysInfo;
		GetSystemInfo(&siSysInfo);
		printf("System  Allocation Granularity: %u\n",
			siSysInfo.dwAllocationGranularity);

        return true;
    }

	int strHash(const char* str)
	{
		int h = 0;
		while (*str)
			h = h << 1 ^ *str++;
		return h;
	}

    std::string IntToChar(int num)
	{
		std::stringstream strs;
		strs << num;
		return strs.str();
	}

    int PolySpatialSharedMemoryWin::RegisterMemory(const char* memName, uint32_t size)
    {
        Privilege(TEXT("SeLockMemoryPrivilege"), TRUE);

        int fd = strHash(memName);
        std::string szName = IntToChar(fd);

        m_hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,    // use paging file
            NULL,                    // default security
            PAGE_READWRITE | SEC_COMMIT | SEC_LARGE_PAGES,// PAGE_READWRITE
            0,                       // max. object size
            size,                    // buffer size
            CA2T(szName.c_str()));                 // name of mapping object

        Privilege(TEXT("SeLockMemoryPrivilege"), FALSE);

        if (m_hMapFile == NULL)
        {
            DisplayError(TEXT("CreateFileMapping"), GetLastError());
            return 0;
        }

        _tprintf(TEXT("File mapping object successfully created.\n"));

        m_MapAccess = FILE_MAP_WRITE;// | FILE_MAP_LARGE_PAGES;

        return fd;
    }

    int PolySpatialSharedMemoryWin::OpenMemory(int fd)
    {
        std::string szName = IntToChar(fd);
        m_hMapFile = OpenFileMapping(
            FILE_MAP_READ,   // read/write access
            FALSE,                 // do not inherit the name
            CA2T(szName.c_str()));               // name of mapping object

        if (m_hMapFile == NULL)
        {
            _tprintf(TEXT("Could not open file mapping object (%d).\n"),
                GetLastError());
            return 0;
        }

        m_MapAccess = FILE_MAP_READ;

        return fd;
    }

    void PolySpatialSharedMemoryWin::Destroy()
    {
        // do nothing, clean up an exit
        if (m_hMapFile)
        {
			CloseHandle(m_hMapFile);
			m_hMapFile = nullptr;
        }
    }

    bool PolySpatialSharedMemoryWin::WriteBuffer(void* pBuffer, uint32_t size, uint32_t offset)
	{
        if (!IsValid())
            return false;

		assert(0 != (m_MapAccess & FILE_MAP_WRITE));

        LPCTSTR pBuf = (LPTSTR)MapViewOfFile(m_hMapFile,          // handle to map object
            m_MapAccess,
			0, offset,// file offset zero
			size);// map the whole thing

		if (pBuf == NULL)
		{
			DisplayError(TEXT("MapViewOfFile"), GetLastError());
            return false;
		}

        CopyMemory((PVOID)pBuf, pBuffer, size);

        UnmapViewOfFile(pBuf);

        return true;
    }

    bool PolySpatialSharedMemoryWin::ReadBuffer(void* pBuffer, uint32_t size, uint32_t offset)
	{
        if (!IsValid())
			return false;

        assert(0 != (m_MapAccess & FILE_MAP_READ));

		LPCTSTR pBuf = (LPTSTR)MapViewOfFile(m_hMapFile,          // handle to map object
			m_MapAccess,
			0, offset,// file offset zero
			size);// map the whole thing

		if (pBuf == NULL)
		{
			DisplayError(TEXT("MapViewOfFile"), GetLastError());
			return false;
		}

        CopyMemory(pBuffer, (PVOID)pBuf, size);

        UnmapViewOfFile(pBuf);

        return true;
    }
}
#endif //_WIN32