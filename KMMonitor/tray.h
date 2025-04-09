#pragma once
#include "appconsts.h"
#include "resource.h"
#include <windows.h>
#include <ShlObj.h>
//#include "hv/HttpServer.h";
//此处无法include HttpServer.h, hv/HttpServer.h与windows.h同时include会发生报错
#include <thread>
class tray
{
public:
	void start(void*, int(*server_stop)(void*), int server_port);

private:
	static const int URL_MAX_LEN = 256;
	static char* web_ui_url;
	static void* http_server;
	static int (*http_server_stop)(void*);
	static DWORD tray_thread_id;
	static HMENU hMenu;
	static const UINT ITEM_ID_SHOW = 12;
	static const UINT ITEM_ID_SETSTART = 13;
	static const UINT ITEM_ID_CLEARSTART = 14;
	static const UINT ITEM_ID_QUIT = 15;
	std::thread t_thread;

	static LRESULT CALLBACK tray_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void tray_thread();
	static void quit();
	static void show_web_ui();
	static void set_auto_run();
	static void delete_auto_run();
};


void char_to_LPCESTR(char*, wchar_t*);
int get_short_cut_path(LPWSTR);
bool file_exists(LPCWSTR);
bool is_auto_run();