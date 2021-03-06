#include "stdafx.h"
#include "../LibX.h"
#include <atlbase.h> 

#include <psapi.h>
#pragma comment(lib,"psapi.lib")////SDK6.0,不知道为什么vc6好像没有自带这个头文件？？

// 请保存为Unicode字符
#pragma warning(disable:4312 4311)

DWORD CLibX::Memory::GetProcessID( __in_opt LPCTSTR lpClassName, __in_opt LPCTSTR lpWindowName )
{
	HWND hWnd = ::FindWindow(lpClassName,lpWindowName);
	if (hWnd == NULL) return NULL;
	DWORD dwProcessID;
	::GetWindowThreadProcessId(hWnd,&dwProcessID);
	return dwProcessID;
}


LONG_PTR CLibX::Memory::WriteProcessMemory( __in const DWORD nProcessID, __in DWORD lpBaseAddress, __in LPVOID lpBuffer, __in int nSize /*= -1 */ )
{
	DWORD dwOldProtect;

	ULONG_PTR  lpNumberOfBytesWritten_ = 0;
	HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,nProcessID);
	VirtualProtectEx(hHandle,(LPVOID)lpBaseAddress,sizeof(lpBuffer),PAGE_READWRITE,&dwOldProtect);
	int nResult = ::WriteProcessMemory(hHandle,(LPVOID)lpBaseAddress,&lpBuffer,nSize == -1 ? sizeof(lpBuffer) : nSize,&lpNumberOfBytesWritten_);
	VirtualProtectEx(hHandle,(LPVOID)lpBaseAddress,sizeof(lpBuffer),dwOldProtect,&dwOldProtect);
	return lpNumberOfBytesWritten_;
}


LONG_PTR CLibX::Memory::WriteProcessMemory( __in const DWORD nProcessID, __in DWORD lpBaseAddress, __in LPVOID lpBuffer, __in int nSize , __in int nOffset, ... )
{
	va_list arg_ptr;
	va_start(arg_ptr,nOffset);
	DWORD nFailue = 0;
	ULONG_PTR  lpNumberOfBytesWritten_ = 0;
	DWORD dwUpMemory = ReadProcessMemory(nProcessID,lpBaseAddress,(DWORD)nFailue);
	for (int i=0; i<nOffset; i++)
	{
		long nOffsetOne = va_arg(arg_ptr,long);
		if (i+1 == nOffset) lpNumberOfBytesWritten_ = WriteProcessMemory(nProcessID,dwUpMemory + nOffsetOne,lpBuffer,nSize);
		else dwUpMemory = ReadProcessMemory(nProcessID,dwUpMemory + nOffsetOne,(DWORD)nFailue);
	}
	return lpNumberOfBytesWritten_;
}


DWORD CLibX::Memory::GetProcessModuleAddr( __in const DWORD nProcessID )
{
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,false,nProcessID); 
	if(hProcess==INVALID_HANDLE_VALUE) 
	{ 
		// printf(" open process failed!\n"); 
		return NULL; 
	}
	DWORD dwModuleAddress = NULL;
	DWORD size=0,ret=0; 
	EnumProcessModules(hProcess,NULL,size,&ret); 
	HMODULE *parry=(HMODULE*)malloc(ret+4); 
	memset(parry,0,ret+4); 
	if(EnumProcessModules(hProcess,parry,ret+4,&ret)) 
	{ 
		// 		wchar_t* path=new wchar_t[MAX_PATH]; 
		// 		memset(path,0,MAX_PATH); 
		// 		UINT i=0; 
		// 
		// 		while(GetModuleFileNameEx(hProcess,parry[i],path,MAX_PATH)) 
		// 		{ 
		// 			return 
		// 			// CString strTemp;
		// 			// strTemp.Format(_T("方法3模块：%s,%08x\n"),path,parry[i]);
		// 			// AfxMessageBox(strTemp);
		// 			// printf("方法3模块：%s\n",path); 
		// 			memset(path,0,MAX_PATH); 
		// 			i++; 
		// 		} 
		// 		delete path; 

	} 
	dwModuleAddress = (DWORD)parry[0];
	free(parry); 

	CloseHandle(hProcess); 
	return dwModuleAddress;
}


#pragma warning(default:4312 4311)
