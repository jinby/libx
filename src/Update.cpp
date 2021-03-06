#include "stdafx.h"
#include "../LibX.h"

#include <atlstr.h>   //可以用 cstring了
#import <msxml3.dll>  rename_namespace("XML")

/*
* 更新步骤
* 
* 1.设置程序自身版本号。
* 2.与服务器建立连接。
* 3.下载更新配置文件。
* 4.与版本号进行检查判。
* 5.获取其他信息，如升级时间
* 
* 整体采用XML方式保存，XML内容格式如下
* <?xml version="1.0"?>
* <SoftUpdate UpdateTime="2013/10/5" UpdateTitle="Title" Versoin="1.5" DownURL="http://pan.baidu.com/share/link?shareid=3849410969&amp;uk=2214900742">Context</SoftUpdate>
* 
*/

#pragma warning(disable:4566 4267)
const lstring NODE_NAME = _T("/SoftUpdate");
const lstring ATTR_TIME = _T("UpdateTime");
const lstring ATTR_TITL = _T("UpdateTitle");
const lstring ATTR_VERS = _T("Versoin");
const lstring ATTR_DURL = _T("DownURL");
const lstring ATTR_SOFT = _T("SoftName");

HHOOK hHook = NULL ;

CLibX::Update::Update( void )
{
	m_dbServerVersion = 0;

	CoInitialize(NULL);
}

CLibX::Update::Update(__in const LPCTSTR szUpdateURL,__in double dbNowVersion )
{
	m_szUpdateUrl     = szUpdateURL;
	m_dbSoftVersion   = dbNowVersion;
	m_dbServerVersion = 0;
	CoInitialize(NULL);
}

CLibX::Update::~Update( void )
{
	CoUninitialize(); 
}

LRESULT CALLBACK CLibX::Update::CBTProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	if(nCode < 0) return CallNextHookEx(hHook,nCode,wParam,lParam);  

	switch(nCode)  
	{  
	case HCBT_ACTIVATE:  
		{  
			UnhookWindowsHookEx(hHook);//防止HOOK到其它的窗口  
			HWND MSGhandle = (HWND)wParam;//MessageBox窗口句柄  
			//65535 是文字item ID  
			::SetDlgItemText(MSGhandle,IDYES,_T("下?(&D)"));
			::SetDlgItemText(MSGhandle,IDNO,_T("取消(&C)"));
			::SetDlgItemText(MSGhandle,IDCANCEL,_T("永不(&N)"));
			return 0;  
		}         
	}  
	return CallNextHookEx(hHook,nCode,wParam,lParam);  
}


lstring CLibX::Update::getWebSource( LPCTSTR szUrl )
{
	lstring strWebSource ;

	CoInitialize(NULL);
	try
	{
		XML::IXMLHTTPRequestPtr http = NULL;//(__uuidof(XMLHTTP));  
		http.CreateInstance(_T("Msxml2.XMLHTTP"));  
		http->open(_T("GET"), szUrl, false);  
		http->setRequestHeader("Referer","");  
		http->setRequestHeader("Accept-Language","zh-cn");  
		http->setRequestHeader("Content-Type","application/x-www-form-urlencoded");  
		http->send();

		if((http->readyState)==4)  //4?表示数据已加?完
		{
			strWebSource = _bstr_t(http->responseText.copy());
		}
		http.Release();
	}
	catch (...){
	}
	CoUninitialize();
	return strWebSource;
}

lstring CLibX::Update::postWebSite( __in const LPCTSTR szUrl ,__in const LPCTSTR szContext )
{
	lstring strWebSource ;

	CoInitialize(NULL);
	try
	{
		XML::IXMLHTTPRequestPtr http = NULL;//(__uuidof(XMLHTTP));  
		http.CreateInstance(_T("Msxml2.XMLHTTP"));  
		http->open(_T("POST"), szUrl, false);  
		http->setRequestHeader("Referer","");  
		http->setRequestHeader("Accept-Language","zh-cn"); 
		http->setRequestHeader("Content-Length",_bstr_t(_tcslen(szContext)));
		http->setRequestHeader("Content-Type","application/x-www-form-urlencoded");  
		http->send(szContext);

		if((http->readyState)==4)  //4?表示数据已加?完
		{
			strWebSource = _bstr_t(http->responseText.copy());
		}
		http.Release();
	}
	catch (...){
	}
	CoUninitialize();
	return strWebSource;
}


