#include "stdafx.h"
#include "../LibX.h"

#include <shellapi.h>
#include <atlenc.h>
#include <mmsystem.h>
#pragma comment(lib,"WINMM.LIB")
#pragma comment(lib, "Version.lib")

void CLibX::System::SuperSleep( __in int nTime,__in int nSpeed /*= 0*/ )
{
	LARGE_INTEGER lar;
	if (nSpeed == 0) nSpeed = 1;
	else if (nSpeed == 1) nSpeed = -10;
	else if (nSpeed == 2) nSpeed = 1000;
	else if (nSpeed == 3) nSpeed = 1000 * 60; 
	else if (nSpeed == 4) nSpeed = 1000 * 60 * 60; 
	else if (nSpeed == 5) nSpeed = 1000 * 60 * 60 * 24; 

	lar.QuadPart = -10 * nTime * nSpeed * 1000 ;
	HANDLE hTimer = CreateWaitableTimer(NULL,FALSE,NULL);
	SetWaitableTimer(hTimer,&lar,NULL,NULL,NULL,FALSE);
	while (MsgWaitForMultipleObjects(1,&hTimer,FALSE,INFINITE,QS_ALLINPUT) != WAIT_OBJECT_0)
	{
		CLibX::System::ProcessEvent();
	}

	CloseHandle(hTimer);
}

void CLibX::System::ProcessEvent()
{
	MSG msg;
	while(PeekMessage(&msg,NULL,NULL,NULL,1) != 0)
	{
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
}

lstring CLibX::System::GetErrorMsg( __in const DWORD dwError )
{
	lchar* pBuffer = NULL;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(lchar*)&pBuffer,
		DEFAULT_MAX_STRING, NULL );

	return pBuffer;
}

BOOL CLibX::System::isAdministrator()
{
	BOOL bIsElevated = FALSE;
	HANDLE hToken    = NULL;
	UINT16 uWinVer   = LOWORD(GetVersion());
	uWinVer = MAKEWORD(HIBYTE(uWinVer),LOBYTE(uWinVer));
	if (uWinVer < 0x0600) return TRUE;
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken))
	{
		struct{
			DWORD TokenIsElevated;
		}te;
		DWORD dwReturnLength = 0;
		if (GetTokenInformation(hToken,(_TOKEN_INFORMATION_CLASS)20,&te,sizeof(te),&dwReturnLength))
		{
			if (dwReturnLength == sizeof(te))
				bIsElevated = te.TokenIsElevated;
		}
		CloseHandle(hToken);
	}
	return bIsElevated;
}

BOOL CLibX::System::runAsAdministrator( __in LPCTSTR lpExeFile )
{
	SHELLEXECUTEINFO SEI = {sizeof(SHELLEXECUTEINFO)};
	SEI.lpVerb = _T("runas");
	SEI.lpFile = lpExeFile;
	SEI.nShow = SW_SHOWNORMAL;
	return ShellExecuteEx(&SEI);
}

lstring CLibX::System::GetComputerName()
{
	DWORD nSize = DEFAULT_MAX_STRING;
	lchar szComputeName[DEFAULT_MAX_STRING] = {0};
	::GetComputerName(szComputeName,&nSize);
	return szComputeName;
}

lstring CLibX::System::GetLoginUserName()
{
	DWORD nSize = DEFAULT_MAX_STRING ;
	lchar szUserName[DEFAULT_MAX_STRING] = {0};
	::GetUserName(szUserName,&nSize);
	return szUserName;
}

HINSTANCE CLibX::System::OpenURLWithIE( __in const LPCTSTR szURL )
{
	return ShellExecute(NULL,_T("open"),_T("explorer.exe"),szURL,NULL,SW_SHOWNORMAL);
}

HINSTANCE CLibX::System::OpenURL( __in const LPCTSTR szURL )
{
	return ShellExecute(NULL,_T("open"),szURL,NULL,NULL,SW_SHOWNORMAL);
}

