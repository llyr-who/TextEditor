// TextEditor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TextEditor.h"
#include "FileHandler.h"

#define MAX_LOADSTRING 100 // Put this in stdafx

const char g_szClassName[] = "myWindowClass";
const char g_szChildClassName[] = "myMDIChildWindowClass";

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hMDIClient = NULL;
HWND g_hMainWindow = NULL;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL				SetUpMDIChildWindowClass(HINSTANCE hInstance);
LRESULT CALLBACK	MDIChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEXTEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEXTEDITOR));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        if (!TranslateMDISysAccel(g_hMainWindow, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEXW);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEXTEDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TEXTEDITOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
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

   if (!SetUpMDIChildWindowClass(hInstance))
	   return 0;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hMainWindow = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// 
HWND CreateNewMDIChild(HWND hMDIClient)
{
	MDICREATESTRUCTA mcs;
	HWND hChild;

	mcs.szTitle = "[Untitled]";
	mcs.szClass = g_szChildClassName;
	mcs.hOwner = GetModuleHandle(NULL);
	mcs.x = mcs.cx = CW_USEDEFAULT;
	mcs.y = mcs.cy = CW_USEDEFAULT;
	mcs.style = MDIS_ALLCHILDSTYLES;

	hChild = (HWND)SendMessageA(hMDIClient, WM_MDICREATE, 0, (LPARAM)&mcs);
	if (!hChild)
	{
		DWORD err = GetLastError(); 
		MessageBoxA(hMDIClient, "MDI Child creation failed.", "Fucking hell!",
			MB_ICONEXCLAMATION | MB_OK);
	}
	return hChild;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		HFONT hfDefault;
		HWND hEdit; // handle to the edit box
		HWND hTool; // handle to the toolbar
		TBBUTTON tbb[3];
		TBADDBITMAP tbab;
		HWND hStatus;
		CLIENTCREATESTRUCT ccs;
		int statwidths[] = { 100, -1 };

		// Create the MDI client

		// Get handle to the window menu where children are listed
		ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 2);
		ccs.idFirstChild = ID_MDI_FIRSTCHILD;

		g_hMDIClient = CreateWindowEx(WS_EX_CLIENTEDGE, _T("mdiclient"), NULL,
			WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hWnd, (HMENU)IDC_MAIN_MDI, GetModuleHandle(NULL), (LPVOID)&ccs);

		if (g_hMDIClient == NULL)
			MessageBox(hWnd, _T("Could not create MDI client."), _T("Error"), MB_OK | MB_ICONERROR);

		
		// Create the toolbar
		hTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
			hWnd, (HMENU)IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL);
		if (hTool == NULL)
			MessageBox(hWnd, _T("Could not create tool bar."), _T("Error"), MB_OK | MB_ICONERROR);
		// Send the TB_BUTTONSTRUCTSIZE message, which is required for
		// backward compatibility.
		SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

		tbab.hInst = HINST_COMMCTRL;
		tbab.nID = IDB_STD_SMALL_COLOR;
		SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

		ZeroMemory(tbb, sizeof(tbb));
		tbb[0].iBitmap = STD_FILENEW;
		tbb[0].fsState = TBSTATE_ENABLED;
		tbb[0].fsStyle = TBSTYLE_BUTTON;
		tbb[0].idCommand = IDM_FILE_NEW;

		tbb[1].iBitmap = STD_FILEOPEN;
		tbb[1].fsState = TBSTATE_ENABLED;
		tbb[1].fsStyle = TBSTYLE_BUTTON;
		tbb[1].idCommand = IDM_FILE_OPEN;

		tbb[2].iBitmap = STD_FILESAVE;
		tbb[2].fsState = TBSTATE_ENABLED;
		tbb[2].fsStyle = TBSTYLE_BUTTON;
		tbb[2].idCommand = IDM_FILE_SAVEAS;

		SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb) / sizeof(TBBUTTON), (LPARAM)&tbb);

		// Create Status bar
		hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
			WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
			hWnd, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);

		SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM)statwidths);
		SendMessage(hStatus, SB_SETTEXTA, 0, (LPARAM)"Hi Anthony!");
	}
	break;
	case WM_SIZE:
	{
		HWND hTool;
		RECT rcTool;
		int iToolHeight;

		HWND hStatus;
		RECT rcStatus;
		int iStatusHeight;

		HWND hEdit;
		int iEditHeight;
		RECT rcClient;

		// Size toolbar and get height
		hTool = GetDlgItem(hWnd, IDC_MAIN_TOOL);
		SendMessage(hTool, TB_AUTOSIZE, 0, 0);
		GetWindowRect(hTool, &rcTool);
		iToolHeight = rcTool.bottom - rcTool.top;

		// Size status bar and get height
		hStatus = GetDlgItem(hWnd, IDC_MAIN_STATUS);
		SendMessage(hStatus, WM_SIZE, 0, 0);
		GetWindowRect(hStatus, &rcStatus);
		iStatusHeight = rcStatus.bottom - rcStatus.top;

		// Calculate remaining height and size edit
		GetClientRect(hWnd, &rcClient);
		iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;
		hEdit = GetDlgItem(hWnd, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, iToolHeight, rcClient.right, iEditHeight, SWP_NOZORDER);
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_FILE_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case IDM_FILE_NEW:
			CreateNewMDIChild(g_hMDIClient);
			break;
		case IDM_FILE_OPEN:
		{
			HWND hChild = CreateNewMDIChild(g_hMDIClient);
			if (hChild)
			{
				DoFileOpen(hChild, g_hMainWindow);
			}
		}
		break;
		case IDM_FILE_CLOSE:
		{
			HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
			if (hChild)
			{
				SendMessage(hChild, WM_CLOSE, 0, 0);
			}
		}
		break;
		case IDM_WINDOW_TITLE:
			SendMessage(g_hMDIClient, WM_MDITILE, 0, 0);
			break;
		case IDM_WINDOW_CASCADE:
			SendMessage(g_hMDIClient, WM_MDICASCADE, 0, 0);
			break;
		default:
		{
			if (LOWORD(wParam) >= ID_MDI_FIRSTCHILD)
			{
				DefFrameProc(hWnd, g_hMDIClient, WM_COMMAND, wParam, lParam);
			}
			else
			{
				HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
				if (hChild)
				{
					SendMessage(hChild, WM_COMMAND, wParam, lParam);
				}
			}
		}
		}
	}
	break;
    default:
        return DefFrameProc(hWnd, g_hMDIClient, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK MDIChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MDICREATE:
	{
		HFONT hfDefault;
		HWND hEdit;

		// Create Edit Control

		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
			0, 0, 100, 100, hwnd, (HMENU)IDC_CHILD_EDIT, GetModuleHandle(NULL), NULL);
		if (hEdit == NULL)
			MessageBox(hwnd, _T("Could not create edit box."), _T("Error"), MB_OK | MB_ICONERROR);

		hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	}
	break;
	case WM_MDIACTIVATE:
	{
		HMENU hMenu, hFileMenu;
		UINT EnableFlag;

		hMenu = GetMenu(g_hMainWindow);
		if (hwnd == (HWND)lParam)
		{	   //being activated, enable the menus
			EnableFlag = MF_ENABLED;
		}
		else
		{						   //being de-activated, gray the menus
			EnableFlag = MF_GRAYED;
		}

		EnableMenuItem(hMenu, 1, MF_BYPOSITION | EnableFlag);
		EnableMenuItem(hMenu, 2, MF_BYPOSITION | EnableFlag);

		hFileMenu = GetSubMenu(hMenu, 0);
		EnableMenuItem(hFileMenu, IDM_FILE_SAVEAS, MF_BYCOMMAND | EnableFlag);

		EnableMenuItem(hFileMenu, IDM_FILE_CLOSE, MF_BYCOMMAND | EnableFlag);
		EnableMenuItem(hFileMenu, IDM_FILE_CLOSEALL, MF_BYCOMMAND | EnableFlag);

		DrawMenuBar(g_hMainWindow);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FILE_OPEN:
			DoFileOpen(hwnd, g_hMainWindow);
			break;
		case IDM_FILE_SAVEAS:
			DoFileSave(hwnd, g_hMainWindow);
			break;
		case IDM_EDIT_CUT:
			SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_CUT, 0, 0);
			break;
		case IDM_EDIT_COPY:
			SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_COPY, 0, 0);
			break;
		case IDM_EDIT_PASTE:
			SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_PASTE, 0, 0);
			break;
		}
		break;
	case WM_SIZE:
	{
		HWND hEdit;
		RECT rcClient;

		// Calculate remaining height and size edit

		GetClientRect(hwnd, &rcClient);

		hEdit = GetDlgItem(hwnd, IDC_CHILD_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
	}
	return DefMDIChildProc(hwnd, msg, wParam, lParam);
	default:
		return DefMDIChildProc(hwnd, msg, wParam, lParam);

	}
	return 0;
}

BOOL SetUpMDIChildWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXA wc;

	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MDIChildWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szChildClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassExA(&wc))
	{
		MessageBox(0, _T("Could Not Register Child Window"), _T("Oh Oh..."),
			MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}
	else
		return TRUE;
}



// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
