//***********************************************************************/
// GetNtProcessInfo Demo Win32 App - Displays up to 50 processes via
//   ListView Common Control
//
// Copyright © 2007 Steven Moore (OrionScorpion).  All Rights Reserved.
//
//***********************************************************************/

#include "stdafx.h"
#include "GetNtProcessInfo.h"

// Global Variables:
HINSTANCE hInst			= NULL;								// current instance
TCHAR szTitle[32]		= { _T(" GetNtProcessInfo Demo") };	// The title bar text
TCHAR szWindowClass[32] = { _T("WCGETNTPROCESSINFODEMO") };	// the main window class name

// Used in WndProc Callback Function
static CREATESTRUCT *cs			 = NULL;
static HWND hWndLv				 = NULL;
static smPROCESSINFO lpi[MAX_PI] = {0};

// Forward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

// The Main entry function
int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE, //hPrevInstance,
                       LPTSTR,	 //lpCmdLine,
                       int       nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

 	MSG msg				= {0};
	WNDCLASSEX wcex		= {0};
	HACCEL hAccelTable	= {0};
 
	wcex.cbSize			= sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hIconSm		= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_GETNTPROCESSINFO;
	wcex.lpszClassName	= szWindowClass;

	RegisterClassEx(&wcex);


	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_GETNTPROCESSINFO);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	INITCOMMONCONTROLSEX iccx = {0};
    iccx.dwICC	= ICC_LISTVIEW_CLASSES;
    iccx.dwSize	= sizeof(INITCOMMONCONTROLSEX);
    
	InitCommonControlsEx(&iccx);

	HWND hWnd = CreateWindowEx(NULL, szWindowClass,
							   szTitle, WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, 0,	// location
                               CW_USEDEFAULT, 0,	// size
                               NULL, NULL,			// parent, menu
                               hInstance, NULL);
	if (!hWnd)
	{
        return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId				= 0;	// WM_COMMAND - Menu Commands
	int wmEvent				= 0;	// WM_COMMAND

	PAINTSTRUCT ps			= {0};	// WM_PAINT
	HDC hDC					= NULL;	// WM_PAINT
	RECT rect				= {0};	// WM_SIZE

	DWORD dwPIDCount		= 0;	// WM_CREATE
	LV_DISPINFO *plvdi		= 0;	// WM_CREATE

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE:
        cs = (CREATESTRUCT *)lParam;
		// create listview window
	    hWndLv = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, _T(""),
								WS_VISIBLE | WS_CHILD | LVS_REPORT | 
								LVS_SINGLESEL | LVS_SHOWSELALWAYS | 
								LVS_NOSORTHEADER, 0, 0, 0, 0, hWnd,
								(HMENU)LISTVIEW_CWND_ID, cs->hInstance, NULL);

		// set styles for listview
        ListView_SetExtendedListViewStyle(hWndLv, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		// Add Columns, Set Colors, etc
		InitDemoListView(hWndLv);

		// Add process entries to listview
		dwPIDCount = EnumProcesses2Array(lpi);

		// Let ListView know we have text in our array via WM_NOTIFY
		// ListView will use LVN_GETDISPINFO to get text from array
		SetTextCallbackFlag(hWndLv, dwPIDCount);

		break;
	case WM_SIZE:
		// maintain small border around ListView
		GetClientRect(hWnd, &rect);
		MoveWindow(hWndLv,
				   2, 2,			// location x,y
				   rect.right - 4,	// width
				   rect.bottom - 4,	// height
				   TRUE);			// repaint

	    break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_NOTIFY:
        switch(((NMHDR *)lParam)->code)
		{
		case LVN_GETDISPINFO:
            plvdi = (NMLVDISPINFO *)lParam;
            if((((NMHDR *)lParam)->hwndFrom == hWndLv))
			{
                switch(plvdi->item.iSubItem)
				{
                case 0:
                    StringCchPrintf(plvdi->item.pszText,
									plvdi->item.cchTextMax,
									_T("%u"), lpi[plvdi->item.iItem].dwPID);
					break;
				case 1:
                    StringCchPrintf(plvdi->item.pszText,
									plvdi->item.cchTextMax,
									_T("%u"), lpi[plvdi->item.iItem].dwParentPID);
					break;
				case 2:
                    StringCchPrintf(plvdi->item.pszText,
									plvdi->item.cchTextMax,
									_T("%u"), lpi[plvdi->item.iItem].dwSessionID);
					break;
				case 3:
                    StringCchPrintf(plvdi->item.pszText,
									plvdi->item.cchTextMax,
									_T("%u"), lpi[plvdi->item.iItem].dwBasePriority);
					break;
				case 4:
                    StringCchPrintf(plvdi->item.pszText,
									plvdi->item.cchTextMax,
									_T("%u"), lpi[plvdi->item.iItem].dwExitStatus);
					break;
                case 5:
					StringCchCopy(plvdi->item.pszText,
								  plvdi->item.cchTextMax,
								  (lpi[plvdi->item.iItem].cBeingDebugged) ? _T("Y") : _T("N"));
					break;
                case 6:
                    StringCchPrintf(plvdi->item.pszText,
									plvdi->item.cchTextMax,
									_T("%0.8x"), lpi[plvdi->item.iItem].dwAffinityMask);
					break;
                case 7:
                    StringCchPrintf(plvdi->item.pszText,
									plvdi->item.cchTextMax,
									_T("%0.8x"), lpi[plvdi->item.iItem].dwPEBBaseAddress);
					break;
                case 8:
                    StringCchCopy(plvdi->item.pszText,
								  plvdi->item.cchTextMax,
								  lpi[plvdi->item.iItem].szCmdLine);
					break;
                case 9:
                    StringCchCopy(plvdi->item.pszText,
								  plvdi->item.cchTextMax,
								  lpi[plvdi->item.iItem].szImgPath);
					break;
				}
			}
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// Listview
void InitDemoListView(HWND hWndLv)
{
	LV_COLUMN lvc = {0};
	TCHAR szLvcColumn[LVC_COLCOUNT][10] = { _T("PID"),
											_T("PPID"),
											_T("SID"),
											_T("Pri"),
 											_T("EC"),
											_T("Debug"),
 											_T("AMask"),
 											_T("PEBAddr"),
											_T("CmdLine"),
											_T("ImgPath") };
	int nLvcWidth[LVC_COLCOUNT] = { 38, 38, 35, 35, 40, 47, 68, 68, 100, 100 };
	int nLvcFormat[LVC_COLCOUNT] = { LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT,
									 LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_CENTER,
									 LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_LEFT,
									 LVCFMT_LEFT };

	// Add columns
	// First column format is always left justified
	ZeroMemory(&lvc, sizeof(lvc));

	lvc.mask = LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT;

	for(int c = 0; c < LVC_COLCOUNT; c++)
	{
		lvc.cx		 = nLvcWidth[c];
		lvc.fmt		 = nLvcFormat[c];
		lvc.iSubItem = c;
		lvc.pszText	 = szLvcColumn[c];
		ListView_InsertColumn(hWndLv, c, &lvc);
	}

	// Set Colors
	ListView_SetBkColor(hWndLv, RGB(255, 255, 255));
	ListView_SetTextColor(hWndLv, RGB(0, 0, 0));
	ListView_SetTextBkColor(hWndLv, CLR_NONE);	// Transparent
}

// Get current running processes
DWORD EnumProcesses2Array(smPROCESSINFO lpi[MAX_PI])
{
	DWORD dwPIDs[MAX_PI] = {0};
	DWORD dwArraySize	 = MAX_PI * sizeof(DWORD);
	DWORD dwSizeNeeded	 = 0;
	DWORD dwPIDCount	 = 0;

	//== only to have better chance to read processes =====
	if(!sm_EnableTokenPrivilege(SE_DEBUG_NAME)) {
	//	return 0;
	}

	// Get a list of Process IDs of current running processes
	if(EnumProcesses((DWORD*)&dwPIDs, dwArraySize, &dwSizeNeeded))
	{
		HMODULE hNtDll = sm_LoadNTDLLFunctions();

		if(hNtDll)
		{
			// Get detail info on each process
            dwPIDCount = dwSizeNeeded / sizeof(DWORD);
			for(DWORD p = 0; p < MAX_PI && p < dwPIDCount; p++)
			{
				if(sm_GetNtProcessInfo(dwPIDs[p], &lpi[p]))
				{
					// Do something else upon success
				}
			}
			sm_FreeNTDLLFunctions(hNtDll);
		}
	}

	// Return either PID count or MAX_PI whichever is smaller
	return (DWORD)(dwPIDCount > MAX_PI) ? MAX_PI : dwPIDCount;
}

// When this flag is set a WM_NOTIFY->LVN_GETDISPINFO message is sent
// and the text is updated.
void SetTextCallbackFlag(HWND hWndLv, const DWORD dwMaxItems)
{
	LV_ITEM lvi = {0};

	ZeroMemory(&lvi, sizeof(LV_ITEM));

    lvi.mask	= LVIF_TEXT;
    lvi.pszText	= LPSTR_TEXTCALLBACK;

    for(DWORD i = 0; i < dwMaxItems; i++)
	{
        ListView_InsertItem(hWndLv, &lvi);
	}
}

#include "NtProcessInfo.cpp"
