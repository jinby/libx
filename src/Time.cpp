#include "stdafx.h"
#include "../LibX.h"

// 请保存为Unicode字符
DWORD CLibX::Time::GetSystemStartTime()
{
	return GetTickCount();
}


SYSTEMTIME CLibX::Time::GetSystemTime_()
{
	SYSTEMTIME lpSystemTime;
	::GetSystemTime(&lpSystemTime);
	return lpSystemTime;
}


lstring CLibX::Time::GetSystemTime()
{
	SYSTEMTIME lpSystemTime = GetSystemTime_();

	return CLibX::string_format(_T("%04d年%02d月%02d日 %02d:%02d:%02d") ,
		lpSystemTime.wYear,lpSystemTime.wMonth,lpSystemTime.wDay,lpSystemTime.wHour,lpSystemTime.wMinute,lpSystemTime.wSecond);
}

int CLibX::Time::GetMonth()
{
	return CLibX::Time::GetMonth(GetSystemTime_());
}

int CLibX::Time::GetMonth( __in const SYSTEMTIME systemTime )
{
	return systemTime.wMonth;
}

int CLibX::Time::GetYear()
{
	return GetYear(GetSystemTime_());
}

int CLibX::Time::GetYear( __in const SYSTEMTIME systemTime )
{
	return systemTime.wYear;
}

int CLibX::Time::GetDay( __in const SYSTEMTIME systemTime )
{
	return systemTime.wDay;
}

int CLibX::Time::GetDay()
{
	return GetDay(GetSystemTime_());
}

int CLibX::Time::GetHour()
{
	return GetHour(GetSystemTime_());
}

int CLibX::Time::GetHour( __in const SYSTEMTIME systemTime )
{
	return systemTime.wHour;
}

int CLibX::Time::GetMinute()
{
	return GetMinute(GetSystemTime_());
}

int CLibX::Time::GetMinute( __in const SYSTEMTIME systemTime )
{
	return systemTime.wMinute;
}

int CLibX::Time::GetSecond()
{
	return GetSecond(GetSystemTime_());
}

int CLibX::Time::GetSecond( __in const SYSTEMTIME systemTime )
{
	return systemTime.wMinute;
}

