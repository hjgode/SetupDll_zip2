// TestUnzip.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestUnzip.h"
#include <commctrl.h>

#include "../unzip.h"
#include "../nclog.h"
#include "../myWin.h"
#include "../iniutil.h"
#include "../registry.h"

//some globals
#define MYINIFILE L"SetupDll_zip2.ini"
TCHAR m_unzipBasedir[MAX_PATH];
TCHAR m_instlDir[MAX_PATH];
BOOL  m_doReboot = false;
TCHAR m_zipFilename[MAX_PATH];

void readIni();
void doUnzip(TCHAR* pszInstallDir);
void unzipWithProgress(const TCHAR*, TCHAR* );
#define MYREGENTRY L"Software\\SetupDll_ZIP2"
// end of local Globals

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			hInst;			// The current instance
HWND				hwndCB;			// The command bar handle

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TESTUNZIP);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTUNZIP));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_TESTUNZIP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance, szWindowClass);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{	
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	if (hwndCB)
		CommandBar_Show(hwndCB, TRUE);

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
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	TCHAR szHello[MAX_LOADSTRING];

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_HELP_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_TEST:
					doUnzip(L"\\Temp");
					break;
				case IDM_FILE_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CREATE:
			hwndCB = CommandBar_Create(hInst, hWnd, 1);			
			CommandBar_InsertMenubar(hwndCB, hInst, IDM_MENU, 0);
			CommandBar_AddAdornments(hwndCB, 0, 0);
			break;
		case WM_PAINT:
			RECT rt;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);
			LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
			DrawText(hdc, szHello, _tcslen(szHello), &rt, 
				DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			CommandBar_Destroy(hwndCB);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for the About box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rt, rt1;
	int DlgWidth, DlgHeight;	// dialog width and height in pixel units
	int NewPosX, NewPosY;

	switch (message)
	{
		case WM_INITDIALOG:
			// trying to center the About dialog
			if (GetWindowRect(hDlg, &rt1)) {
				GetClientRect(GetParent(hDlg), &rt);
				DlgWidth	= rt1.right - rt1.left;
				DlgHeight	= rt1.bottom - rt1.top ;
				NewPosX		= (rt.right - rt.left - DlgWidth)/2;
				NewPosY		= (rt.bottom - rt.top - DlgHeight)/2;
				
				// if the About box is larger than the physical screen 
				if (NewPosX < 0) NewPosX = 0;
				if (NewPosY < 0) NewPosY = 0;
				SetWindowPos(hDlg, 0, NewPosX, NewPosY,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			return TRUE;

		case WM_COMMAND:
			if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

void doUnzip(TCHAR* pszInstallDir){

	if(pszInstallDir!=NULL)
		wsprintf(m_instlDir, pszInstallDir);
	else
		wsprintf(m_instlDir, L"\\Temp\\");

	readIni();

	showWindow(GetModuleHandle(NULL));

	TCHAR* zipFile = new TCHAR[MAX_PATH];
	wsprintf(zipFile, L"%s%s", m_instlDir, m_zipFilename);
/*
	nclog(L"Install_Exit: Calling unzipFileWithPath with '%s' and '%s'\n", zipFile, m_unzipBasedir);
	unzipFileWithPath(zipFile, m_unzipBasedir);
*/
	nclog(L"Install_Exit: Calling unzipWithProgress with '%s' and '%s'\n", zipFile, m_unzipBasedir);
	unzipWithProgress(zipFile, m_unzipBasedir);

}

// this needs the global var m_instlDir (with a trailing \) to be set!
void readIni(){
	nclog(L"readIni ENTRY: instlDir='%s', basedir='%s', zipname='%s', reboot=%i\n", m_instlDir, m_unzipBasedir, m_zipFilename, m_doReboot);

	TCHAR tempStr[MAX_PATH];

	//first try the ini file
	TCHAR* iniFile;
	iniFile = new TCHAR[MAX_PATH];

	//add a backslash
	TCHAR lastchar = m_instlDir[_tcslen(m_instlDir)-1];
	if (lastchar!='\\' && lastchar!='/') 
		_tcscat(m_instlDir,_T("\\"));

	wsprintf(iniFile, L"%s%s", m_instlDir, MYINIFILE);
	//try to read basedir from the ini file
	BOOL bRes = ReadIni(L"unzip", L"basedir", tempStr, MAX_PATH, iniFile);
	if(bRes){
		//basedir
		wsprintf(m_unzipBasedir, tempStr);

		//reboot
		bRes=ReadIni(L"unzip", L"reboot", tempStr, MAX_PATH, iniFile);
		if(bRes){
			if(wcscmp(tempStr, L"1")==0)
				m_doReboot=TRUE;
			else
				m_doReboot=FALSE;
		}
		else
			m_doReboot=FALSE;
		//zip file name
		bRes=ReadIni(L"unzip", L"zipfile", tempStr, MAX_PATH, iniFile);
		if(bRes)
			wsprintf(m_zipFilename, tempStr);
		else
			wsprintf(m_zipFilename, L"install.zip"); 
	}
	else{
		//now try the registry
		if(OpenKey(MYREGENTRY)==ERROR_SUCCESS){
			if(RegReadStr(L"unzipbasedir", tempStr)==0)
				wsprintf(m_unzipBasedir, tempStr);
			else
				wsprintf(m_unzipBasedir, L"");

			//add a backslash
			TCHAR lastchar = m_unzipBasedir[_tcslen(m_unzipBasedir)-1];
			if (lastchar!='\\' && lastchar!='/') 
				_tcscat(m_unzipBasedir,_T("\\"));


			if (RegReadStr(L"zipfilename", tempStr)==0)
				wsprintf(m_zipFilename, tempStr);
			else
				wsprintf(m_zipFilename, L"install.zip");
			
			if (RegReadStr(L"doreboot", tempStr)==0){
				if(wcscmp(tempStr, L"1")==0)
					m_doReboot=TRUE;
				else
					m_doReboot=FALSE;
			}
			else
				m_doReboot=FALSE;
			CloseKey();
		}
	}
	nclog(L"readIni EXIT: instlDir='%s', basedir='%s', zipname='%s', reboot=%i\n", m_instlDir, m_unzipBasedir, m_zipFilename, m_doReboot);
}

void EnsureDirectory1(const TCHAR *rootdir, const TCHAR *dir)
{ if (rootdir!=0 && GetFileAttributes(rootdir)==0xFFFFFFFF) CreateDirectory(rootdir,0);
  if (*dir==0) return;
  const TCHAR *lastslash=dir, *c=lastslash;
  while (*c!=0) {if (*c=='/' || *c=='\\') lastslash=c; c++;}
  const TCHAR *name=lastslash;
  if (lastslash!=dir)
  { TCHAR tmp[MAX_PATH]; memcpy(tmp,dir,sizeof(TCHAR)*(lastslash-dir));
    tmp[lastslash-dir]=0;
    EnsureDirectory1(rootdir,tmp);
    name++;
  }
  TCHAR cd[MAX_PATH]; *cd=0; if (rootdir!=0) _tcscpy(cd,rootdir); _tcscat(cd,dir);
  if (GetFileAttributes(cd)==0xFFFFFFFF) CreateDirectory(cd,NULL);
}

char *replace_char_in_string(char *string, char to_search, char to_replace_with)
{
    char *buf;
    int i=0,j=0;
    char temp[1024];

    while(string[i] !='\0')
    {
		if(string[i] == to_search )
		{
			temp[j] = to_replace_with;
			i++;
			j++;
		}
		else
			temp[j++] = string[i++];
    }
    temp[j] = '\0';
    buf = (char *) malloc(strlen(temp) + 1);
    strcpy(buf,temp);
    // printf("%s\n",buf);
    return buf;
}

BOOL abort_p=false;
void unzipWithProgress(const TCHAR *zipfn, TCHAR* baseDir)
{ 
	nclog(L"SetupDll unzipWithProgress version: 1.3\n");

	nclog(L"unzipWithProgress started\n");
	HZIP hz = OpenZip(zipfn,0);
	if(hz==0){
		nclog(L"OpenZip for '%s' failed.\n", zipfn);
	}

	unsigned long uRes = SetUnzipBaseDir(hz, baseDir);
	nclog(L"SetUnzipBaseDir to '%s' returned %u\n", baseDir, uRes);

	nclog(L"OpenZip OK. Getting zip entries\n");

	ZIPENTRY ze; 
	uRes = GetZipItem(hz,-1,&ze); 
	if (uRes == ZR_OK){
		nclog(L"Found %i zip entries\n", ze.index);

		int numentries=ze.index;
		// first we retrieve the total size of all zip items
		DWORD tot=0; 
		int i; //counter var
		for (i=0; i<numentries; i++) {
			GetZipItem(hz,i,&ze); 
			tot+=ze.unc_size;
		}
		nclog(L"Total size=%l\n", tot);

		iProgressMax=tot;
		SendMessage(FindWindow(L"myWin", NULL), WM_UPDATE_TEXT, 0, 0);

		//
		DWORD countall=0; // this is our progress so far
		for (i=0; i<numentries && !abort_p; i++){ 

			nclog(L"Trying to get zip entry #%i\n", i);
			uRes = GetZipItem(hz,i,&ze);

			if(uRes==ZR_OK){
				//TCHAR* szMsg = new TCHAR[MAX_PATH];
				// We'll unzip each file bit by bit, to a file on disk
				TCHAR fn[1024];
				TCHAR szMsg[MAX_PATH+1];
				DWORD dwRes=0;
				wsprintf(fn, L"\\z\\%s", ze.name);
				wsprintf(szMsg, L"%s", ze.name);
				nclog(L"\tSendMessage '%s'\n", szMsg);

				wsprintf(szPuffer, szMsg);
				SendMessage(FindWindow(L"myWin", NULL), WM_UPDATE_TEXT, 0, 0);
				nclog(L"\tGot name of zip entry '%s'\n", ze.name);

				//NEW: to ensure we can overwrite IN_ROM files, we will use a temporary file and copy that later
/*				TCHAR tempFilename[MAX_PATH];
				wsprintf(tempFilename, L"\\Temp\\%s", ze.name);

				TCHAR pathOnly[MAX_PATH]; TCHAR filenameOnly[MAX_PATH];
				stripfilenameandpath(tempFilename, pathOnly, filenameOnly);
				createDirs(L"\\Temp", pathOnly);

				TCHAR finalFilename[MAX_PATH];
				wsprintf(finalFilename, L"%s%s", baseDir, ze.name);
*/
				
				TCHAR dst[MAX_PATH]; 
				wsprintf(dst, L"%s%s", baseDir, ze.name);				
				
				char dstA[MAX_PATH];
				char *dstTemp=new char[MAX_PATH];
				wcstombs(dstA, dst, MAX_PATH);
				//make unique backslash
				dstTemp = replace_char_in_string(dstA, '/', '\\');
				mbstowcs(dst, dstTemp, MAX_PATH);

				nclog(L"\t********** dst = '%s'\n", dst);

				const TCHAR *ufn = (const TCHAR*)dst;
				const TCHAR *name=ufn; 
				const TCHAR *c=name; 
				while (*c!=0) {
					if (*c=='/' || *c=='\\') 
						name=c+1; 
					c++;
				}

				TCHAR dir[MAX_PATH]; 
				_tcscpy(dir,ufn); 
				if (name==ufn) 
					*dir=0; 
				else 
					dir[name-ufn]=0;

				TCHAR fName[MAX_PATH]; 
				bool isabsolute = (dir[0]=='/' || dir[0]=='\\' || (dir[0]!=0 && dir[1]==':'));
				if (isabsolute) {
					wsprintf(fName,_T("%s%s"),dir,name); 
					EnsureDirectory1(0,dir);
				}
				//else {wsprintf(fName,_T("%s%s%s"),rootdir,dir,name); EnsureDirectory1(rootdir,dir);}

				HANDLE hf = CreateFile(fName /*fn*/,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				nclog(L"CreateFile with '%s'\n", fName);

				//use a temp file
				//HANDLE hf = CreateFile(tempFilename,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				char buf[16384]; // Each chunk will be 16k big. After each chunk, we show progress
				DWORD countfile=0;
				for (ZRESULT zr=ZR_MORE; zr==ZR_MORE && !abort_p; ) // nb. the global "abort_p" flag is set by the user clicking the Cancel button.
				{ 
					zr=UnzipItem(hz,i,buf,16384);
					nclog(L"calling UnzipItem returned %i\n", zr);
					unsigned long bufsize=16384; 
					if (zr==ZR_OK) 
						bufsize=ze.unc_size-countfile; // bufsize is how much we got this time
					DWORD writ; 
					WriteFile(hf,buf,bufsize,&writ,0); 
					countfile+=bufsize; // countfile counts how much of this file we've unzipped so far
					countall+=bufsize; // countall counts how much total we've unzipped so far
					// Now show progress, and let Windows catch up...
					int iPos = (int)(100.0*((double)countall)/((double)tot));
					
//					nclog(".");
//					wcscat(szPuffer, L".\0");
					iProgressPos=iPos;
					SendMessage(FindWindow(L"myWin", NULL), WM_UPDATE_TEXT, 0, 0);
					
					//PumpMessages();
				}
				if(ze.unc_size==0 && ze.attr!=FILE_ATTRIBUTE_DIRECTORY){
					nclog("L...touching 0 bytes file\n");
					DWORD writ=0;
					WriteFile(hf,buf,0,&writ,0); //
				}
				CloseHandle(hf);

				//now copy the temp file on top of the target file
/*				if (CopyFile(tempFilename, finalFilename, FALSE))
					nclog(L"Copying tempfile '%s' to target '%s' OK\n", tempFilename, finalFilename);
				else
					nclog(L"Copying tempfile '%s' to target '%s' FAILED with 0x%0x\n", tempFilename, finalFilename, GetLastError());

				DeleteFile(tempFilename);
*/
				if (abort_p) 
					DeleteFile(fn); //finalFilename);
			}
			else
				nclog(L"\tGetZipItem failed \n");

			nclog(L"\n");
		} //for
		CloseZip(hz);
	}
	SendMessage(FindWindow(L"myWin", NULL), WM_QUIT, 0, 0);
}
