#pragma once

#include "stdafx.h"
#include "../LibX.h"

#include <wincrypt.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <vector>
#include <atlconv.h>
#include <shellapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "shell32.lib")

#ifdef _UNICODE
	#define MY_T2A T2A	// USES_CONVERSION;MY_T2A
#else
	#define MY_T2A 	
#endif // _UNICODE

// 请保存为Unicode字符
#pragma warning(disable:4312 4311)

LONGLONG CLibX::File::GetFileSize( __in const LPCTSTR szFilePath )
{
	USES_CONVERSION;
	HFILE  nFile = _lopen(MY_T2A(szFilePath),0);

	PLARGE_INTEGER  nFileSzie = 0;
	GetFileSizeEx((HANDLE)nFile,nFileSzie);
	CloseHandle((HANDLE)nFile);

	return nFileSzie->QuadPart;
}

lstring CLibX::File::GetFileName( __in const LPCTSTR szFile )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	_tcscpy_s(szChar,DEFAULT_MAX_STRING,szFile);
	PathStripPath(szChar);
	return szChar;
}

lstring CLibX::File::GetFileFolder( __in const LPCTSTR szFile )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	_tcscpy_s(szChar,DEFAULT_MAX_STRING,szFile);
	PathRemoveFileSpec(szChar);
	PathAddBackslash(szChar);
	return szChar;
}

lstring CLibX::File::GetFileExtension( __in const LPCTSTR szFile )
{
	return PathFindExtension(szFile);
}


lstring CLibX::File::GetFileMd5( __in const LPCTSTR szFilePath )
{
	lstring szReturn = _T("");

	HANDLE hFile = ::CreateFile(szFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)                                        //如果CreateFile调用失败  
	{  
		CloseHandle(hFile);  
		return szReturn;
	}

	HCRYPTPROV hProv=NULL;  
	if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //获得CSP中一个密钥容器的句柄  
	{  
		return szReturn;
	} 

	HCRYPTPROV hHash=NULL;  
	if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)     //初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被CryptHashData调用。  
	{  
		return szReturn;
	}

	DWORD dwFileSize=::GetFileSize(hFile,0);    //获取文件的大小  
	if (dwFileSize==0xFFFFFFFF)               //如果获取文件大小失败  
	{  
		return szReturn;
	}  
	byte* lpReadFileBuffer=new byte[dwFileSize];  
	DWORD lpReadNumberOfBytes;  
	if (ReadFile(hFile,lpReadFileBuffer,dwFileSize,&lpReadNumberOfBytes,NULL)==0)        //读取文件  
	{  
		return szReturn;
	}  
	if(CryptHashData(hHash,lpReadFileBuffer,lpReadNumberOfBytes,0)==FALSE)      //hash文件  
	{  
		return szReturn; 
	}  

	delete[] lpReadFileBuffer;  
	CloseHandle(hFile);          //关闭文件句柄  
	// 

	BYTE *pbHash;  
	DWORD dwHashLen=sizeof(DWORD);

	if (CryptGetHashParam(hHash,HP_HASHVAL,NULL,&dwHashLen,0) == NULL)      //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn          
	{  
		return szReturn; 
	}  
	pbHash = (byte*)malloc(dwHashLen);  

	if(CryptGetHashParam(hHash,HP_HASHVAL,pbHash,&dwHashLen,0))            //获得md5值  
	{  
		lchar szd[10] = {0};
		for(DWORD i=0;i<dwHashLen;i++)         //输出md5值  
		{  
			wsprintf(szd,_T("%02x"),pbHash[i]);
			szReturn.append(szd);
		}  
	}  
	//善后工作  
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv,0);

	return szReturn;
}


int CLibX::File::MoveFile( __in const LPCTSTR lpExistingFileName,__in const LPCTSTR lpNewFileName )
{
	SHFILEOPSTRUCT shf;
	shf.wFunc = FO_MOVE;
	shf.pFrom = lpExistingFileName;
	shf.pTo = lpNewFileName;
	shf.fFlags = FOF_ALLOWUNDO;
	return SHFileOperation(&shf);
}


