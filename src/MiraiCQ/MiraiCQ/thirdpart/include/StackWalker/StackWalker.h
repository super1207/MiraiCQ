#pragma once
#include <Windows.h>
#include <tchar.h>

//该结构定义于Psapi.h 文件，但是这里为了不影响获取到的模块结果，因此不包含该文件
#define MAX_SYMBOL_PATH 1024
#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPMODULE 0x00000008
#define MAX_VERSION_LENGTH 512
#define STACKWALK_MAX_NAMELEN 1024

#define ASSERT(judge)\
	{\
		if(!(judge))\
		{\
			DebugBreak();\
		}\
	}

typedef struct tagMODULEENTRY32
{
	DWORD   dwSize;
	DWORD   th32ModuleID;  // This module
	DWORD   th32ProcessID; // owning process
	DWORD   GlblcntUsage;  // Global usage count on the module
	DWORD   ProccntUsage;  // Module usage count in th32ProcessID's context
	BYTE*   modBaseAddr;   // Base address of module in th32ProcessID's context
	DWORD   modBaseSize;   // Size in bytes of module starting at modBaseAddr
	HMODULE hModule;       // The hModule of this module in th32ProcessID's context
	TCHAR   szModule[MAX_MODULE_NAME32 + 1];
	TCHAR   szExePath[MAX_PATH];
} MODULEENTRY32;

typedef struct _MODULEINFO
{
	LPVOID lpBaseOfDll;
	DWORD  SizeOfImage;
	LPVOID EntryPoint;
} MODULEINFO, *LPMODULEINFO;

typedef MODULEENTRY32* PMODULEENTRY32;
typedef MODULEENTRY32* LPMODULEENTRY32;

typedef struct _tag_MODULE_INFO 
{
	DWORD64 ModuleAddress;
	DWORD dwModSize;
	TCHAR szModuleName[MAX_MODULE_NAME32 + 1];
	TCHAR szModulePath[MAX_PATH];
	TCHAR szSymbolPath[MAX_PATH];
	TCHAR szVersion[MAX_VERSION_LENGTH];
	struct _tag_MODULE_INFO* pNext;
}MODULE_INFO, *LPMODULE_INFO;

typedef struct tagSTACKINFO
{
	DWORD64 szFncAddr;
	TCHAR szFileName[MAX_PATH];
	TCHAR szFncName[MAX_PATH];
	unsigned long uFileNum;
	TCHAR    undName[STACKWALK_MAX_NAMELEN];
	TCHAR    undFullName[STACKWALK_MAX_NAMELEN];
	tagSTACKINFO *pNext;
}STACKINFO, *LPSTACKINFO;

class CStackWalker
{
public:
	CStackWalker(HANDLE hProcess = GetCurrentProcess(), WORD wPID = GetCurrentProcessId(), LPCTSTR lpSymbolPath = NULL);
	~CStackWalker(void);
	BOOL LoadSymbol();
	LPMODULE_INFO GetLoadModules();
	void GetModuleInformation(LPMODULE_INFO pmi);

	void FreeModuleInformations(LPMODULE_INFO pmi);
	virtual void OutputString(LPCTSTR lpszFormat, ...);

	LPSTACKINFO StackWalker(HANDLE hThread = GetCurrentThread(), const CONTEXT* context = NULL);
	void FreeStackInformations(LPSTACKINFO psi);

protected:
	LPMODULE_INFO GetModulesTH32();
	LPMODULE_INFO GetModulesPSAPI();
protected:
	HANDLE m_hProcess;
	WORD m_wPID;
	LPTSTR m_lpszSymbolPath;
	BOOL m_bSymbolLoaded;
};

#if defined(_M_IX86)
#ifdef CURRENT_THREAD_VIA_EXCEPTION
#define GET_CURRENT_THREAD_CONTEXT(c, contextFlags)\
do\
{\
	memset(&c, 0, sizeof(CONTEXT));\
	EXCEPTION_POINTERS* pExp = NULL;\
	__try\
	{\
		throw 0;\
	}\
	__except (((pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER:EXCEPTION_EXECUTE_HANDLER))\
	{\
	}\
	if (pExp != NULL)\
		memcpy(&c, pExp->ContextRecord, sizeof(CONTEXT));\
	c.ContextFlags = contextFlags;\
} while (0);
#else
#define GET_CURRENT_THREAD_CONTEXT(c, contextFlags) \
	do\
	{\
		memset(&c, 0, sizeof(CONTEXT));\
		c.ContextFlags = contextFlags;\
		__asm    call $+5\
		__asm    pop eax\
		__asm    mov c.Eip, eax\
		__asm    mov c.Ebp, ebp\
		__asm    mov c.Esp, esp\
} while (0)
#endif

#else
#define GET_CURRENT_THREAD_CONTEXT(c, contextFlags) \
	do\
	{ \
		memset(&c, 0, sizeof(CONTEXT));\
		c.ContextFlags = contextFlags;\
		RtlCaptureContext(&c);\
} while (0);
#endif