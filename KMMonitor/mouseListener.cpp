#include "utils.h"
#include "mouseListener.h"

mouseData mouseListener::current_minute_mouse_data;
std::counting_semaphore<10000> mouseListener::data_sem(0);
std::mutex mouseListener::data_queue_lock;
std::queue<mouse_data_frame> mouseListener::data_queue;

mouseListener::~mouseListener() {
	if (listen_thread.joinable()) {
		stop_listen();
		listen_thread.join();
		data_handler_thread.join();
	}
}

void mouseListener::start_listen() {
	if (listen_thread_run_flag) return;
	current_minute_mouse_data.reset();
	listen_thread_run_flag = true;
	listen_thread = std::thread(listen, this);
	data_handler_thread = std::thread(&mouseListener::data_handler, this);
}

void mouseListener::stop_listen() {
	//发送消息, 使监听线程退出消息循环
	if (listen_thread_run_flag)	PostThreadMessage(listen_thread_id, WM_QUIT, 0, 0);
	listen_thread_run_flag = false;
	mouseListener::data_sem.release();
}

char* mouseListener::get_data_binary_buffer(int& size)
{
	size = current_minute_mouse_data.to_binary();
	return current_minute_mouse_data.binary_buf;
}

void mouseListener::listen(mouseListener* listener) {
	listener->listen_thread_id = GetCurrentThreadId();
	listener->mouseHook = SetWindowsHookEx(
		WH_MOUSE_LL,
		LowLevelMouseProc,
		GetModuleHandleA(nullptr),
		NULL
	);
	if (listener->mouseHook == nullptr) {
		listener->listen_thread_run_flag = false;
		return;
	}
	MSG msg;
	//消息循环, 没用, 但得有
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	UnhookWindowsHookEx(listener->mouseHook);
}

void mouseListener::data_handler() {
	while (true) {
		data_sem.acquire();
		if (!listen_thread_run_flag && data_queue.empty()) break;
		data_queue_lock.lock();
		mouse_data_frame temp_data = data_queue.front();
		data_queue.pop();
		data_queue_lock.unlock();
		//开始处理数据
		mouseData* m_data = &current_minute_mouse_data;
		m_data->updated();
		switch (temp_data.wParam) {
		case WM_MOUSEMOVE:
			//鼠标移动事件
			if (m_data->xy_available) {
				int deltaX = temp_data.pt.x - m_data->last_x;
				int deltaY = temp_data.pt.y - m_data->last_y;
				m_data->move_distance += std::sqrt(deltaX * deltaX + deltaY * deltaY);
			}
			else {
				m_data->xy_available = true;
			}
			m_data->last_x = temp_data.pt.x;
			m_data->last_y = temp_data.pt.y;
			break;
		case 0x201:
			//鼠标左键按下事件, 注意和键盘不同, 左键持续按下时只会触发一次该事件
			break;
		case 0x202:
			//鼠标左键抬起事件
			m_data->left_count += 1;
			break;
			//        case 0x204:
			//            //鼠标右键按下事件, 和左键一样, 持续按下只触发一次
			//            break;
		case 0x205:
			//鼠标右键抬起事件
			m_data->right_count += 1;
			break;
			//        case 0x207:
			//            //鼠标中键按下, 同理, 持续按下触发一次
			//            break;
		case 0x208:
			//鼠标中键抬起
			m_data->scroll_press_count += 1;
			break;
		case 0x20a:
			//鼠标滚轮滚动事件
			m_data->scroll_count += 1;
			break;
			//        case 0x20b:
			//            //鼠标侧键按下, 还没触发过侧键事件
			//            break;
			//        case 0x20c:
			//            //鼠标侧键抬起事件
			//            break;
			//        case 0x20e:
			//            //水平滚轮滚动事件, 没触发过...
			//            break;
		default:
			break;
		}
	}
}

/**
 *  鼠标事件处理函数
 *
 * @param nCode     规定钩子如何处理消息，小于 0 则直接 CallNextHookEx
 * @param wParam
 * 事件类型:
 * 鼠标移动 	0x200
 * 鼠标左键按下 	0x201
 * 鼠标左键抬起 	0x202
 * 鼠标右键按下 	0x204
 * 鼠标右键抬起 	0x205
 * 鼠标滚轮滚动 	0x20a
 * 鼠标侧键按下 	0x20b
 * 鼠标侧健抬起 	0x20c
 * 鼠标水平滚轮滚动 	0x20e
 * @return
 */
