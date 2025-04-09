#include "tray.h"

/*
* 抄的
* https://blog.csdn.net/wangyi463295828/article/details/117442705
*/

HMENU tray::hMenu;
void* tray::http_server;
int (*tray::http_server_stop)(void*);
DWORD tray::tray_thread_id;
char* tray::web_ui_url = nullptr;

LRESULT CALLBACK tray::tray_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	NOTIFYICONDATA nid{};
	UINT WM_TASKBARCREATED;
	POINT pt;
	int item_id;
	// 不要修改TaskbarCreated，这是系统任务栏自定义的消息
	WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
	switch (message)
	{
	case WM_CREATE://窗口创建时候的消息.
		nid.cbSize = sizeof(nid);
		nid.hWnd = hwnd;
		nid.uID = 0;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_USER;
		nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		lstrcpy(nid.szTip, app_const::APP_NAME);
		Shell_NotifyIcon(NIM_ADD, &nid);
		hMenu = CreatePopupMenu();//生成菜单
		//添加选项
		AppendMenu(hMenu, MF_STRING, ITEM_ID_SHOW, TEXT("显示界面"));
		if (is_auto_run())
			AppendMenu(hMenu, MF_STRING, ITEM_ID_CLEARSTART, TEXT("清除开机自启动"));
		else
			AppendMenu(hMenu, MF_STRING, ITEM_ID_SETSTART, TEXT("开机自启动"));
		AppendMenu(hMenu, MF_STRING, ITEM_ID_QUIT, TEXT("退出"));
		Shell_NotifyIcon(NIM_ADD, &nid);
		break;
	case WM_USER:
		if (lParam == WM_LBUTTONDOWN) {
			//单击事件
			show_web_ui();
		}
		if (lParam == WM_RBUTTONDOWN)
		{
			GetCursorPos(&pt);//取鼠标坐标
			::SetForegroundWindow(hwnd);//解决在菜单外单击左键菜单不消失的问题
			//EnableMenuItem(hMenu, ITEM_ID_SHOW, MF_GRAYED);//让菜单中的某一项变灰
			item_id = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hwnd, NULL);//显示菜单并获取选项ID
			switch (item_id)
			{
			case ITEM_ID_SHOW:
				show_web_ui();
				break;
			case ITEM_ID_SETSTART:
				set_auto_run();
				ModifyMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ITEM_ID_CLEARSTART, TEXT("清除开机自启动"));
				break;
			case ITEM_ID_CLEARSTART:
				delete_auto_run();
				ModifyMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ITEM_ID_SETSTART, TEXT("开机自启动"));
				break;
			case ITEM_ID_QUIT:
				quit();
				break;
			default:
				break;
			}
		}
		break;
	case WM_DESTROY://窗口销毁时候的消息.
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;
	default:
		/*
		*   防止当Explorer.exe 崩溃以后，程序在系统系统托盘中的图标就消失
			原理：Explorer.exe 重新载入后会重建系统任务栏。当系统任务栏建立的时候会向系统内
			所有注册接收TaskbarCreated 消息的顶级窗口发送一条消息，我们只需要捕捉这个消
			息，并重建系统托盘的图标即可。
		*/
		if (message == WM_TASKBARCREATED)
			SendMessage(hwnd, WM_CREATE, wParam, lParam);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void tray::start(void* server, int(*server_stop)(void*), int server_port)
{
	//保存server对象和http_server_stop指针函数
	http_server = server;
	http_server_stop = server_stop;
	//根据端口号生成URL
	char port_n[10];
	sprintf_s(port_n, 10, "%d", server_port);
	if (web_ui_url) delete[] web_ui_url;
	web_ui_url = new char[URL_MAX_LEN];
	strcpy_s(web_ui_url, URL_MAX_LEN, "http://localhost:");
	strcat_s(web_ui_url, URL_MAX_LEN, port_n);
	strcat_s(web_ui_url, URL_MAX_LEN, "/");
	//启动tray线程
	t_thread = std::thread(tray_thread);
}