BOOL CLibX::System::OpenDVD()
{
	return mciSendString(_T("set CDAudio door open wait"),NULL,0,NULL) == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL CLibX::System::CloseDVD()
{
	return mciSendString(_T("set CDAudio door close wait"),NULL,0,NULL) == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL CLibX::System::is64Bit_OS()
{
	typedef void (WINAPI *LPFN_PGNSI)(LPSYSTEM_INFO);
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_PGNSI pGNSI = (LPFN_PGNSI ) GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")),  "GetNativeSystemInfo");
	LPFN_ISWOW64PROCESS  fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	BOOL bIsWow64 = FALSE;
	fnIsWow64Process(GetCurrentProcess(), &bIsWow64);

	return bIsWow64;
}

lstring CLibX::System::RunCommandResult( __in const lstring strParamt )
{
	lstring strResult = _T("");
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (CreatePipe(&hRead,&hWrite,&sa,0) == FALSE) return FALSE;

	lstring strCommandFull = CLibX::File::GetSystemPath();
	strCommandFull.append(_T("system32\\cmd.exe /c "));
	strCommandFull.append(strParamt);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if (::CreateProcess(NULL,(lchar*)strCommandFull.c_str(),NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi) == FALSE)
	{
		CloseHandle(hWrite);
		CloseHandle(hRead);
		return FALSE;
	}

	CloseHandle(hWrite);

	lchar buffer[4095] = {0};
	DWORD bytesRead = 0;
	while (true)
	{
		ReadFile(hRead,buffer,4095,&bytesRead,NULL);
		if (bytesRead > 0 )
			strResult = strResult + buffer;
		else
			break;
	}

	CloseHandle(hRead);

	return strResult;
}

DWORD CLibX::System::OnlyRunOne( __in HWND hWnd, __in_opt LPCTSTR lpTager,__in BOOL isShow /*= TRUE */,__in BOOL isAutoExit /*= TRUE*/ )
{
	lstring strTager = CLibX::string_format(_T("CLibX::RunOnlyOn::%s"),lpTager);
	HANDLE hSem = CreateSemaphore(NULL,1,1,strTager.c_str());
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hSem);
		if (isShow == TRUE)
		{
			HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(),GW_CHILD) ;
			while(::IsWindow(hWndPrevious))
			{
				if (::GetProp(hWndPrevious,strTager.c_str()) != NULL)
				{
					if (::IsIconic(hWndPrevious) == TRUE)	::ShowWindow(hWndPrevious,SW_RESTORE);
					::SetForegroundWindow(hWndPrevious);
					::SetForegroundWindow(::GetLastActivePopup(hWndPrevious));
					break;
				}
				hWndPrevious = ::GetWindow(hWndPrevious,GW_HWNDNEXT);
			}
		}
		if (isAutoExit == TRUE) ExitProcess(ERROR_SUCCESS);
		return ERROR_ALREADY_EXISTS ;
	}

	return ::SetProp(hWnd,strTager.c_str(),(HANDLE)1) ;
}

DWORD CLibX::System::OnlyRunOne_Delete( __in HWND hWnd, __in_opt LPCTSTR lpTager )
{
#pragma warning(disable:4311)
	lstring strTager = CLibX::string_format(_T("LibX::RunOnlyOn::%s"),lpTager);
	return (DWORD)::RemoveProp(hWnd,strTager.c_str());
#pragma warning(default:4311)
}

OSVERSIONINFO CLibX::System::GetSystemVersion()
{
	OSVERSIONINFO VersionInformation ;
	DWORD dwVerOff = 0 , osKind = -1 ;

	// 版本判断
	memset(&VersionInformation, 0, sizeof(VersionInformation));
	VersionInformation.dwOSVersionInfoSize = sizeof(VersionInformation) ;
	GetVersionEx(&VersionInformation) ;
	return VersionInformation;
}

lstring CLibX::System::GetSystemVersionName( __in OSVERSIONINFO* ovs /*= NULL*/,__in const LPCTSTR lpSpase /*= _T(" ") */ )
{
	OSVERSIONINFO OS = ovs == NULL ? GetSystemVersion() : *ovs;
	lstring strReturn = _T("[Undefined OS version]");
	if (OS.dwMajorVersion == 5)
	{
		switch (OS.dwMinorVersion)
		{
		case 0:strReturn = _T("Windows 2000");break;
		case 1:strReturn = _T("Windows XP");break;
		case 2:strReturn = _T("Windows Server 2003");break;
		}
	}
	else if (OS.dwMajorVersion == 6)
	{
		switch (OS.dwMinorVersion)
		{
		case 0:strReturn = _T("Windows Vista");break;
		case 1:strReturn = _T("Windows 7");break;
		case 2:strReturn = _T("Windows 8");break;
		case 3:strReturn = _T("Windows 8.1 Preview");break;
		}
	}

	strReturn = CLibX::String::string_replace(strReturn,_T(" "),lpSpase);
	return strReturn;
}

HINSTANCE CLibX::System::OpenAndSelectFile( __in const LPCTSTR lpszFile )
{
	lstring szParam = _T("/select, ")	;
	szParam.append(lpszFile);

	return ShellExecute(NULL,_T("open"),_T("explorer"),szParam.c_str(),NULL,SW_SHOW);
}

lstring CLibX::Item::getExeVersion( __in const HMODULE hModule )
{
	lchar cPath[MAX_PATH];
	lstring strVersion;

	DWORD dwHandle,InfoSize;

	::GetModuleFileName(hModule,cPath ,sizeof(cPath)); //首先获得版本信息资源的长度
	InfoSize = GetFileVersionInfoSize(cPath,&dwHandle); //将版本信息资源读入缓冲区
	if(InfoSize == 0) return _T("");

	lchar *InfoBuf = new lchar[InfoSize];

	GetFileVersionInfo(cPath,0,InfoSize,InfoBuf); //获得生成文件使用的代码页及文件版本
	unsigned int  cbTranslate = 0;
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	VerQueryValue(InfoBuf, TEXT("\\VarFileInfo\\Translation"),(LPVOID*)&lpTranslate,&cbTranslate);

	for( unsigned int i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
	{
		lchar  SubBlock[MAX_PATH];
		wsprintf( SubBlock, TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);

		void *lpBuffer=NULL;
		unsigned int dwBytes=0;
		VerQueryValue(InfoBuf, SubBlock, &lpBuffer, &dwBytes);

		strVersion.append((lchar*)lpBuffer) ;
	}

	delete[] InfoBuf;

	return strVersion;
}

int CLibX::System::GetScreenHeight()
{
	return GetSystemMetrics( SM_CYSCREEN );
}

int CLibX::System::GetScreenWidth()
{
	return GetSystemMetrics( SM_CXSCREEN ); 
}
