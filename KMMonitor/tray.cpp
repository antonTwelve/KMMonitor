#include "tray.h"

/*
* ����
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
	// ��Ҫ�޸�TaskbarCreated������ϵͳ�������Զ������Ϣ
	WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
	switch (message)
	{
	case WM_CREATE://���ڴ���ʱ�����Ϣ.
		nid.cbSize = sizeof(nid);
		nid.hWnd = hwnd;
		nid.uID = 0;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_USER;
		nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		lstrcpy(nid.szTip, app_const::APP_NAME);
		Shell_NotifyIcon(NIM_ADD, &nid);
		hMenu = CreatePopupMenu();//���ɲ˵�
		//���ѡ��
		AppendMenu(hMenu, MF_STRING, ITEM_ID_SHOW, TEXT("��ʾ����"));
		if (is_auto_run())
			AppendMenu(hMenu, MF_STRING, ITEM_ID_CLEARSTART, TEXT("�������������"));
		else
			AppendMenu(hMenu, MF_STRING, ITEM_ID_SETSTART, TEXT("����������"));
		AppendMenu(hMenu, MF_STRING, ITEM_ID_QUIT, TEXT("�˳�"));
		Shell_NotifyIcon(NIM_ADD, &nid);
		break;
	case WM_USER:
		if (lParam == WM_LBUTTONDOWN) {
			//�����¼�
			show_web_ui();
		}
		if (lParam == WM_RBUTTONDOWN)
		{
			GetCursorPos(&pt);//ȡ�������
			::SetForegroundWindow(hwnd);//����ڲ˵��ⵥ������˵�����ʧ������
			//EnableMenuItem(hMenu, ITEM_ID_SHOW, MF_GRAYED);//�ò˵��е�ĳһ����
			item_id = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hwnd, NULL);//��ʾ�˵�����ȡѡ��ID
			switch (item_id)
			{
			case ITEM_ID_SHOW:
				show_web_ui();
				break;
			case ITEM_ID_SETSTART:
				set_auto_run();
				ModifyMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ITEM_ID_CLEARSTART, TEXT("�������������"));
				break;
			case ITEM_ID_CLEARSTART:
				delete_auto_run();
				ModifyMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ITEM_ID_SETSTART, TEXT("����������"));
				break;
			case ITEM_ID_QUIT:
				quit();
				break;
			default:
				break;
			}
		}
		break;
	case WM_DESTROY://��������ʱ�����Ϣ.
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;
	default:
		/*
		*   ��ֹ��Explorer.exe �����Ժ󣬳�����ϵͳϵͳ�����е�ͼ�����ʧ
			ԭ��Explorer.exe �����������ؽ�ϵͳ����������ϵͳ������������ʱ�����ϵͳ��
			����ע�����TaskbarCreated ��Ϣ�Ķ������ڷ���һ����Ϣ������ֻ��Ҫ��׽�����
			Ϣ�����ؽ�ϵͳ���̵�ͼ�꼴�ɡ�
		*/
		if (message == WM_TASKBARCREATED)
			SendMessage(hwnd, WM_CREATE, wParam, lParam);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void tray::start(void* server, int(*server_stop)(void*), int server_port)
{
	//����server�����http_server_stopָ�뺯��
	http_server = server;
	http_server_stop = server_stop;
	//���ݶ˿ں�����URL
	char port_n[10];
	sprintf_s(port_n, 10, "%d", server_port);
	if (web_ui_url) delete[] web_ui_url;
	web_ui_url = new char[URL_MAX_LEN];
	strcpy_s(web_ui_url, URL_MAX_LEN, "http://localhost:");
	strcat_s(web_ui_url, URL_MAX_LEN, port_n);
	strcat_s(web_ui_url, URL_MAX_LEN, "/");
	//����tray�߳�
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

	// ����WS_EX_TOOLWINDOW, ��ʾΪsystray
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

	//tmd, һ������������Ϣѭ��......
	//��Ϣѭ��
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
	// ʹ��ShellExecute��Ĭ�������
	HINSTANCE result = ShellExecuteA(nullptr, "open", web_ui_url, nullptr, nullptr, SW_SHOWNORMAL);
}

/**
* ��startup�ļ��д�����ݷ�ʽ, �Ѵ������ֱ�Ӹ���
*/
void tray::set_auto_run()
{
	//ͨ��������ݷ�ʽʵ��������, ��������Ȩ��
	LPWSTR short_cut_path = new wchar_t[MAX_PATH];
	bool is_exist = false;
	//��ȡ�����ļ��е�·��
	if (get_short_cut_path(short_cut_path)) {
		if (SUCCEEDED(CoInitialize(NULL)))
		{
			IShellLink* pShellLink;
			CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
			if (pShellLink)
			{
				//exe�ļ�·��
				char exe_path[MAX_PATH + 1]{ 0 };
				GetModuleFileNameA(NULL, exe_path, MAX_PATH);
				wchar_t exe_path_w[MAX_PATH + 1]{ 0 };
				char_to_LPCESTR(exe_path, exe_path_w);
				//��ȡexe�����ļ���·��
				wchar_t exe_folder_path[MAX_PATH + 1]{ 0 };
				wcscpy_s(exe_folder_path, exe_path_w);
				wchar_t* p = wcsrchr(exe_folder_path, L'\\');	//�ҵ����һ��'\'
				p[0] = 0;	//�ض��ַ���
				//���ó���·��
				pShellLink->SetPath(exe_path_w);
				//���ù���·��
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
* ������������ݷ�ʽ����·��
*/
int get_short_cut_path(LPWSTR short_cut_path) {
	//��ȡ�����ļ��е�·��
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_STARTUP, NULL, 0, short_cut_path))) {
		const wchar_t* slash = L"\\";
		wcscat_s(short_cut_path, MAX_PATH, slash);
		wcscat_s(short_cut_path, MAX_PATH, app_const::SHORT_CUT_NAME);
		return 1;
	}
	return 0;
}

/**
* �ж��ļ��Ƿ����
*/
bool file_exists(LPCWSTR file_path) {
	DWORD attributes = GetFileAttributes(file_path);
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

/**
* �ж��Ƿ��Ѿ������˿���������
*/
bool is_auto_run()
{
	//�жϿ�ݷ�ʽ�Ƿ����
	LPWSTR short_cut_path = new wchar_t[MAX_PATH];
	bool is_exist = false;
	//��ȡ�����ļ��е�·��
	if (get_short_cut_path(short_cut_path)) {
		is_exist = file_exists(short_cut_path);
	}
	delete[] short_cut_path;
	return is_exist;
}