int CLibX::File::CopyFile( __in const LPCTSTR lpExistingFileName, __in const LPCTSTR lpNewFileName, __in const BOOL bFailIfExists )
{
	if (bFailIfExists == TRUE && IsExist(lpExistingFileName) == TRUE) return ERROR_FILE_EXISTS;
	SHFILEOPSTRUCT shf;
	shf.wFunc = FO_COPY;
	shf.pFrom = lpExistingFileName;
	shf.pTo = lpNewFileName;
	shf.fFlags = FOF_ALLOWUNDO;
	return SHFileOperation(&shf);
}


int CLibX::File::DeleFile( __in const LPCTSTR lpFileName )
{
	SHFILEOPSTRUCT shf;
	shf.wFunc = FO_DELETE;
	shf.pFrom = lpFileName;
	shf.fFlags = FOF_ALLOWUNDO;
	return SHFileOperation(&shf);
}

int CLibX::File::Rename( __in const LPCTSTR lpExistingFileName,__in const LPCTSTR lpNewFileName )
{
	SHFILEOPSTRUCT shf;
	shf.wFunc = FO_RENAME;
	shf.pFrom = lpExistingFileName;
	shf.pTo = lpNewFileName;
	shf.fFlags = FOF_ALLOWUNDO;
	return SHFileOperation(&shf);
}


BOOL CLibX::File::IsExist( __in const LPCTSTR lpFilePath )
{
	DWORD dwAttributes = ::GetFileAttributes(lpFilePath);
	return (dwAttributes != ((DWORD)-1)) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

BOOL CLibX::File::DirectoryIsExist( __in const LPCTSTR lpDirectory )
{
	DWORD dwAttributes = ::GetFileAttributes(lpDirectory);
	return (dwAttributes != ((DWORD)-1)) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}


int CLibX::File::CreateFile( __in const LPCTSTR lpNewFileName,__in int iAttribute /*= 0*/ )
{
	USES_CONVERSION;
	return _lcreat(MY_T2A(lpNewFileName),iAttribute);
}


int CLibX::File::Run( __in const LPCTSTR lpCommand,__in UINT uCmdShow )
{
	USES_CONVERSION;
	return WinExec(MY_T2A(lpCommand),uCmdShow);
}


lstring CLibX::File::GetSystemPath()
{
	lchar szChar[DEFAULT_MAX_STRING];
	GetWindowsDirectory(szChar,DEFAULT_MAX_STRING);
	PathAddBackslash(szChar);

	return szChar;
}


lstring CLibX::File::GetSystemPath( __in int nIndex )
{
	lchar szChar[DEFAULT_MAX_STRING];
	LPITEMIDLIST PID;
	SHGetSpecialFolderLocation(NULL,nIndex,&PID);
	SHGetPathFromIDList(PID,szChar);
	PathAddBackslash(szChar);
	return szChar;
}


lstring CLibX::File::GetRunPath()
{
	lchar szChar[DEFAULT_MAX_STRING];
	::GetModuleFileName(NULL, szChar, DEFAULT_MAX_STRING);
	PathRemoveFileSpec(szChar);
	PathAddBackslash(szChar);
	return szChar;
}


lstring CLibX::File::GetExeRunPath()
{
	lchar szChar[DEFAULT_MAX_STRING];
	::GetModuleFileName(NULL, szChar, DEFAULT_MAX_STRING);
	return szChar;
}


lstring CLibX::File::GetExeName()
{
	return PathFindFileName(GetExeRunPath().c_str());
}


lstring CLibX::File::GetExeNameNoExtension()
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	_tcscpy_s(szChar,DEFAULT_MAX_STRING,CLibX::File::GetExeName().c_str());
	PathRemoveExtension(szChar);
									   
	return szChar;
}

													
lstring CLibX::File::GetAppPath( __in LPCTSTR lpExtension /*= _T("")*/ )
{
	lstring strAppPath = CLibX::File::GetRunPath();
	strAppPath.append(CLibX::File::GetExeNameNoExtension());
	strAppPath.append(lpExtension);
	return strAppPath;
}


