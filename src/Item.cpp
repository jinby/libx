#include "stdafx.h"
#include "../LibX.h"

#include <assert.h>

// 请保存为Unicode字符


void CLibX::Item::SetItemOffsetPos( __in const HWND hWnd,__in int X /*= 0*/,__in int Y /*= 0*/ )
{
	RECT rect;

	GetWindowRect(hWnd,&rect);
	CLibX::Item::ScreenToClient(GetParent(hWnd) ,&rect);
	OffsetRect(&rect,X,Y);

	CLibX::Item::MoveWindow(hWnd,&rect);
}

void CLibX::Item::ScreenToClient( __in const HWND hWnd,LPRECT lpRect )
{
	_ASSERT_EXPR(::IsWindow(hWnd),L"IsWindow");
	::ScreenToClient(hWnd, (LPPOINT)lpRect);
	::ScreenToClient(hWnd, ((LPPOINT)lpRect)+1);

	if (GetExStyle(hWnd) & WS_EX_LAYOUTRTL)
	{
		LONG temp = lpRect->left; 
		lpRect->left = lpRect->right; 
		lpRect->right = temp; 
	}
}

DWORD CLibX::Item::GetExStyle( __in const HWND hWnd )
{
	return (DWORD)GetWindowLong(hWnd, GWL_EXSTYLE);
}

