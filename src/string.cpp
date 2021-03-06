#pragma once
#include "stdafx.h"
#include "../LibX.h"

#include <string>
#include <atlutil.h>


// 请保存为Unicode字符

int CLibX::String::string_replace( __inout lstring &strBase,__in lstring strSrc,__in lstring strDes)
{
	int nReplace = 0;
	lstring::size_type pos = 0;  
	lstring::size_type srcLen = strSrc.size();  
	lstring::size_type desLen = strDes.size();  
	pos=strBase.find(strSrc, pos);   
	while ((pos != lstring::npos))  
	{  
		strBase.replace(pos, srcLen, strDes);  
		pos=strBase.find(strSrc, (pos+desLen));
		nReplace ++ ;
	} 

	return nReplace;
}

lstring CLibX::String::string_trim( __in const lstring str )
{
	lstring dest = str;

	lstring::iterator i;
	for (i = dest.begin(); i != dest.end(); i++) {
		if (!isspace(*i)) {
			dest.erase(dest.begin(), i);
			break;
		}
	}

	if (i == dest.end()) {
		return dest;
	}

	for (i = dest.end() - 1; i != dest.begin(); i--) {
		if (!isspace(*i)) {
			dest.erase(i + 1, dest.end());
			break;
		}
	}

	return dest;
}

lstring CLibX::String::string_ltrim( __in const lstring string_ )
{
	lstring str = string_;
	if (str.find_first_not_of(_T(" \n\r\t"))!=lstring::npos){
		str = str.substr(str.find_first_not_of(_T(" \n\r\t")));
	}
	return  str;
}

lstring CLibX::String::string_rtrim( __in const lstring string_ )
{
	lstring str = string_;
	if (str.find_first_not_of(_T(" \n\r\t"))!=lstring::npos){
		str = str.substr(0,str.find_last_not_of(_T(" \n\r\t"))+1); 
	}
	return str;
}



std::vector<lstring> CLibX::String::string_split( __in const lstring src,__in const lstring delim )
{
	std::vector<lstring> retVector;
	size_t last = 0;
	size_t index = src.find(delim,last);
	while (index != lstring::npos)
	{
		lstring sub = src.substr(last,index-last);
		retVector.push_back(sub);
		last = index+1;
		index = src.find(delim,last);
	}
	if (last != (src.length()))
	{
		retVector.push_back(src.substr(last,index-last));
	}

	return retVector;
}

#pragma warning(disable:4018 4267)
lstring CLibX::String::string_left( __in const lstring str,__in int nLen )
{
	lstring strTemp ;
	if (nLen > str.length()) nLen = str.length();
	for (int i=0; i< nLen; i++)
	{
		strTemp += str[i];
	}// end of for
	return strTemp;
}

lstring CLibX::String::string_right( __in const lstring str,__in int nLen )
{
	lstring strTemp;  
	if (nLen > str.length()) nLen = str.length();

	for (int i=0; i< str.length() ; i++)
	{
		if (str.length() - i <= nLen)
		{
			strTemp += str[i];
		}
	}// end of for

	return strTemp;
}

#pragma warning(default:4018 4267)

lchar * CLibX::String::char_replace( __inout lchar* source,__in lchar* sub,__in lchar* rep)
{
#pragma warning(disable:4244)
	lchar* result;
	lchar *pc1,*pc2,*pc3;
	size_t isub = _tcslen(sub);
	size_t irep = _tcslen(rep);
	size_t isource = _tcslen(source);

	if(NULL == *sub)
	{
		return _tcsdup(source);
	}
	result=(lchar *)malloc(((irep>isub)?(float)_tcslen(source)/isub*irep+1:isource)*sizeof(lchar));
	pc1=result;
	while(*source!=NULL)
	{
		pc2=source;
		pc3=sub;
		while(*pc2==*pc3 && *pc3!=NULL && *pc2 != NULL)
			pc2++,pc3++;
		if(NULL==*pc3)
		{
			pc3=rep;
			while(*pc3!=NULL)
				*pc1++=*pc3++;
			pc2--;
			source=pc2;
		}
		else
			*pc1++ = *source;
		source++;
	}
	*pc1=NULL;
	return result;
#pragma warning(default:4244)
}

