#ifndef HOOKTEST_KEYBDLISTENER_H
#define HOOKTEST_KEYBDLISTENER_H

#include <map>
#include <windows.h>
#include <thread>
#include <iostream>
#include <ctime>
#include <cstring>
#include <fstream>
#include <mutex>
#include <queue>
#include <semaphore>
#include "define.h"

class keyBDData {
public:
	keyBDData() {
		reset();
		press_count = 0;
	}

	std::tm c_time{};
	int press_count = 0;                //按下总次数
	unsigned short press_count_min = 0;            //当前分钟下按下总次数
	std::map<unsigned short, unsigned short> keyBD_count;     //每个按键的按下次数, 键是对应按键的keycode, 值是对应按键的按下次数
	char* binary_buf = nullptr;     //转换为二进制数据时的buffer

	/**
	* 将数据存如buf, 返回数据长度
	* buf内存在to_binary内开辟
	* 使用完需要free
	*/
	int to_binary();

	/**
	 * 保存数据到Hex文件
	 */
	void save_data();

	/**
	 * 加载数据到当前对象
	 * @param data 指向数据开始位置
	 * @return 返回当前读取了几个字节的数据
	 */
	void load_data(std::ifstream&);

	/**
	 * 将所有值设置为初始值, 将时间设置为当前时间
	 */
	void reset();

	/**
	 * 数据有更新时检查是否需要输出数据
	 */
	void updated();
};

struct key_data_frame {
	DWORD flags;			//事件类型
	DWORD vkCode;				//鼠标指针位置数据

	key_data_frame(DWORD f, DWORD code) {
		flags = f;
		vkCode = code;
	}
};

class keyBDListener {
private:
	HHOOK keyboardHook = nullptr;
	DWORD listen_thread_id;
	//监听线程对象
	std::thread listen_thread;
	std::thread data_handler_thread;
	//监听线程是否运行的标志
	bool listen_thread_run_flag = false;
	static keyBDData current_minute_keyBD_data;
	static std::counting_semaphore<10000> data_sem;		//用于标记当前待处理数据数量的信号量
	static std::mutex data_queue_lock;					//数据队列锁
	static std::queue<key_data_frame> data_queue;		//监听线程获取的数据队列

	//按键按下回调函数
	static LRESULT CALLBACK LowLevelKeyboardProc(
		_In_ int nCode,        // 规定钩子如何处理消息，小于 0 则直接 CallNextHookEx
		_In_ WPARAM wParam,    // 消息类型
		_In_ LPARAM lParam    // 指向某个结构体的指针，这里是 KBDLLHOOKSTRUCT（低级键盘输入事件）
	);

	//监听线程函数
	static void listen(keyBDListener*);
	void data_handler();	//处理监听线程获取到的数据

public:
	~keyBDListener();

	void start_listen();

	void stop_listen();

	char* get_data_binary_buffer(int& size);
};



#endif //HOOKTEST_KEYBDLISTENER_H
