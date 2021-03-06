#pragma once
#include "stdafx.h"
#include "../LibX.h"

#pragma message("-----------------------------------------------------")
#pragma message("                    感谢使用LibX")
#pragma message("-----------------------------------------------------")

#pragma warning (disable:4996)


// 请保存为Unicode字符
lstring CLibX::string_format( const LPCTSTR fmt, ... )
{
	va_list ap;
	va_start(ap, fmt);

	const size_t SIZE = 512;
	lchar buffer[SIZE] = { 0 };

	try
	{
		_vsntprintf(buffer, SIZE, fmt, ap);
	
		va_end(ap);
	}
	catch (...){
	}
	return lstring(buffer);
}

#pragma warning (default:4996)


