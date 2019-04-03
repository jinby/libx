
/*---------------------------------------------------------------------------
szDllName:    需要替换DLL的名称,如Kernel32.dll/MSVCR71.dll等
szThumbName:  需要替换导入表的DLL,如DWGPreviewloc.dll/AcThumbnail16.dll等
szApiName:    函数名称,如ReadFile/fread等
lpNewProc:    替换的函数名称,如Hook_ReadFile/Hook_fread等
---------------------------------------------------------------------------*/
BOOL CheckNeedHook(IN LPCSTR DllName,OUT TCHAR *szThumbName,
				   OUT CHAR* szApiName,OUT CHAR *szChaneIATDll, DWORD_PTR **lpNewProc)
{
	ULONG iHookCount;

	

	for(iHookCount= 0; iHookCount < HOOKDLL_NUMS; iHookCount++)
	{
		if (0 == _stricmp(DllName,g_NeedHookNames[iHookCount].szThumbAnsiName))
		{
			strncpy(szApiName,g_NeedHookNames[iHookCount].szApiName,32);
			lstrcpy(szThumbName,g_NeedHookNames[iHookCount].szThumbUnilName);
			strncpy(szChaneIATDll, g_NeedHookNames[iHookCount].szDllName,32);

			*lpNewProc = g_NeedHookNames[iHookCount].lpNewProc;
		
			return TRUE;
		}
	}
	return FALSE;
}


//查找动态库模块
BOOL CheckDllMoudle(IN TCHAR *ThumbNailHander,IN char* dllName)
{
	HMODULE hMod =NULL;

	hMod= ::GetModuleHandle (ThumbNailHander);

	if (hMod == NULL)
	{
		return FALSE;
	}

	DWORD dwSize = 0;
	IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)hMod;  
	IMAGE_OPTIONAL_HEADER *pOptHeader = (IMAGE_OPTIONAL_HEADER *)( (BYTE *)hMod + pDosHeader->e_lfanew + 24 );  

	//PIMAGE_IMPORT_DESCRIPTOR pImageImport = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hProcess,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&dwSize);
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *)( (BYTE *)hMod + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);  
	while( pImportDesc->FirstThunk)  
	{  
		char *pszDllName = (char *) ( (BYTE *)hMod + pImportDesc->Name);  
		//OutputDebugString(_T("========this=======dll module name========"));
		//OutputDebugStringA(pszDllName);
		if (stricmp(pszDllName,dllName) == 0)
		{
			return TRUE;
		}

		pImportDesc++;  
	}  

	return FALSE;
}