void CLibX::Item::MoveWindow( __in const HWND hWnd,__in const LPCRECT lpRect, BOOL bRepaint /*= TRUE*/ )
{
	::MoveWindow(hWnd,lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
}

void CLibX::Item::WindowJitter( __in const HWND hWnd,__in int nJitter /*= 3*/,__in int nSpeed /*= 10*/ )
{
	HWND hMoveTagerHwnd = hWnd;
	RECT rect;
	GetWindowRect(hWnd,&rect);

	nJitter = nJitter < 3 ? 3 : nJitter;
	nSpeed = nSpeed < 10 ? 10 : nSpeed;

	for (int i=0 ;i <= nJitter ; i++)
	{
		CLibX::System::SuperSleep(nSpeed);
		::MoveWindow(hMoveTagerHwnd,rect.left - 3,rect.top - 3,rect.right - rect.left,rect.bottom - rect.top,TRUE);
		CLibX::System::SuperSleep(nSpeed);
		::MoveWindow(hMoveTagerHwnd,rect.left - 5,rect.top,rect.right - rect.left,rect.bottom - rect.top,TRUE);
		CLibX::System::SuperSleep(nSpeed);
		::MoveWindow(hMoveTagerHwnd,rect.left - 3,rect.top + 3,rect.right - rect.left,rect.bottom - rect.top,TRUE);
		CLibX::System::SuperSleep(nSpeed);
		::MoveWindow(hMoveTagerHwnd,rect.left ,rect.top + 5,rect.right - rect.left,rect.bottom - rect.top,TRUE);
		CLibX::System::SuperSleep(nSpeed);
		::MoveWindow(hMoveTagerHwnd,rect.left + 3,rect.top + 3,rect.right - rect.left,rect.bottom - rect.top,TRUE);
		CLibX::System::SuperSleep(nSpeed);
		::MoveWindow(hMoveTagerHwnd,rect.left + 5,rect.top,rect.right - rect.left,rect.bottom - rect.top,TRUE);
		CLibX::System::SuperSleep(nSpeed);
		::MoveWindow(hMoveTagerHwnd,rect.left + 3,rect.top - 3,rect.right - rect.left,rect.bottom - rect.top,TRUE);
		CLibX::System::SuperSleep(nSpeed);
		::MoveWindow(hMoveTagerHwnd,rect.left ,rect.top - 5,rect.right - rect.left,rect.bottom - rect.top,TRUE);
	}
	::MoveWindow(hMoveTagerHwnd,rect.left ,rect.top,rect.right - rect.left,rect.bottom - rect.top,TRUE);
}

lstring CLibX::Item::GetWindowText( __in const HWND hWnd )
{
	
	int nLen = ::GetWindowTextLength(hWnd);
	lchar* szChar = new lchar[nLen];
	::GetWindowText(hWnd, szChar, nLen+1);
	lstring rString = szChar;
	delete[] szChar;

	return rString;
}


int CLibX::Item::GetWindowWidth( __in const HWND hWnd )
{
	RECT rec ;
	::GetWindowRect(hWnd,&rec);
	return rec.right - rec.left;
}

int CLibX::Item::GetWindowHeight( __in const HWND hWnd )
{
	RECT rec ;
	::GetWindowRect(hWnd,&rec);
	return rec.bottom - rec.top;
}

lstring CLibX::Item::GetDlgItemText( __in const HWND hWnd,__in UINT nID )
{
	HWND hItemHwnd = ::GetDlgItem(hWnd,nID);
	int nLen = ::GetWindowTextLength(hItemHwnd) + 1;
	lchar* szChar = new lchar[nLen];
	::GetDlgItemText(hWnd,nID,szChar,nLen);
	lstring rString = szChar;

	delete[] szChar;
	return rString;
}

BOOL CLibX::Item::ActivateWindow( __in const HWND hWnd )
{
	DWORD nProcessID = GetWindowThreadProcessId(hWnd,NULL);
	AttachThreadInput(GetCurrentThreadId(),nProcessID,TRUE);
	SetActiveWindow(hWnd);
	AttachThreadInput(GetCurrentThreadId(),nProcessID,FALSE);
	return SetForegroundWindow(hWnd);
}

void CLibX::Item::SetWindowMin( __in const HWND hWnd )
{
	::PostMessage(hWnd,274,61473,0);
}

void CLibX::Item::SetWindowMax( __in const HWND hWnd )
{
	PostMessage(hWnd,274,61488,0);
}

lstring CLibX::Item::GetWindowClass( __in const HWND hWnd )
{
	lchar szChar[DEFAULT_MAX_STRING] = {0}   ;
	GetClassName(hWnd,szChar,DEFAULT_MAX_STRING + 1);
	return szChar;
}

void CLibX::Item::SetWindowTop( __in HWND hwnd,__in BOOL isTop /*= TRUE*/ )
{
#pragma warning(disable:4312)
	::SetWindowPos (hwnd,HWND(isTop == TRUE ? -1 : -2), 0, 0, 0, 0, 3);
#pragma warning(default:4312)
}

BOOL CLibX::Item::SetWindowLayered( __in const HWND hwnd, __in COLORREF crKey, __in BYTE bAlpha, __in DWORD dwFlags,__in BOOL isCroess /*= FALSE*/ )
{
	typedef BOOL (WINAPI *lpfn) (HWND hWnd, COLORREF cr,BYTE bAlpha, DWORD dwFlags);

	//DWORD WS_EX_LAYERED = 0x00080000;
	if (isCroess == TRUE)
	{
		DWORD dwWinFlag = GetWindowLong(hwnd,GWL_EXSTYLE);
		SetWindowLong(hwnd,GWL_EXSTYLE,dwWinFlag|WS_EX_TRANSPARENT|0x00080000); // 
	}else
	{
		SetWindowLong(hwnd,GWL_EXSTYLE,0x00080000);
	}

	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	lpfn g_pSetLayeredWindowAttributes = (lpfn)GetProcAddress(hUser32,"SetLayeredWindowAttributes");
	if (g_pSetLayeredWindowAttributes != NULL)
	{
		return g_pSetLayeredWindowAttributes(hwnd,crKey,bAlpha,dwFlags);
	}

	return FALSE;
}

int CLibX::Item::GetWindowTran( __in const HWND hWnd )
{
	typedef BOOL (WINAPI *lpfn) (__in HWND hwnd, __out_opt COLORREF *pcrKey, __out_opt BYTE *pbAlpha, __out_opt DWORD *pdwFlags);
	BYTE bt=0;
	DWORD pdwFlags = 2;

	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	lpfn g_pGetLayeredWindowAttributes = (lpfn)GetProcAddress(hUser32,"GetLayeredWindowAttributes");
	if (g_pGetLayeredWindowAttributes != NULL)
	{
		return g_pGetLayeredWindowAttributes(hWnd,NULL,&bt,&pdwFlags) == TRUE ? bt : -1;
	}
	return -1;
}

int CLibX::Item::Edit_GetLineCount( __in const HWND hWnd )
{
#pragma warning(disable:4244)
	return SendMessage(hWnd,EM_GETLINECOUNT,NULL,NULL);
#pragma warning(default:4244)
}

void CLibX::Item::Edit_AllSelectText( __in const HWND hWnd )
{
#pragma warning(disable:4267)
	CLibX::Item::Edit_SelectText(hWnd,0,CLibX::Item::GetWindowText(hWnd).length());
#pragma warning(default:4267)
}

void CLibX::Item::Edit_SelectText( __in const HWND hWnd,__in int nStart,__in int nEnd )
{
	::SetFocus(hWnd);
	::SendMessage(hWnd,EM_SETSEL,nStart - 1,nEnd);
}

HRESULT CLibX::Item::Edit_ToIE_Style( __in const HWND hWnd )
{
	const DWORD SHACF_DEFAULT = 0x00000000;
	return	Edit_ExStyle(hWnd,SHACF_DEFAULT);
}

HRESULT CLibX::Item::Edit_ExStyle( __in const HWND hWnd,DWORD dwFlags )
{
	typedef HRESULT (WINAPI *lpfn) (HWND hwndEdit, DWORD dwFlags);
	HMODULE hUser32 = GetModuleHandle(_T("Shlwapi.dll"));
	lpfn g_pSHAutoComplete = (lpfn)GetProcAddress(hUser32,"SHAutoComplete");
	if (g_pSHAutoComplete != NULL)
	{
		return g_pSHAutoComplete(hWnd,dwFlags) ;
	}
	return S_FALSE;
}

void CLibX::Item::Edit_SetMaxLen( __in const HWND hWnd,__in int nMax )
{
	SendMessage(hWnd,EM_LIMITTEXT,nMax,NULL);
}

BOOL CLibX::Item::SetItemFocus( __in const HWND hWnd )
{
	return ::IsWindow(SetFocus(hWnd));
}