BOOL CLibX::File::CreateMultipleDirectory( __in const lstring szPath )
{
	BOOL bSuccess = FALSE;

	lchar szChar[DEFAULT_MAX_STRING] = {0};
	_tcscpy_s(szChar,DEFAULT_MAX_STRING,szPath.c_str());
	PathAddBackslash(szChar);

	std::vector<lstring> vPath;	   //存放每一层目录字符串
	lstring strTemp;			   //一个临时变量,存放目录字符串

	for (unsigned int i=0; i< _tcslen(szChar); i++)
	{
		//如果当前字符是'\\'
		//将当前层的字符串添加到数组中
		if (szChar[i] == _T('\\'))
		{
			vPath.push_back(strTemp);
		}
		strTemp += szChar[i];
	}// end of for

	//遍历存放目录的数组,创建每层目录
	for (std::vector<lstring>::const_iterator vIter = vPath.begin(); vIter != vPath.end(); vIter++)
	{
		_tcscpy_s(szChar,DEFAULT_MAX_STRING,vIter->c_str());
		 //如果CreateDirectory执行成功,返回true,否则返回false
		bSuccess = CreateDirectory(szChar, NULL) ;
	}

	return bSuccess;
}


lstring CLibX::File::GetShortPathName( __in const LPCTSTR lpszLongPath )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	::GetShortPathName(lpszLongPath,szChar,DEFAULT_MAX_STRING);
	return szChar;
}


BOOL CLibX::File::WriteResourceFile( __in const LPCTSTR lpFileFullPath,__in const int nResourceID,__in const LPCTSTR lpResourceType )
{
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(nResourceID),lpResourceType);
	if (hRsrc == NULL) return FALSE;
	HGLOBAL hGlobal = LoadResource(NULL,hRsrc);
	if (hGlobal == NULL) return FALSE;
	LPVOID lpVoid = LockResource(hGlobal);
	if (lpVoid == NULL) return FALSE;
	HANDLE hFile = ::CreateFile(lpFileFullPath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;
	DWORD dw = 0;
	BOOL isSuccessful = WriteFile(hFile,lpVoid,SizeofResource(NULL,hRsrc),&dw,NULL);
	if (isSuccessful == FALSE) return FALSE;
	CloseHandle(hFile);
	FreeResource(hRsrc);
	return TRUE;
}


BOOL CLibX::File::SetFileAdministratorOwn( __in const LPCTSTR lpFilePath )
{

	return TRUE;
}


lstring CLibX::File::GetTempFilePath( __in const LPCTSTR lpszPrefixString /*= _TEXT("libx_")*/ )
{

	lchar szChar[DEFAULT_MAX_STRING] = {0};
	lchar szTemp[DEFAULT_MAX_STRING] = {0};
	::GetTempPath(DEFAULT_MAX_STRING,szChar);
	::GetTempFileName(szChar,lpszPrefixString,0,szTemp);
	return szTemp;
}


lstring CLibX::File::GetRelativePath( __in const LPCTSTR lpszFrom,__in const LPCTSTR lpszTo )
{
	lchar szChar[DEFAULT_MAX_STRING];
	PathRelativePathTo(szChar,lpszFrom,FILE_ATTRIBUTE_DIRECTORY,lpszTo,FILE_ATTRIBUTE_NORMAL);
	return szChar;
}

lstring CLibX::File::GetAbsolutePath( __in const LPCTSTR lpszName )
{
	lstring strFileFullPath = _T("") ;

	if (PathIsRelative(lpszName))
	{
		lchar szTemp[MAX_PATH] = {0};
		lchar** lppPart={NULL};

		GetFullPathName(lpszName,MAX_PATH,szTemp,lppPart)   ;
		strFileFullPath = szTemp;

	}
	else
	{
		strFileFullPath = lpszName;
	}

	return strFileFullPath;
}

BOOL CLibX::File::CheckFileExtension( __in const LPCTSTR lpszFile,__in const LPCTSTR lpszExt )
{
	return PathMatchSpec(lpszFile,lpszExt);
}