BOOL CLibX::Update::checkUpdate()
{
	if (m_szUpdateUrl.length() < 5) return FALSE;

	m_szUpdateContext = getWebSource(m_szUpdateUrl.c_str());

	if (m_szUpdateContext.length() < 20) return FALSE;

	XML::IXMLDOMDocumentPtr pDoc2;

	// ?建DOMDocument?象 
	HRESULT hr2  =  pDoc2.CreateInstance(__uuidof(XML::DOMDocument30)); 
	if ( !SUCCEEDED(hr2) ) return  FALSE;

	pDoc2->loadXML(m_szUpdateContext.c_str());

	try
	{
		XML::IXMLDOMNodePtr pNode = pDoc2->selectSingleNode(NODE_NAME.c_str());
		if (pNode != NULL)
		{
			XML::IXMLDOMElementPtr pElement = pNode;
			_variant_t vtUpdateTime  = pElement->getAttribute(ATTR_TIME.c_str());
			_variant_t vtUpdateTitle = pElement->getAttribute(ATTR_TITL.c_str());
			_variant_t vtVersoin     = pElement->getAttribute(ATTR_VERS.c_str());
			_variant_t vtDownURL     = pElement->getAttribute(ATTR_DURL.c_str());
			_variant_t vtSoftName    = pElement->getAttribute(ATTR_SOFT.c_str());

			m_dbServerVersion = _tstof(_bstr_t(vtVersoin.bstrVal));
			m_szUpdateDURL    = _bstr_t(vtDownURL.bstrVal);
			m_szUpdateTitle   = _bstr_t(vtUpdateTitle.bstrVal);
			m_szUpdatetime    = _bstr_t(vtUpdateTime.bstrVal);
			m_szSoftName      = _bstr_t(vtSoftName.bstrVal);
			m_szContext       = pElement->Gettext();
			pElement.Release();
		}
		pNode.Release();
	}
	catch (...)
	{
		pDoc2.Release();
		return false;
	}

	pDoc2.Release();
	return m_dbServerVersion > m_dbSoftVersion;
}

BOOL CLibX::Update::openUpdateUrl()
{
	HINSTANCE hIns = CLibX::System::OpenURL(m_szUpdateDURL.c_str());
	return hIns != NULL;
}

int CLibX::Update::showMessageBox()
{
	lstring szText = CLibX::string_format(_T("更新??：\t%s Ver %.2f \r\n更新内容：%s\r\n更新??：\t%s\r\n下?地址：\t%s"),
		m_szUpdateTitle.c_str(),m_dbServerVersion, m_szContext.c_str(),m_szUpdatetime.c_str(),m_szUpdateDURL.c_str());

	hHook = SetWindowsHookEx(WH_CBT,(HOOKPROC)CLibX::Update::CBTProc,GetModuleHandle(NULL),GetCurrentThreadId());
	return MessageBox(NULL,szText.c_str(),m_szUpdateTitle.c_str(),MB_YESNOCANCEL|MB_ICONINFORMATION);
}


BOOL CLibX::Update::save( double dbVersion, LPCTSTR szContext,LPCTSTR szSoftName, LPCTSTR szTitle,LPCTSTR szDURL,LPCTSTR szTime, LPCTSTR szFilePath )
{
	CoInitialize(NULL);

	HRESULT hr = NULL;
	try
	{
		XML::IXMLDOMDocumentPtr pDoc; 
		XML::IXMLDOMElementPtr  xmlRoot ; 

		// 创建DOMDocument对象
		hr  =  pDoc.CreateInstance(__uuidof(XML::DOMDocument30)); 
		if ( ! SUCCEEDED(hr)) 
		{  
			return  FALSE;
		} 

		XML::IXMLDOMProcessingInstructionPtr pDOMPI=pDoc->createProcessingInstruction(_T("xml"), _T("version=\"1.0\"")); // ?建XML声明
		pDoc->appendChild(pDOMPI); // 添加XML声明

		xmlRoot = pDoc -> createElement( _T("SoftUpdate") );
		xmlRoot->setAttribute(ATTR_TIME.c_str() ,szTime );
		xmlRoot->setAttribute(ATTR_TITL.c_str() ,szTitle);
		xmlRoot->setAttribute(ATTR_VERS.c_str() ,(double)dbVersion);
		xmlRoot->setAttribute(ATTR_DURL.c_str() ,szDURL);
		xmlRoot->setAttribute(ATTR_SOFT.c_str() ,szSoftName);
		xmlRoot->Puttext(szContext);

		pDoc -> appendChild(xmlRoot);
		hr = pDoc -> save( szFilePath );

		pDOMPI.Release();
		xmlRoot.Release();
		pDoc.Release();
	}catch ( ... ){
	}

	CoUninitialize(); 
	return SUCCEEDED(hr);
}


double CLibX::Update::versionToDouble( __in const LPCTSTR szVersion )
{
	lstring strTemp;
	lstring strVersion = szVersion;

	for (unsigned int i=0; i< strVersion.length(); i++)
	{
		if (strVersion[i] >='0' && strVersion[i] <= '9')
		{
			strTemp += strVersion[i];
		}
	}// end of for

	return _tstof(strTemp.c_str());
}
#pragma warning(default:4566 4267)
