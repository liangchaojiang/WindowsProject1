// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WindowsProject1.h"
#include <bluetoothapis.h>


#define MAX_LOADSTRING 100
#define MAX_SERIAL_BUFFER 4096


#pragma comment (lib,"Bthprops.lib")
HANDLE hCom;
HWND hwnd; 
LPARAM lparam;
WPARAM wparm;
COMSTAT Comstat;

static HWND  button, serialShow, hWinRich;


// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HDC hdcglobal;
RECT rect1;

static WCHAR messageOut[1024000]=L"init";

OVERLAPPED Rol = { 0 };
OVERLAPPED Wol = { 0 };




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
			TEXT("打开串口"),
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
		RECT rect2;
		GetWindowRect(hwnd,&rect2);
		MoveWindow(hWinRich,0,0,1000,500,TRUE);

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
			case ID_32776:
				SendMessage(hWinRich, EN_SETFOCUS, -2, -1);
				SendMessage(hWinRich, EM_REPLACESEL, true, (LPARAM)messageOut);
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
		hCom = CreateFile(
			TEXT("COM3"),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED|FILE_ATTRIBUTE_NORMAL,
			0
		);
		if (hCom == (HANDLE)-1) {
			return FALSE;
		}
		if (!SetupComm(hCom, 2048, 2048)) {
			CloseHandle(hCom);
			return FALSE;
		}
		if (!GetCommState(hCom, &dcb)) {
			CloseHandle(hCom);
			return FALSE;
		}
		else {
			dcb.DCBlength = sizeof(dcb);
			if (!BuildCommDCB(L"115200,n,8,1", &dcb)) {
				CloseHandle(hCom);
				return FALSE;
			}
			if (!SetCommState(hCom, &dcb)) {
				CloseHandle(hCom);
				return FALSE;
			}
		}

		if (!setuptimeout(MAXDWORD,0,0,50,2000)) {
			return FALSE;
		}

		return TRUE;
}


DWORD WINAPI ThreadRead( LPVOID lpParameter) {

	bool bResult;
	DWORD dwError;
	DWORD BytesRead;

	GetClientRect(hwnd,&rect1);
	hdcglobal = GetWindowDC(hwnd);

	static unsigned long nCurrectPos = 0;//指针当前位置


	for (;;) {

		DWORD dwWantRead = 100;
		DWORD dwRealRead;
		DWORD dwRet;
		DWORD dwRes;
		char pReadBuf [100];
		memset(pReadBuf, 0, sizeof(pReadBuf));
		WCHAR wszClassName[100];

		Rol.hEvent = CreateEvent(NULL,          //创建Rol的hEvent成员为无信号状态
			TRUE,
			FALSE,
			NULL);

		if (Rol.hEvent == NULL)
		{
			return -1;
		}

		memset(wszClassName, 0, sizeof(wszClassName));
		if (ClearCommError(hCom, &dwError, NULL)) {
			PurgeComm(hCom,PURGE_TXABORT|PURGE_TXCLEAR);
		}
		if (bResult=ReadFile(hCom, pReadBuf, dwWantRead, &dwRealRead, &Rol)) {

			MultiByteToWideChar(CP_ACP, 0, pReadBuf, strlen(pReadBuf) + 1, wszClassName,
				sizeof(wszClassName) / sizeof(wszClassName[0]));
			if (wszClassName[0]!=0) {
				if (nCurrectPos < 1024000) {
				for(int i =0;i<100;i++)
					if (wszClassName[i] != 0) {
						messageOut[nCurrectPos] = wszClassName[i];
						nCurrectPos += 1;
					}
					else
					{
						break;
					}
				}
			}
			

		}
		else {
			
			dwRet = GetLastError();

			if (!bResult && (dwRet == ERROR_IO_PENDING)) {

				dwRes = WaitForSingleObject(Rol.hEvent, 5000);   //5秒超时

				switch (dwRes)
				{
				case  WAIT_OBJECT_0:

					if (!GetOverlappedResult(hCom,
						&Rol,
						&dwRealRead,    //实际读出的字节数
						TRUE))     //TRUE表示直到操作完成函数才返回
					{
						//操作失败,可以使用GetLastError()获取错误信息
					}
					else
					{
						//操作成功完成,数据读取存入myByte中

						//这里加入处理数据的代码

					}
					break;
				case   WAIT_TIMEOUT:
					//读操作失败,原因是超时
					break;
				default:
					//这里加入默认处理代码
					break;
				}

			}
		}
		CloseHandle(Rol.hEvent);
		Sleep(100);
	
	}

}

DWORD WINAPI WriteChar(WCHAR* m_szWriteBuffer, DWORD m_nToSend) {
	DWORD dwRes;
	DWORD dwWrite;
	BYTE    myByte[10] = "AT\r\n";

	BOOL bResult;

	Wol.Internal = 0;        //设置OVERLAPPED结构Wol
	Wol.InternalHigh = 0;
	Wol.Offset = 0;
	Wol.OffsetHigh = 0;
	Wol.hEvent = CreateEvent(NULL,          //创建Wol的hEvent成员为无信号状态
		TRUE,
		FALSE,
		NULL);

	if (Wol.hEvent == NULL)
	{
		return -1;
	}


	bResult = WriteFile(hCom,         //串口句柄
		&myByte,     //存放待发送数据
		4,         //欲发送的字节数
		NULL,
		&Wol);       //指向创建hCom时的Wol的指针

	if (bResult)
	{
		//printf("send success \r\n");
	}

	if (!bResult) {

		dwRes = WaitForSingleObject(Wol.hEvent, 500);   //5ms超时
		switch (dwRes)
		{
		case   WAIT_OBJECT_0:
			if (!GetOverlappedResult(hCom,
				&Wol,
				&dwWrite,
				TRUE))     //TRUE表示直到操作完成函数才返回
			{
				//操作失败,可以使用GetLastError()获取错误信息
			}
			else
			{
				MessageBox(hwnd, TEXT("数据发送成功"), TEXT("打开串口"), 0);
				//发送数据成功
				//printf("send success dwWrite = %d \r\n", dwWrite);
				//这里加入发送成功的处理代码
			}
			break;
		case   WAIT_TIMEOUT:
			//读操作失败,原因是超时
			break;
		default:
			//这里加入默认处理代码
			break;
		}
	}

	return 0;
}


bool setuptimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant)
{
	COMMTIMEOUTS timeouts;
	memset(&timeouts, 0, sizeof(timeouts));
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



