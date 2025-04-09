#ifndef HOOKTEST_MOUSELISTENER_H
#define HOOKTEST_MOUSELISTENER_H

#include <windows.h>
#include <cmath>
#include <thread>
#include <ctime>
#include <fstream>
#include <iostream>
#include <cstring>
#include <queue>
#include <mutex>
#include <semaphore>

class mouseData {
public:
	mouseData() {
		reset();
	}

	std::tm c_time{};                       //当前记录数据的时间
	unsigned short left_count = 0;			//左键按下次数
	unsigned short right_count = 0;			//右键按下次数
	double move_distance = 0;				//鼠标移动距离, 单位为像素
	unsigned short scroll_count = 0;		//滚轮滚动次数
	unsigned short scroll_press_count = 0;	//滚轮按下次数
	bool xy_available = false;				//鼠标的x, y坐标是否可用, 没记录过坐标时为不可用状态
	int last_x = 0;							//上次移动事件触发时鼠标x坐标
	int last_y = 0;							//y坐标
	char* binary_buf = nullptr;
	const int buffer_size = sizeof(std::time_t) + 4 * sizeof(unsigned short) + sizeof(double);

	int to_binary();

	void save_data();

	void load_data(std::ifstream& in_file);

	void reset();

	/**
	 * 数据有更新时检查是否需要输出数据
	 */
	void updated();
};

struct mouse_data_frame {
	WPARAM wParam;			//事件类型
	POINT pt;				//鼠标指针位置数据

	mouse_data_frame(WPARAM e, POINT p) {
		wParam = e;
		pt = p;
	}
};

class mouseListener {
private:
	HHOOK mouseHook = nullptr;
	DWORD listen_thread_id;
	std::thread listen_thread;
	std::thread data_handler_thread;
	bool listen_thread_run_flag = false;
	static mouseData current_minute_mouse_data;
	static std::counting_semaphore<10000> data_sem;		//用于标记当前待处理数据数量的信号量
	static std::mutex data_queue_lock;					//数据队列锁
	static std::queue<mouse_data_frame> data_queue;			//监听线程获取的数据队列


	static LRESULT CALLBACK LowLevelMouseProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

	static void listen(mouseListener*);
	void data_handler();	//处理监听线程获取到的数据

public:
	~mouseListener();

	void start_listen();

	void stop_listen();

	char* get_data_binary_buffer(int& size);
};



#endif //HOOKTEST_MOUSELISTENER_H