std::string CLibX::String::UTF8ToGB( __in const wchar_t* utf8 )
{
	int nStrLen = WideCharToMultiByte(CP_ACP, 0, utf8, -1, NULL, 0, NULL, NULL);  
	char* str = new char[nStrLen+1];  
	memset(str, 0, nStrLen+1);  
	WideCharToMultiByte(CP_ACP, 0, utf8, -1, str, nStrLen, NULL, NULL);  
  
	std::string str_ret = str;

	if (str != NULL)
	{
		delete[] str;
	}

	return str_ret;
}

std::wstring CLibX::String::GBToUTF8(__in const char* gb2312 )
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);  
	wchar_t* wstr = new wchar_t[len+1];  
	memset(wstr, 0, len+1);  
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len); 
	std::wstring w_str = wstr;
	delete[] wstr;

	return w_str;
}

lstring CLibX::String::load_string( __in UINT nID,__in HINSTANCE hInstance /*= NULL*/ )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	::LoadString(hInstance,nID,szChar,DEFAULT_MAX_STRING);
	return szChar;
}

int CLibX::String::GetRand( __in int nMin,__in int nMax )
{
	return  rand() % (nMax - nMin) + nMin ;
}

lstring CLibX::String::GetRandNo( __in int nLen )
{
	lstring strReturn;
	for (int i = 0 ; i < nLen ; i++)
	{
		strReturn.append(CLibX::string_format(_T("%d"),CLibX::String::GetRand(0,9)));
	}
	return strReturn;
}

lstring CLibX::String::GetRandNo( __in int nMin ,__in int nMax,__in UINT nLen )
{
	lstring strReturn;
	for (unsigned int i = 0 ; i < nLen ; i++)
	{
		strReturn.append(CLibX::string_format(_T("%d"),rand() % (nMax - nMin) + nMin));
	}
	return strReturn;
}

lstring CLibX::String::GetRandChinese( __in int nLen )
{
	lstring strChinese ;

	for (int i = 0 ; i < nLen ; i++)
	{
		char Chinese[] = "??\0";
		Chinese[0] = -CLibX::String::GetRand(38,80);
		Chinese[1] = -CLibX::String::GetRand(38,80);

#ifdef _UNICODE
		strChinese += CLibX::String::GBToUTF8(Chinese);
#else
		strChinese += Chinese;
#endif // _UNICODE
	}

	return strChinese;
}
#pragma warning(disable:4566 4267)

lstring CLibX::String::GetRandXing()
{
	lstring strXing = _T("李,王,?,?,?,?,?,黄,周,?,徐,?,胡,朱,高,林,何,郭,?,?,梁,宋,?,?,?,唐,?,于,董,?,程,曹,袁,?,?,傅,沈,曾,彭,?,?,?,蒋,蔡,?,丁,魏,薛,叶,?,余,潘,杜,戴,夏,?,汪,田,任,姜,范,方,石,姚,?,廖,?,熊,金,?,?,孔,白,崔,康,毛,邱,秦,江,史,?,侯,邵,孟,?,万,段,章,?,?,尹,黎,易,常,武,?,?,?,?,文");
	std::vector<lstring> arrString = CLibX::String::string_split(strXing.c_str(),_T(",")) ;
	int nRandPos = rand() % arrString.size();
	return arrString[nRandPos];
}

