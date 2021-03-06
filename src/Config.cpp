#include "stdafx.h"
#include "../LibX.h"

#include <atlbase.h>
// 请保存为Unicode字符

CRegKey* key = NULL;

HKEY CLibX::Config::OpenRegKey( __in const HKEY hKey, __in_opt LPCTSTR lpSubKey , __in REGSAM samDesired /*= KEY_ALL_ACCESS */ )
{
	key = new CRegKey();
	key->Open(hKey,lpSubKey,samDesired );
	return key->m_hKey;
}

LONG CLibX::Config::CloseRegKey( __in const HKEY nKey )
{
	return RegCloseKey(nKey);
}

DWORD CLibX::Config::ReadRegDWORD( __in const HKEY hKey, __in const LPCTSTR lpszValueName )
{
	key = new CRegKey(hKey);
	DWORD qwValue;
	key->QueryDWORDValue(lpszValueName,qwValue);
	return qwValue;
}

lstring CLibX::Config::ReadRegString( __in const HKEY hKey, __in const LPCTSTR lpszValueName, ULONG nSize /*= DEFAULT_MAX_STRING */ )
{
	lchar* strTemp = new lchar[nSize];
	ULONG dwRead = nSize;
	key = new CRegKey(hKey);
	key->QueryStringValue(lpszValueName,strTemp,&dwRead);
	lstring str = strTemp;
	delete[] strTemp;
	return str; 
}

LONG CLibX::Config::ReadRegBinaryValue( __in const HKEY hKey,__in const LPCTSTR pszValueName,__out void* pValue,__in ULONG lBytes /*= DEFAULT_MAX_STRING*/ )
{
	key = new CRegKey(hKey);
	ULONG dwRead = key->QueryBinaryValue(pszValueName,pValue,&dwRead);
	return dwRead;
}

LONG CLibX::Config::WriteRegStringValue( __in const HKEY hKey,__in const LPCTSTR pszValueName,__in const LPCTSTR lpString )
{
	key = new CRegKey(hKey);
	ULONG lWrite = key->SetStringValue(pszValueName,lpString);
	return lWrite;
}

LONG CLibX::Config::WriteRegDWORDValue( __in const HKEY hKey,__in const LPCTSTR pszValueName,__in const DWORD dwValue )
{
	key = new CRegKey(hKey);
	ULONG lWrite = key->SetDWORDValue(pszValueName,dwValue);
	return lWrite;
}

LONG CLibX::Config::WriteRegBinaryValue( __in const HKEY hKey,__in const LPCTSTR pszValueName,__in const LPVOID lpVoid,__in ULONG nBytes )
{
	key = new CRegKey(hKey);
	ULONG lWrite = key->SetBinaryValue(pszValueName,lpVoid,nBytes);
	return lWrite;
}

LONG CLibX::Config::CreateRegKey( __in const HKEY hKeyParent, __in const LPCTSTR lpSubKey )
{
	key = new CRegKey(hKeyParent);
	ULONG lWrite = key->Create(hKeyParent,lpSubKey);
	return lWrite;
}

LONG CLibX::Config::DeleteRegSubKey( __in const HKEY hKeyParent,__in const LPCTSTR lpszSubKey )
{
	key = new CRegKey(hKeyParent);
	ULONG lWrite = key->DeleteSubKey(lpszSubKey);
	return lWrite;
}

LONG CLibX::Config::DeleteRegValue( __in const HKEY hKeyParent,__in const LPCTSTR lpszValue )
{
	key = new CRegKey(hKeyParent);
	ULONG lWrite = key->DeleteValue(lpszValue);
	return lWrite;
}

LONG CLibX::Config::DeleteRegAllSubKey( __in const HKEY hKeyParent,__in const LPCTSTR lpszKey )
{
	key = new CRegKey(hKeyParent);
	ULONG lWrite = key->RecurseDeleteKey(lpszKey);
	return lWrite;
}

BOOL CLibX::Config::WriteIniString( __in const LPCTSTR lpAppName, __in const LPCTSTR lpKeyName, __in const LPCTSTR lpString, __in const LPCTSTR lpFileName )
{
	return WritePrivateProfileString(lpAppName,lpKeyName,lpString,lpFileName);
}