lstring CLibX::File::RenameExtension( __in const LPCTSTR lpszFile,__in const LPCTSTR lpszExt )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	_tcscpy_s(szChar,DEFAULT_MAX_STRING,lpszFile);
	PathRenameExtension(szChar,lpszExt);
	return szChar;
}


BOOL CLibX::File::CreateShortCut( const LPCTSTR FilePath,const LPCTSTR LinkPath )
{
	BOOL ret = FALSE;  
	IShellLink *psl;  
	CoInitialize(NULL); 
	if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&psl)))  
	{  
		IPersistFile *ppf;  
		psl->SetPath(FilePath);  
		psl->SetDescription(_T("Shortcut"));  
		psl->SetShowCmd(SW_SHOW);  
		if(SUCCEEDED(psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf)))  
		{  
			//WORD wsz[MAX_PATH];  
			//MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,LinkPath,-1,wsz,MAX_PATH);
			// 
			lstring szFilePath = LinkPath;
			if (CLibX::File::IsDirectory(LinkPath))
			{
				lstring szFileName = CLibX::File::GetFileName(FilePath);
				szFileName = CLibX::File::RenameExtension(szFileName.c_str(),_T(".lnk"));
				szFilePath = CLibX::File::AddBackslash(LinkPath) + szFileName;
			}

			wchar_t szLinkPath[DEFAULT_MAX_STRING] = {0};
#ifdef _UNICODE
			wcscpy_s(szLinkPath,DEFAULT_MAX_STRING,szFilePath.c_str());
#else
			wcscpy_s(szLinkPath,DEFAULT_MAX_STRING,CLibX::String::GBToUTF8(szFilePath.c_str()).c_str());
#endif // _UNICODE


			if(SUCCEEDED(ppf->Save(szLinkPath,true)))  
				ret=true;  
			ppf->Release();  
		}  
		psl->Release();  
	} 

	CoUninitialize(); 
	return ret;
}


BOOL CLibX::File::IsDirectory( __in const LPCTSTR lpszFile )
{
	return PathIsDirectory(lpszFile);
}

lstring CLibX::File::AddBackslash( __in const LPCTSTR lpszFile )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	_tcscpy_s(szChar,DEFAULT_MAX_STRING,lpszFile);
	PathAddBackslash(szChar);
	return szChar;
}

lstring CLibX::File::DelBackslash( __in const LPCTSTR lpszFile )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	_tcscpy_s(szChar,DEFAULT_MAX_STRING,lpszFile);
	PathRemoveBackslash(szChar);
	return szChar;
}

lstring CLibX::File::DelFileExtension( __in const LPCTSTR lpszFile )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	_tcscpy_s(szChar,DEFAULT_MAX_STRING,lpszFile);
	PathRemoveExtension(szChar);
	return szChar;
}

lstring CLibX::File::GetLnkTerget( __in const LPCTSTR lpszFile )
{
	HRESULT hres;
	IShellLink* pShellLink;
	WIN32_FIND_DATA wfd;

	lchar szGotPath[MAX_PATH];

	CoInitialize(NULL); 
	hres = CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(VOID**)&pShellLink);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;
		hres = pShellLink->QueryInterface(IID_IPersistFile,(VOID**)&ppf);
		if (SUCCEEDED(hres))
		{

			wchar_t szLinkPath[DEFAULT_MAX_STRING] = {0};
#ifdef _UNICODE
			wcscpy_s(szLinkPath,DEFAULT_MAX_STRING,lpszFile);
#else
			wcscpy_s(szLinkPath,DEFAULT_MAX_STRING,CLibX::String::GBToUTF8(lpszFile).c_str());
#endif // _UNICODE
			
			hres = ppf->Load(szLinkPath,STGM_READ);
			if (SUCCEEDED(hres))
			{
				int nIconIndex = 0 ;
				hres = pShellLink->GetPath(szGotPath,MAX_PATH,(WIN32_FIND_DATA*)&wfd,SLGP_RAWPATH);

				// 读取快捷方式的打开方式
				if (SUCCEEDED(hres))
				{
					return szGotPath;
				}
			}
		}
		ppf->Release();
	}
	CoUninitialize();
	return _T("");
}

#pragma warning(default:4312 4311)