lstring CLibX::String::GetRandMing( int nLen /*= -1*/,bool nIsBoy /*= true */ )
{
	lstring strReturn;
	lstring strBoy  = _T("?,?,勇,毅,俊,峰,?,?,平,保,?,文,?,力,明,永,健,世,广,志,?,?,良,海,山,仁,波,?,?,福,生,?,元,全,国,?,学,祥,才,?,武,新,利,清,?,彬,富,?,信,子,杰,涛,昌,成,康,星,光,天,?,安,岩,中,茂,?,林,有,?,和,彪,博,?,先,敬,震,振,壮,会,思,群,豪,心,邦,承,?,?,功,松,善,厚,?,磊,民,友,裕,河,哲,江,超,浩,亮,政,?,亨,奇,固,之,?,翰,朗,伯,宏,言,若,?,朋,斌,梁,?,?,?,克,?,翔,旭,?,?,晨,辰,士,以,建,家,致,?,炎,德,行,?,泰,盛,雄,?,?");
	lstring strGril = _T("芝,玉,萍,?,娥,玲,芬,芳,燕,彩,春,菊,?,?,?,梅,琳,素,云,?,真,?,雪,?,?,妹,霞,香,月,?,媛,?,瑞,凡,佳,嘉,?,勤,珍,?,莉,桂,?,叶,璧,?,?,琦,晶,妍,茜,秋,珊,莎,?,黛,青,倩,?,?,婉,?,瑾,?,露,瑶,怡,?,雁,?,?,?,荷,丹,蓉,眉,君,琴,蕊,薇,菁,梦,?,苑,?,柔,竹,?,凝,?,?,霄,?,芸,菲,寒,欣,?,伊,?,宜,可,?,舒,影,?,枝,思,?,秀,?,育,馥,琦,晶,妍,茜,秋,珊,莎,?,黛,青,倩,?,?,?,?,苑,?,馨,?,?,韵,融,?,?,咏,卿,?,?,?,毓,悦,昭,冰,爽,?,茗,羽,希");

	std::vector<lstring> arrString = CLibX::String::string_split(nIsBoy == true ? strBoy.c_str() : strGril.c_str(),_T(",")) ;

	if (nLen <= 0)  nLen = rand() % 2 + 1;
	for (int i = 0 ;i < nLen ;i++)
	{
		strReturn += arrString[rand() % arrString.size()];
	}

	return strReturn;
}

lchar* CLibX::String::char_rev( lchar *s )
{
	for(lchar *end = s + _tcslen(s) - 1; end > s ; --end, ++s)
	{
		*s ^= *end;
		*end ^= *s;
		*s ^= *end;
	}
	return s;
}

lchar* CLibX::String::char_left( const lchar *src,size_t n )
{
	lstring strSrc = src;

	size_t nLen = _tcslen(src);

	if(n > nLen) n=nLen;
	lchar* strTemp = new lchar[n] ;
	memset(strTemp,'\0',n * sizeof(lchar));

	for (size_t i=0; i< n; i++)
	{
		strTemp[i] = strSrc[i];
	}// end of for
	strTemp[n+1] = _T('\0');

	return strTemp;
}

lstring CLibX::String::FormatStringToEscape( __in const LPCTSTR szUrl )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	DWORD dwLength = 0;
	AtlEscapeUrl( szUrl , szChar , &dwLength , DEFAULT_MAX_STRING ) ;
	return szChar;
}

lstring CLibX::String::FormatEscapeToString( __in const LPCTSTR szUrl )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0};
	DWORD dwLength = 0;
	AtlUnescapeUrl( szUrl , szChar , &dwLength , DEFAULT_MAX_STRING ) ;
	return szChar;
}

lstring CLibX::String::FormatStringToHex( __in const LPCTSTR szText )
{
	int nTextLen = _tcslen(szText) * sizeof(lchar);
	int nMaxLen = nTextLen * 2 + sizeof(lchar);

	char* szChar =  new char[nMaxLen];
	memset(szChar,'\0',nMaxLen * sizeof(char));
	AtlHexEncode((const BYTE *)szText,nTextLen,szChar,&nMaxLen);
	szChar[nMaxLen] = _T('\0');

#ifdef _UNICODE
	lstring szTemp = CLibX::String::GBToUTF8(szChar) ;
#else
	lstring szTemp = szChar ;
#endif // _UNICODE
	delete[] szChar;
	return szTemp;
}

