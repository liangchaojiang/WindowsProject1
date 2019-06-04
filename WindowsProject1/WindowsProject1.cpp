// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WindowsProject1.h"
#include <bluetoothapis.h>


#define MAX_LOADSTRING 100

#pragma comment (lib,"Bthprops.lib")
HANDLE hCom;
HWND hwnd; 
LPARAM lparam;
WPARAM wparm;
COMSTAT Comstat;
OVERLAPPED OverLapped;

static HWND  button, serialShow, hWinRich;


// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HDC hdcglobal;
RECT rect1;
WCHAR wszClassName[2048];





// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL OpenSerial();
DWORD WINAPI ThreadRead(LPVOID lpParameter);
DWORD WINAPI WriteChar(WCHAR* m_szWriteBuffer, DWORD m_nToSend);
bool setuptimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant);






int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
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
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(MY_MENU);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	RECT rect;
	hwnd= hWnd;
	lparam = lParam;
	wparm = wParam;
	case WM_CREATE:
	{
		hWinRich = CreateWindowEx(WS_EX_CLIENTEDGE,TEXT("EDIT"), TEXT(""),
			 WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
			0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		button = CreateWindow(
			TEXT("BUTTON"),
			TEXT("BUTTON"),
			WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
			1000,
			0,
			150,
			100,
			hWnd,
			NULL,
			NULL,
			NULL
		);


	}
	case WM_SIZE:
		MoveWindow(hWinRich,0,0,300,200,TRUE);

    case WM_COMMAND:
        {
		if ((HWND)lParam == button) {

			if (OpenSerial() == FALSE) {

				MessageBox(hWnd, TEXT("打开串口失败"), TEXT("打开串口"), 0);

				InvalidateRgn(hWnd,NULL,TRUE);
				UpdateWindow(hWnd);
			}
			else {
				MessageBox(hWnd, TEXT("打开串口成功"), TEXT("打开串口"), 0);
				HANDLE HRead;
				HRead = CreateThread(NULL, 0, ThreadRead, NULL, 0, NULL);
				DestroyWindow(button);
			}
			

			break;
		}
			int wmId = LOWORD(wParam);
			WCHAR * temp = (WCHAR*)("this is a temp");
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_32784: 
				WriteChar(temp, 20);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }

        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd,&rect1);
			hdcglobal = hdc;
            // TODO: 在此处添加使用 hdc 的任何绘图代码...

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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

BOOL OpenSerial() {

	DCB dcb;
	COMMTIMEOUTS tim_out;
		hCom = CreateFile(
			TEXT("COM3"),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			0
		);
		if (hCom == (HANDLE)-1) {
			return FALSE;
		}
		if (!GetCommState(hCom, &dcb)) {
			return FALSE;
		}
		dcb.BaudRate = 115200;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		if (!SetCommState(hCom, &dcb)) {
			return FALSE;
		}
		if (!setuptimeout(0,0,0,0,0)) {
			return FALSE;
		}

		return TRUE;
}


DWORD WINAPI ThreadRead( LPVOID lpParameter) {
	bool bRead = TRUE;
	bool bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;

	GetClientRect(hwnd,&rect1);


	char RXBuff[1024];
	for (;;) {
		bResult = ClearCommError(hCom,&dwError,&Comstat);
		if (Comstat.cbInQue == 0)
			continue;
		if (bRead) {
			bResult = ReadFile(hCom,  //通信设备（此处为串口）句柄，由CreateFile()返回值得到  
				RXBuff,  //指向接收缓冲区
				1024,  //指明要从串口中读取的字节数  
				&BytesRead,   //  
				&OverLapped);  //OVERLAPPED结构  
			if (!bResult) {
				switch (dwError == GetLastError())
				{
				                 case ERROR_IO_PENDING:
					                     bRead = FALSE;
					                     break;
				                 default:
					                     break;
				}
			}
			hdcglobal = GetWindowDC(hwnd);

			memset(wszClassName,0,sizeof(wszClassName));
			MultiByteToWideChar(CP_ACP,0,RXBuff,strlen(RXBuff)+1,wszClassName,
				sizeof(wszClassName)/sizeof(wszClassName[0]));
			//DrawText(hdcglobal, wszClassName, -1, &rect1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			SendMessage(hWinRich, EN_SETFOCUS, -2, -1);
			SendMessage(hWinRich, EM_REPLACESEL, true, (LPARAM)wszClassName);
			//SetWindowText(hWinRich,wszClassName);
			//MessageBox(hwnd,LPCWSTR(RXBuff),TEXT("串口数据"),0);
		}
		else {
			bRead = TRUE;
		}
		Sleep(100);
	}

   if (!bRead)
   {
	   MessageBox(hwnd, TEXT("msgrrr"), TEXT("dsafd"), 0);
       bRead = TRUE;
      bResult = GetOverlappedResult(hCom,
          &OverLapped,
          &BytesRead,
           TRUE);
    }



}

DWORD WINAPI WriteChar(WCHAR* m_szWriteBuffer, DWORD m_nToSend) {
	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	DWORD dwRes;
	BOOL fRes;
	

	// Create this write operation's OVERLAPPED structure's hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL)
		// error creating overlapped event handle
		return FALSE;
	char * buff;
	buff = (char*)"AB";
	// Issue write.
	if (!WriteFile(hCom,buff , 5, &dwWritten, &osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			// WriteFile failed, but isn't delayed. Report error and abort.
			fRes = FALSE;
		}
		else
			// Write is pending.
			dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
		switch (dwRes)
		{
			// OVERLAPPED structure's event has been signaled. 
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hCom, &osWrite, &dwWritten, FALSE))
				fRes = FALSE;
			else
				// Write operation completed successfully.
				fRes = TRUE;
			break;

		default:
			// An error has occurred in WaitForSingleObject.
			// This usually indicates a problem with the
			// OVERLAPPED structure's event handle.
			fRes = FALSE;
			break;
		}
	}

	else
		// WriteFile completed immediately.
		fRes = TRUE;

	//	CloseHandle(osWrite.hEvent);   // close the port 
	return fRes;

}


bool setuptimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant)
{
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = ReadInterval;
	timeouts.ReadTotalTimeoutConstant = ReadTotalconstant;
	timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier;
	timeouts.WriteTotalTimeoutConstant = WriteTotalconstant;
	timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;
	if (!SetCommTimeouts(hCom, &timeouts))
	{
		return false;
	}
	else
		return true;
}