LRESULT CALLBACK

mouseListener::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	auto* ms = (MSLLHOOKSTRUCT*)lParam;
	/*
	 *  typedef struct tagMSLLHOOKSTRUCT {
	 *      POINT pt;
	 *      DWORD mouseData;    //当事件是滚轮滚动时, 它的高16位记录的是滚动方向及距离. 正值表示远离用户的滚动, 负值表示靠近用户的滚动, 其数值恒定为120, 可以理解为表示一格滚动.
	 *      DWORD flags;
	 *      DWORD c_time;
	 *      ULONG_PTR dwExtraInfo;
	 *  } MSLLHOOKSTRUCT,*LPMSLLHOOKSTRUCT,*PMSLLHOOKSTRUCT;
	 *
	 *  pt指向的结构体:
	 *  typedef struct tagPOINT {
	 *      LONG x;
	 *      LONG y;
	 *  } POINT, *PPOINT;
	 */
	mouse_data_frame temp_data(wParam, ms->pt);
	mouseListener::data_queue_lock.lock();
	mouseListener::data_queue.push(temp_data);
	mouseListener::data_sem.release();
	mouseListener::data_queue_lock.unlock();
	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void mouseData::reset() {
	std::time_t now = std::time(nullptr);
	localtime_s(&c_time, &now);
	c_time.tm_sec = 0;
	left_count = 0;
	right_count = 0;
	move_distance = 0;
	scroll_count = 0;
	scroll_press_count = 0;
}

void mouseData::load_data(std::ifstream& in_file) {
	//FIX:实现有问题, 这样只能读取到最前面的数据
	std::time_t c_timestamp;
	in_file.read(reinterpret_cast<char*>(&c_timestamp), sizeof(time_t));
	localtime_s(&c_time, &c_timestamp);
	in_file.read(reinterpret_cast<char*>(&left_count), sizeof(left_count));
	in_file.read(reinterpret_cast<char*>(&right_count), sizeof(right_count));
	in_file.read(reinterpret_cast<char*>(&move_distance), sizeof(move_distance));
	in_file.read(reinterpret_cast<char*>(&scroll_count), sizeof(scroll_count));
	in_file.read(reinterpret_cast<char*>(&scroll_press_count), sizeof(scroll_press_count));
}

int mouseData::to_binary()
{
	const unsigned short end_flag = 0xffff;
	if (binary_buf == nullptr)
		binary_buf = new char[buffer_size];
	char* write_ptr = binary_buf;
	std::time_t c_timestamp = std::mktime((struct tm*)&c_time);
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&c_timestamp), sizeof(c_timestamp));
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&left_count), sizeof(left_count));
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&right_count), sizeof(right_count));
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&move_distance), sizeof(move_distance));
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&scroll_count), sizeof(scroll_count));
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&scroll_press_count), sizeof(scroll_press_count));
	return buffer_size;
}

void mouseData::save_data() {
	/**
	* 刚启动时如果一分钟内无鼠标时间则可能出现全0
	* 仅在这种情况下出现一次全0
	**/
	if (left_count + right_count + move_distance + scroll_count + scroll_press_count == 0) return;
	std::ofstream out_file("mouseData", std::ios::app | std::ios::binary);
	std::time_t c_timestamp = std::mktime((struct tm*)&c_time);
	int size = to_binary();
	out_file.write(binary_buf, size);
	/*out_file.write(reinterpret_cast<const char*>(&c_timestamp), sizeof(c_timestamp));
	out_file.write(reinterpret_cast<const char*>(&left_count), sizeof(left_count));
	out_file.write(reinterpret_cast<const char*>(&right_count), sizeof(right_count));
	out_file.write(reinterpret_cast<const char*>(&move_distance), sizeof(move_distance));
	out_file.write(reinterpret_cast<const char*>(&scroll_count), sizeof(scroll_count));
	out_file.write(reinterpret_cast<const char*>(&scroll_press_count), sizeof(scroll_press_count));*/
	out_file.close();
}

void mouseData::updated() {
	std::time_t now = std::time(nullptr);
	std::tm current_time{};
	localtime_s(&current_time, &now);
	current_time.tm_sec = 0;
	if (std::mktime((struct tm*)&current_time) != std::mktime((struct tm*)&c_time)) {
		//保存数据
		save_data();
		//重新开始计数
		reset();
	}
}