lstring CLibX::String::FormatHexToString( __in const LPCTSTR szText )
{
	int nTextLen = _tcslen(szText);
	int nMaxLen = nTextLen + sizeof(lchar);
	
#ifdef _UNICODE
	std::string szTextA = CLibX::String::UTF8ToGB(szText);
#else
	std::string szTextA = szText;
#endif // _UNICODE

	lchar* szChar =  new lchar[nMaxLen];
	memset(szChar,'\0',nMaxLen * sizeof(lchar));

	AtlHexDecode(szTextA.c_str(),nTextLen,(BYTE*)szChar,&nMaxLen);
	szChar[nMaxLen] = _T('\0');

	lstring szTemp = szChar ;
	delete[] szChar;
	return szTemp;
}

lstring CLibX::String::Base64Encode( __in const LPCTSTR lpText )
{
	lstring strTarget = lpText; 
	int nDesLen = Base64EncodeGetRequiredLength(strTarget.length());

	char* szText = new char[nDesLen];
	memset(szText,'\0',nDesLen * sizeof(lchar));

	::Base64Encode((const BYTE*)(lchar*)strTarget.c_str(),strTarget.length(),szText,&nDesLen);
	szText[nDesLen + 1] = '\0';
#ifdef _UNICODE
	lstring str64 = CLibX::String::GBToUTF8(szText);
#else
	lstring str64 = szText;
#endif // _UNICODE

	delete[] szText;

	return str64;
}

lstring CLibX::String::Base64Decode( __in const LPCTSTR lpText )
{
#ifdef _UNICODE
	std::string strBase64A = CLibX::String::UTF8ToGB(lpText);
#else
	std::string strBase64A = lpText;
#endif // _UNICODE

	int nDecLen = Base64DecodeGetRequiredLength(strBase64A.length());

	char* szText = new char[nDecLen];
	memset(szText,'\0',nDecLen);

	::Base64Decode(strBase64A.c_str(),strBase64A.length(),(BYTE*)szText,&nDecLen);

#ifdef _UNICODE
	lstring strDecode = CLibX::String::GBToUTF8(szText);
#else
	lstring strDecode = szText;
#endif // _UNICODE
	delete[] szText;

	return strDecode;
}

#pragma warning(disable:4244 4996)

lstring CLibX::String::FormatUCodeToString ( __in const LPCTSTR szText )
{
#ifdef _UNICODE
	std::wstring strText = szText;
#else
	std::wstring strText = CLibX::String::GBToUTF8(szText);
#endif // _UNICODE

	std::wstring strRetText;
	std::wstring strTemp;


	for (lstring::size_type i=0; i< strText.length() ; i++)
	{
		if (strText[i] == _T('\\') && strText[i+1] == _T('u'))
		{
			strTemp = strText[i+2];
			strTemp += strText[i+3];
			strTemp += strText[i+4];
			strTemp += strText[i+5];

			strRetText += wcstol(strTemp.c_str(),NULL,16);
			i += 5;
		}else
		{
			strRetText += strText[i];
		}
	}// end of for


#ifdef _UNICODE
	return strRetText;
#else
	return CLibX::String::UTF8ToGB(strRetText.c_str());
#endif // _UNICODE
}

lstring CLibX::String::FormatStringToUCode( __in const LPCTSTR szText )
{
#ifdef _UNICODE
	std::wstring strTest = szText;
#else
	std::wstring strTest = CLibX::String::GBToUTF8(szText);
#endif // _UNICODE

	std::wstring strRetCode ;
	for (std::wstring::size_type i=0; i< strTest.length() ; i++)
	{
		wchar_t a = strTest[i];
		if ( a > 0x80)
		{
			wchar_t szChar[10] = {0};
			std::wstring str = L"\\u" ;
			str.append(_itow(a,szChar,16));
			strRetCode.append(str)	;
		}else
		{
			strRetCode += a;
		}
	}// end of for
	
#ifdef _UNICODE
	return strRetCode;
#else
	return CLibX::String::UTF8ToGB(strRetCode.c_str());
#endif // _UNICODE

}
#pragma warning(default:4244 4996)


#pragma warning(default:4566 4267)