int replace_IAT_TUM(IN const CHAR *pDllName,IN TCHAR *ThumbNailHander,
					IN const CHAR *pApiName,IN DWORD_PTR *lpNewProc,IN BOOL bReplace)
{
	
	HMODULE hMod ;

	if (ThumbNailHander == NULL)
	{
		hMod= ::GetModuleHandle (NULL);
	}
	else
	{
		hMod= ::GetModuleHandle (ThumbNailHander);
	}

	if (hMod == NULL)
	{
		//OutputDebugString(_T("GetModuleHandle  Fail"));
		return 0;
	}

	IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)hMod;  


	IMAGE_OPTIONAL_HEADER *pOptHeader = (IMAGE_OPTIONAL_HEADER *)( (BYTE *)hMod + pDosHeader->e_lfanew + 24 );  
	PIMAGE_NT_HEADERS pNtHeade=NULL;

	ULONG dwSize;
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *)( (BYTE *)hMod + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);  

	while(pImportDesc->FirstThunk)  
	{  
		char *pszDllName = (char *) ( (BYTE *)hMod + pImportDesc->Name);  
	
		if(_stricmp(pszDllName, pDllName) == 0 )  
		{  
			break;  
		}  
		pImportDesc++;  
	}  
		
	PIMAGE_THUNK_DATA   pOrigThunk   =   (PIMAGE_THUNK_DATA)((DWORD_PTR)hMod   +   (DWORD_PTR)(pImportDesc-> OriginalFirstThunk));   
	//第一个IAT项的Thunk地址。 
	PIMAGE_THUNK_DATA   pRealThunk   =   (PIMAGE_THUNK_DATA)((DWORD_PTR)hMod   +   (DWORD_PTR)(pImportDesc-> FirstThunk));   
	//循环查找被截API函数的IAT项，并使用替代函数地址修改其值。 
	while(pOrigThunk-> u1.Function)   
	{   
		//检测此Thunk是否为IAT项。 
		if((pOrigThunk-> u1.Ordinal   &   IMAGE_ORDINAL_FLAG)   !=   IMAGE_ORDINAL_FLAG)   
		{ 
			//获取此IAT项所描述的函数名称。 
			PIMAGE_IMPORT_BY_NAME   pByName   =(PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)hMod+(DWORD_PTR)(pOrigThunk-> u1.AddressOfData));   
			if(pByName-> Name[0]   ==   '\0 ')  
			{
				return   FALSE;   
			}
			////检测是否为挡截函数。 
			//OutputDebugString(L"------dll iat moudle name");
			//OutputDebugStringA((char*)pByName->Name);
			if(stricmp(pApiName, (char*)pByName->Name)   ==   0)   
			{   
				DWORD_PTR *lpAddr = (DWORD_PTR *)&pRealThunk->u1.Function;
				char bufmessage[512];
				DWORD_PTR dwError;
				DWORD dwOldProtect;  
								
				MEMORY_BASIC_INFORMATION   mbi_thunk; 
				//查询修改页的信息。 
				VirtualQuery(lpAddr,   &mbi_thunk,   sizeof(MEMORY_BASIC_INFORMATION));   
				
				//改变修改页保护属性为PAGE_READWRITE。 
				VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize,PAGE_READWRITE,&mbi_thunk.Protect);   
				
				WriteProcessMemory(::GetCurrentProcess(), lpAddr, &lpNewProc, sizeof(lpNewProc), NULL);
				
				//恢复修改页保护属性。 
				VirtualProtect(mbi_thunk.BaseAddress,   mbi_thunk.RegionSize,   PAGE_EXECUTE,   &dwOldProtect);   
				
			}   
		}   
		pOrigThunk++;   
		pRealThunk++;   
	}
	
	return 1;
}



