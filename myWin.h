//myWin.h

//some window routines

#include "windows.h"
#include <commctrl.h>
#pragma comment (lib, "commctrl.lib ")
//"ComCtl32.lib")

#include "nclog.h"

#define MainClassName L"myWin"
#define WM_UPDATE_TEXT WM_USER + 1000

//global
HINSTANCE g_hInst=NULL;
TCHAR szPuffer[MAX_PATH];
TCHAR szPuffer2[MAX_PATH];

int iProgressMax=100;
int iProgressPos=0;

HWND hProgress=NULL;
HWND hTxtEdit1=NULL;
HWND hTxtEdit2=NULL;

const int iOffsetLeft = 10;
const int iOffsetTop = 10;

int iEwidth=240;
int iEheight=26;
int iEtxtHeight=14;

RECT screenRect;

HWND g_hwndParent=NULL;

LRESULT CALLBACK myWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){

	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	rect.bottom=52;
	rect.left=26;
	rect.right=200;
	rect.top=26;

	int x1,y1,x2,y2,w,h;
	RECT ClientRect;
	RECT txtRect1;

	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC  = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&InitCtrlEx);

	switch (uMsg){
	case WM_UPDATE_TEXT:
		nclog(L"+++++++++ WM_UPDATE_TEXT\n");
		nclog(L"---- szPuffer='%s'\n", szPuffer);
/*
		if(hTextWnd!=NULL){
			SetWindowText(hTextWnd, szMsg);
			UpdateWindow(hTextWnd);
		}
*/
		SetWindowText(hTxtEdit1, szPuffer);

		wsprintf(szPuffer2, L"Unzip in progress");
		SetWindowText(hTxtEdit2, szPuffer2);

		SendMessage(hProgress, PBM_SETPOS, iProgressPos, 0);

		InvalidateRect(hwnd, &ClientRect, TRUE);
		UpdateWindow(hwnd);
		break;

/*
Client Rectangle
      0,0                     x->
        ,-----------------------------------------------.
        | x,y                w                          |
        |  ,----------------------------------------.   |
        +  |                                        |   |
    y   | h|                                        |   |
    |   |  `----------------------------------------.   |
    v   |                                         x1,y1 |
        +                                               |
        |                                               |
        |                                               |
        +                                               |
        |                                               |
        |                                               |
        `-----------------------------------------------'
*/
	case WM_CREATE:
		GetWindowRect(hwnd, &ClientRect);
		wsprintf(szPuffer, L"WM_CREATE");
		
		x1=iOffsetLeft; //always same
		w=(ClientRect.right-ClientRect.left) - 2*iOffsetLeft; //always same
		h=iEheight;

		y1=iOffsetTop;
		y1=y1 - iEheight/2 + iOffsetTop; //move half the height to top
		x2=x1+w;
		y2=y1+h;

		//create first text row
		hTxtEdit2 = CreateWindowEx(0, L"Static" /*L"EDIT"*/, NULL, 
			WS_CHILD | WS_VISIBLE | SS_LEFT /*ES_READONLY*/, x1, y1, w, iEtxtHeight, hwnd, NULL, g_hInst, NULL);

		//move down 
		y1 = y1 + iEtxtHeight + iOffsetTop;

		//create second text row
		hTxtEdit1 = CreateWindowEx(0, L"Static" /*L"EDIT"*/, NULL, 
			WS_CHILD | WS_VISIBLE | SS_LEFT /*ES_READONLY*/, x1, y1, w, iEtxtHeight*2, hwnd, NULL, g_hInst, NULL);

		//move down 
		y1 = y1 + iEtxtHeight*2 + iOffsetTop;

		//create progress bar
		hProgress = CreateWindowEx(0, PROGRESS_CLASS, NULL,
		               WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
			      x1, y1, w, iEheight/2,
			      hwnd, NULL, g_hInst, NULL);



		DEBUGMSG(1, (L"+++++++++ WM_CREATE\n"));
		break;
	case WM_PAINT :
		DEBUGMSG(1, (L"+++++++++ WM_PAINT\n"));
		GetWindowRect(hwnd, &ClientRect);
		
		x1=iOffsetLeft;
		y1=(1*(ClientRect.bottom-ClientRect.top) /4) - 26/2;
		w=(ClientRect.right-ClientRect.left) - 2*iOffsetLeft;
		h=iEheight;
		x2=x1+w;
		y2=y1+h;

		hDC = BeginPaint(hwnd, &ps);
		txtRect1.left=x1;
		txtRect1.top=y1;
		txtRect1.right=x2;
		txtRect1.bottom=y2;

		//DrawText(hDC, szPuffer, -1, &txtRect1, DT_RIGHT /*DT_CENTER*/ | DT_VCENTER | DT_SINGLELINE);
//		TextOut(hDC, 0, 0, szPuffer, sizeof(szPuffer[0])*sizeof(szPuffer)-1);
		EndPaint(hwnd, &ps);
		break;
		//return 0;
	case WM_WININICHANGE:
		GetWindowRect(GetDesktopWindow(), &screenRect);
		int iScreenWidth, iScreenHeight;
		iScreenWidth=screenRect.right - screenRect.left;
		iScreenHeight=screenRect.bottom - screenRect.top;

		int x, y, w, h;
		x= iOffsetLeft;
		y= iScreenHeight/3; // 10;
		w= iScreenWidth-2*iOffsetLeft; // 220;
		h= iScreenHeight/3; // 80;
		if(h<95)
			h=95;
		SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOZORDER);
		break;
	case WM_DESTROY:
	case WM_QUIT:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam );

	}
	return 0; //TRUE;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter){

	HINSTANCE theInstance = (HINSTANCE) lpParameter;

	DEBUGMSG(1, (L"ThreadProc, HINSTANCE= %u\n", theInstance));

	MSG wmsg;
	WNDCLASS wc;
	//wc.cbSize=sizeof(WNDCLASSEX);
	wc.style          = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = myWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE)theInstance; //hInstance;
	wc.hIcon = NULL; // LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
	wc.hCursor = NULL; //LoadCursor(NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH)(COLOR_INFOBK);//COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL; //MainClassName;
	wc.lpszClassName = MainClassName;
	//wc.hIconSm         = NULL; //LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);

	ATOM aClass = RegisterClass(&wc);

	if(aClass==0){
		DEBUGMSG(1, (L"Failed to register class: %u\n", GetLastError()));
		return -1;
	}
	DWORD dwExStyle = 0;
	DWORD dwFlags = WS_BORDER | WS_POPUP | WS_VISIBLE;
	//TCHAR* szClass = L"SetupDLL_zip";
	TCHAR* szTitle = L"Installing...";

	GetWindowRect(GetDesktopWindow(), &screenRect);
	int iScreenWidth, iScreenHeight;
	iScreenWidth=screenRect.right - screenRect.left;
	iScreenHeight=screenRect.bottom - screenRect.top;

	int x, y, w, h;
	x= iOffsetLeft;
	y= iScreenHeight/3; // 10;
	w= iScreenWidth-2*iOffsetLeft; // 220;
	h= iScreenHeight/3; // 95;
	if(h<95)
		h=95;

	HWND hwndParent = FindWindow(L"Dialog", L"Installation"); // GetDesktopWindow();

	HINSTANCE hInstance = NULL;
	void* lpParam = NULL;
	HWND hWnd = CreateWindowEx(dwExStyle, MainClassName, szTitle, dwFlags, x, y, w, h, hwndParent, NULL, hInstance, lpParam);

	if(hWnd==NULL){
		DEBUGMSG(1, (L"Failed to create window: %u\n", GetLastError()));
		return -2;
	}

	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);
	while(GetMessage(&wmsg,NULL,0,0))
	{
		TranslateMessage(&wmsg);
		DispatchMessage(&wmsg);
	}
	return wmsg.wParam;

	return 0;
}

HANDLE hThread=NULL;

int showWindow(HANDLE hHandle){ 

	DWORD dwThreadID;
	DEBUGMSG(1, (L"showWindow, HANDLE= %u\n", hHandle));

	if(hThread==NULL)
		hThread = CreateThread(NULL, 0, &ThreadProc, (LPVOID) hHandle, NULL, &dwThreadID);
	if(hThread==NULL){
		DEBUGMSG(1, (L"Failed to create thread: %u\n", GetLastError()));
		return -1;
	}
	else
		return 0;
}