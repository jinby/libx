//***********************************************************************/
// Demo Header to Display process via ListView in Win32
//
// Copyright © 2007 Steven Moore (OrionScorpion).  All Rights Reserved.
//
//***********************************************************************/
#pragma once

#include "resource.h"

#define MAX_PI				50	// Max processes to get for demo
#define LVC_COLCOUNT		10	// Column Count for ListView
#define	LISTVIEW_CWND_ID	1	// Used as HMENU in CreateWindowEx for ListView Child Wnd ID

#include "NTProcessInfo.h"

void InitDemoListView(IN HWND hWndLv);
DWORD EnumProcesses2Array(OUT smPROCESSINFO lpi[MAX_PI]);
void SetTextCallbackFlag(IN HWND hWndLv, IN const DWORD dwMaxItems);