void tray::tray_thread()
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = tray_callback;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = app_const::APP_NAME;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), app_const::APP_NAME, MB_ICONERROR);
		return;
	}

	tray_thread_id = GetCurrentThreadId();

	// 设置WS_EX_TOOLWINDOW, 显示为systray
	hwnd = CreateWindowEx(WS_EX_TOOLWINDOW,
		app_const::APP_NAME, app_const::APP_NAME,
		WS_POPUP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL, NULL, NULL, NULL);

	ShowWindow(hwnd, 0);
	UpdateWindow(hwnd);

	//tmd, 一个程序三个消息循环......
	//消息循环
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	printf("tray quit test\n");
}

void tray::quit()
{
	http_server_stop(http_server);
	PostThreadMessage(tray_thread_id, WM_QUIT, 0, 0);
}

void tray::show_web_ui()
{
	// 使用ShellExecute打开默认浏览器
	HINSTANCE result = ShellExecuteA(nullptr, "open", web_ui_url, nullptr, nullptr, SW_SHOWNORMAL);
}

/**
* 在startup文件夹创建快捷方式, 已存在则会直接覆盖
*/
void tray::set_auto_run()
{
	//通过创建快捷方式实现自启动, 避免申请权限
	LPWSTR short_cut_path = new wchar_t[MAX_PATH];
	bool is_exist = false;
	//获取启动文件夹的路径
	if (get_short_cut_path(short_cut_path)) {
		if (SUCCEEDED(CoInitialize(NULL)))
		{
			IShellLink* pShellLink;
			CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
			if (pShellLink)
			{
				//exe文件路径
				char exe_path[MAX_PATH + 1]{ 0 };
				GetModuleFileNameA(NULL, exe_path, MAX_PATH);
				wchar_t exe_path_w[MAX_PATH + 1]{ 0 };
				char_to_LPCESTR(exe_path, exe_path_w);
				//获取exe所在文件夹路径
				wchar_t exe_folder_path[MAX_PATH + 1]{ 0 };
				wcscpy_s(exe_folder_path, exe_path_w);
				wchar_t* p = wcsrchr(exe_folder_path, L'\\');	//找到最后一个'\'
				p[0] = 0;	//截断字符串
				//设置程序路径
				pShellLink->SetPath(exe_path_w);
				//设置工作路径
				pShellLink->SetWorkingDirectory(exe_folder_path);
				IPersistFile* pPersistFile;
				pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
				if (pPersistFile)
				{
					pPersistFile->Save(short_cut_path, FALSE);
					pPersistFile->Release();
				}
				pShellLink->Release();
			}
			CoUninitialize();
		}
	}
	delete[] short_cut_path;
}

void tray::delete_auto_run()
{
	LPWSTR short_cut_path = new wchar_t[MAX_PATH];
	if (get_short_cut_path(short_cut_path)) {
		DeleteFile(short_cut_path);
	}
	delete[] short_cut_path;
}


void char_to_LPCESTR(char* char_string, wchar_t* wide_string) {
	int size = MultiByteToWideChar(CP_UTF8, 0, char_string, -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, char_string, -1, wide_string, size);
}

/**
* 构建自启动快捷方式所在路径
*/
int get_short_cut_path(LPWSTR short_cut_path) {
	//获取启动文件夹的路径
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_STARTUP, NULL, 0, short_cut_path))) {
		const wchar_t* slash = L"\\";
		wcscat_s(short_cut_path, MAX_PATH, slash);
		wcscat_s(short_cut_path, MAX_PATH, app_const::SHORT_CUT_NAME);
		return 1;
	}
	return 0;
}

/**
* 判断文件是否存在
*/
bool file_exists(LPCWSTR file_path) {
	DWORD attributes = GetFileAttributes(file_path);
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

/**
* 判断是否已经设置了开机自启动
*/
bool is_auto_run()
{
	//判断快捷方式是否存在
	LPWSTR short_cut_path = new wchar_t[MAX_PATH];
	bool is_exist = false;
	//获取启动文件夹的路径
	if (get_short_cut_path(short_cut_path)) {
		is_exist = file_exists(short_cut_path);
	}
	delete[] short_cut_path;
	return is_exist;
}