HMODULE WINAPI Mine_LoadLibraryExW(LPCWSTR LibNameW, HANDLE hFile, DWORD dwFlags)
{
	HMODULE hModule;
	CHAR   szAnsiLibName[MAX_PATH]; 
	CHAR  szHookApiName[32]={0};
	CHAR  szHookDllName[32]={0};
	TCHAR  szThumbName[32]={0};
	DWORD_PTR *lpNewProc;
	

	hModule =  Real_LoadLibraryExW(LibNameW,hFile,dwFlags);

	if(hModule)
	{

		WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,LibNameW,-1,szAnsiLibName,sizeof(szAnsiLibName),NULL,NULL);   
		PCHAR pDllName = ParseFileNameA(szAnsiLibName);
		//OutputDebugStringA(pDllName);
	
		if (stricmp(pDllName,"zwcad.exe") == 0)
		{
			if (CheckDllMoudle(_T("DWGPreview.dll"),"MSVCR90.dll") == TRUE
				&& CheckDllMoudle(_T("DWGPreview.dll"),"mfc90u.dll") == TRUE)
			{

				//OutputDebugStringA("CheckDllMoudle TRUE zhongwang 2011");
				replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"MapViewOfFile",(DWORD_PTR *)Hook_MapViewOfFile,TRUE);
				replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"CreateFileW",(DWORD_PTR *)Hook_CreateFileW,TRUE);
				replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"CreateFileMappingW",(DWORD_PTR *)Hook_CreateFileMappingW,TRUE);
				replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"UnmapViewOfFile",(DWORD_PTR *)Hook_UnmapViewOfFile,TRUE);
			}
			//replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"ReadFile",(DWORD_PTR *)Hook_ReadFile,TRUE);
			//zhongwang cad 2010
			if (CheckDllMoudle(_T("DWGPreview.dll"),"MSVCR90.dll") == TRUE
				&& CheckDllMoudle(_T("DWGPreview.dll"),"mfc90.dll") == TRUE)
			{
				g_EZhongwangDllVer= MSVCR_90;
				//OutputDebugStringA("zhongwang cad 2010 version");
				replace_IAT_TUM("MSVCR90.DLL",_T("DWGPreview.dll"),"fread",(DWORD_PTR*)Hook_fread,TRUE);
			}
			else
			{
				g_EZhongwangDllVer = MSVCR_71;
				replace_IAT_TUM("msvcr71.dll",_T("DWGPreview.dll"),"fread",(DWORD_PTR *)Hook_fread,TRUE);

			}
			return hModule;
		}
		if (stricmp(pDllName,"acad.exe") == 0)
		{
			
			

			replace_IAT_TUM("KERNEL32.DLL",_T("AcThumbnail16.dll"),"ReadFile",(DWORD_PTR *)Hook_ReadFile,TRUE); //for autocad of win2k
			//replace_IAT_TUM("KERNEL32.DLL",_T("AcSignCore16.dll"),"ReadFile",(DWORD_PTR *)Hook_ReadFile,TRUE); //for autocad of win2k
			return hModule;

		}
		if(CheckNeedHook(pDllName,szThumbName,szHookApiName,szHookDllName,&lpNewProc))
		{
			//OutputDebugStringA("CheckNeedHook");
			if (stricmp(pDllName,"DWGPreview.dll") == 0)
			{
				//ZHONGWANG CAD 2011
				if (CheckDllMoudle(_T("DWGPreview.dll"),"MSVCR90.dll") == TRUE
					&& CheckDllMoudle(_T("DWGPreview.dll"),"mfc90u.dll") == TRUE)
				{

					//OutputDebugStringA("CheckDllMoudle TRUE zhongwang 2011");
					replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"MapViewOfFile",(DWORD_PTR *)Hook_MapViewOfFile,TRUE);
					replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"CreateFileW",(DWORD_PTR *)Hook_CreateFileW,TRUE);
					replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"CreateFileMappingW",(DWORD_PTR *)Hook_CreateFileMappingW,TRUE);
					replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"UnmapViewOfFile",(DWORD_PTR *)Hook_UnmapViewOfFile,TRUE);
				}
				//replace_IAT_TUM("KERNEL32.DLL",_T("DWGPreview.dll"),"ReadFile",(DWORD_PTR *)Hook_ReadFile,TRUE);
				//zhongwang cad 2010
				if (CheckDllMoudle(_T("DWGPreview.dll"),"MSVCR90.dll") == TRUE
					&& CheckDllMoudle(_T("DWGPreview.dll"),"mfc90.dll") == TRUE)
				{
					g_EZhongwangDllVer= MSVCR_90;
					//OutputDebugStringA("zhongwang cad 2010 version");
					replace_IAT_TUM("MSVCR90.DLL",_T("DWGPreview.dll"),"fread",(DWORD_PTR*)Hook_fread,TRUE);

				}
				else
				{

					g_EZhongwangDllVer = MSVCR_71;
					//OutputDebugStringA("zhongwang cad under 2010 version");
					replace_IAT_TUM(szHookDllName,szThumbName,szHookApiName,lpNewProc,TRUE);

				}
				return hModule;
			}
			//autocad 
			//OutputDebugStringA("auto cad is");
			replace_IAT_TUM(szHookDllName,szThumbName,szHookApiName,lpNewProc,TRUE);
		}
	}

	return hModule;
}