lstring CLibX::Config::GetIniString( __in const LPCTSTR lpAppName, __in const LPCTSTR lpKeyName, __in const LPCTSTR lpDefault, __in const LPCTSTR lpFileName,__in DWORD dwReadSize /*= DEFAULT_MAX_STRING*/ )
{
	lchar* szChar = new lchar[dwReadSize];
	GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,szChar,dwReadSize,lpFileName);
	lstring szText = szChar;
	delete[] szChar;
	return szText;
}

BOOL CLibX::Config::WriteIniInt( __in const LPCTSTR lpAppName, __in const LPCTSTR lpKeyName, __in int nValue, __in const LPCTSTR lpFileName )
{
	lstring strTemp = CLibX::string_format(_T("%d"),nValue);;
	return CLibX::Config::WriteIniString(lpAppName,lpKeyName,strTemp.c_str(),lpFileName);
}

int CLibX::Config::GetIniInt( __in const LPCTSTR lpAppName, __in const LPCTSTR lpKeyName, __in int nDefault, __in const LPCTSTR lpFileName )
{
	return GetPrivateProfileInt(lpAppName,lpKeyName,nDefault,lpFileName);
}

BOOL CLibX::Config::WriteIniStruct( __in const LPCTSTR lpAppName, __in const LPCTSTR lpKeyName, __in LPVOID lpStruct, __in UINT uSizeStruct, __in const LPCTSTR lpFileName )
{
	return WritePrivateProfileStruct(lpAppName,lpKeyName,lpStruct,uSizeStruct,lpFileName);
}

BOOL CLibX::Config::GetIniStruct( __in const LPCTSTR lpAppName,__in LPCTSTR const lpKeyName,__out LPVOID lpStruct,__in UINT uSizeStruct,__in const LPCTSTR lpFileName )
{
	return GetPrivateProfileStruct(lpAppName,lpKeyName,lpStruct,uSizeStruct,lpFileName);
}

BOOL CLibX::Config::WirteIniDouble( __in const LPCTSTR lpAppName,__in LPCTSTR const lpKeyName,__in double dbValue,__in const LPCTSTR lpFileName )
{
	lstring strTemp = CLibX::string_format(_T("%f"),dbValue);;
	return CLibX::Config::WriteIniString(lpAppName,lpKeyName,strTemp.c_str(),lpFileName);
}

double CLibX::Config::GetIniDouble( __in const LPCTSTR lpAppName,__in LPCTSTR const lpKeyName,__in double dbDefault,__in const LPCTSTR lpFileName )
{
	lstring strDefault = CLibX::string_format(_T("%f"),dbDefault);
	lstring strTemp = CLibX::Config::GetIniString(lpAppName,lpKeyName,strDefault.c_str(),lpFileName);
	return _tstof(strTemp.c_str());
}

BOOL CLibX::Config::WirteIniBool( __in const LPCTSTR lpAppName,__in LPCTSTR const lpKeyName,__in BOOL bValue,__in const LPCTSTR lpFileName, __in const LPCTSTR szTrue /*= _T("是|否")*/ )
{
	std::vector<lstring> vecTr = CLibX::String::string_split(szTrue,_T("|"));
	lstring szTemp = bValue ? vecTr[0] : vecTr[1];
	return CLibX::Config::WriteIniString(lpAppName,lpKeyName,szTemp.c_str(),lpFileName);
}

BOOL CLibX::Config::GetIniBool( __in const LPCTSTR lpAppName,__in LPCTSTR const lpKeyName,__in BOOL bDefault,__in const LPCTSTR lpFileName, __in const LPCTSTR szTrue /*= _T("是|否")*/ )
{
	std::vector<lstring> vecTr = CLibX::String::string_split(szTrue,_T("|"));
	lstring szDefault = bDefault ? vecTr[0] : vecTr[1];
	lstring strTemp = CLibX::Config::GetIniString(lpAppName,lpKeyName,szDefault.c_str(),lpFileName);
	
	return strTemp == vecTr[0];

}
