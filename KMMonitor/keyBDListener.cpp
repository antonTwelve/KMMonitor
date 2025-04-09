#include "utils.h"			//不要把utils.h移到keyBDListener.h里, utils.h中有hv/HttpServer.h, keyBDListener.h里有windows.h, 同时include报错
#include "keyBDListener.h"

keyBDData keyBDListener::current_minute_keyBD_data;
std::counting_semaphore<10000> keyBDListener::data_sem(0);
std::mutex keyBDListener::data_queue_lock;
std::queue<key_data_frame> keyBDListener::data_queue;

/**
 * 开始监听键盘
 */
void keyBDListener::start_listen() {
	if (listen_thread_run_flag) return;  //已在运行
	listen_thread_run_flag = true;
	listen_thread = std::thread(listen, this);    //启动监听线程
	data_handler_thread = std::thread(&keyBDListener::data_handler, this);
}

void keyBDListener::listen(keyBDListener* listener) {
	listener->listen_thread_id = GetCurrentThreadId();
	listener->keyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,            // 钩子类型，WH_KEYBOARD_LL 为键盘钩子
		LowLevelKeyboardProc,    // 指向钩子函数的指针
		GetModuleHandleA(nullptr),    // Dll 句柄
		NULL
	);
	if (listener->keyboardHook == nullptr) {    //Hook失败
		listener->listen_thread_run_flag = false;
		return;
	}
	MSG msg;
	//消息循环
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	UnhookWindowsHookEx(listener->keyboardHook);
}

void keyBDListener::data_handler() {
	//循环, 直到运行flag为false并且数据队列为空
	while (true) {
		data_sem.acquire();
		if (!listen_thread_run_flag && data_queue.empty()) break;
		data_queue_lock.lock();
		key_data_frame temp_data = data_queue.front();
		data_queue.pop();
		data_queue_lock.unlock();
		//开始处理数据
		keyBDData* k_data = &current_minute_keyBD_data;
		// 检查当前时间是否记录的时间(在同一分钟内), 判断是否保存数据并重新开始计数
		k_data->updated();
		if (temp_data.flags == 128 || temp_data.flags == 129) {
			// 监控键盘
			// count()与find()类似, 返回0或1表示是否存在
			if (k_data->keyBD_count.count(temp_data.vkCode)) {
				k_data->keyBD_count[temp_data.vkCode] += 1;
			}
			else {
				k_data->keyBD_count[temp_data.vkCode] = 1;
			}
			k_data->press_count += 1;
			k_data->press_count_min += 1;
		}
	}
}

void keyBDListener::stop_listen() {
	if (listen_thread_run_flag) PostThreadMessage(listen_thread_id, WM_QUIT, 0, 0);
	listen_thread_run_flag = false;
	keyBDListener::data_sem.release();	//释放信号, 激活数据处理线程, 使之退出
}

char* keyBDListener::get_data_binary_buffer(int& size)
{
	size = current_minute_keyBD_data.to_binary();
	return current_minute_keyBD_data.binary_buf;
}

/**
 * 按键按下callback
 *
 * lParam结构体定义:
 *  typedef struct tagKBDLLHOOKSTRUCT {
 *      DWORD     vkCode;		// 按键代号
 *      DWORD     scanCode;		// 硬件扫描代号，同 vkCode 也可以作为按键的代号。
 *      DWORD     flags;		// 事件类型，一般按键按下为 0 抬起为 128。
 *      DWORD     c_time;			// 消息时间戳
 *      ULONG_PTR dwExtraInfo;	// 消息附加信息，一般为 0。
 *  }KBDLLHOOKSTRUCT,*LPKBDLLHOOKSTRUCT,*PKBDLLHOOKSTRUCT;
 *
 * @param nCode     规定钩子如何处理消息，小于 0 则直接 CallNextHookEx
 * @param wParam    消息类型 256: 按键按下   257: 按键抬起
 * @param lParam    指向某个结构体的指针，这里是 KBDLLHOOKSTRUCT（低级键盘输入事件）
 * @return
 */
LRESULT CALLBACK keyBDListener::LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	auto* ks = (KBDLLHOOKSTRUCT*)lParam;        // 包含低级键盘输入事件信息
	key_data_frame temp_data(ks->flags, ks->vkCode);
	keyBDListener::data_queue_lock.lock();
	keyBDListener::data_queue.push(temp_data);
	keyBDListener::data_sem.release();
	keyBDListener::data_queue_lock.unlock();
#ifdef SHOWVKCODE
	if (ks->flags == 128 || ks->flags == 129) {
		//DEBUG, 显示当前按键的vkCode
		std::cout << "ckCode:" << ks->vkCode << std::endl;
	}
#endif
	//return 1;		// 使按键失效
	// 将消息传递给钩子链中的下一个钩子
	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

keyBDListener::~keyBDListener() {
	if (listen_thread.joinable()) {
		stop_listen();
		listen_thread.join();
	}
}

void keyBDData::load_data(std::ifstream& in_file) {
	std::time_t c_timestamp;
	in_file.read(reinterpret_cast<char*>(&c_timestamp), sizeof(time_t));
	localtime_s(&c_time, &c_timestamp);

	in_file.read(reinterpret_cast<char*>(&press_count_min), sizeof(press_count_min));
	//读取按键按下次数
	keyBD_count.clear();
	unsigned short key = 0, value = 0;
	while (true) {
		in_file.read(reinterpret_cast<char*>(&key), sizeof(key));
		if (key == 0xffff) break;
		in_file.read(reinterpret_cast<char*>(&value), sizeof(value));
		keyBD_count[key] = value;
	}
}

int keyBDData::to_binary()
{
	const unsigned short end_flag = 0xffff;
	//计算bufer大小
	int buffer_size = sizeof(std::time_t) + 2 * sizeof(unsigned short) + (keyBD_count.size() * 2 * sizeof(unsigned short));
	if (binary_buf != nullptr) delete[] binary_buf;
	binary_buf = new char[buffer_size];
	char* write_ptr = binary_buf;
	std::time_t c_timestamp = std::mktime((struct tm*)&c_time);
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&c_timestamp), sizeof(c_timestamp));
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&press_count_min), sizeof(press_count_min));
	for (const auto& pair : keyBD_count) {
		utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
		utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
	}
	utils::mem_cpy_move(&write_ptr, reinterpret_cast<const char*>(&end_flag), sizeof(end_flag));
	return buffer_size;
}

void keyBDData::save_data() {
	// 该分钟内没有需要保存的数据, 只会在刚启动时出现一次无数据情况
	if (press_count_min <= 0) return;
	std::ofstream out_file("keyBDData", std::ios::app | std::ios::binary);
	int size = to_binary();
	out_file.write(binary_buf, size);
	/*std::time_t c_timestamp = std::mktime((struct tm*)&c_time);
	out_file.write(reinterpret_cast<const char*>(&c_timestamp), sizeof(c_timestamp));
	out_file.write(reinterpret_cast<const char*>(&press_count_min), sizeof(press_count_min));
	for (const auto& pair : keyBD_count) {
		out_file.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
		out_file.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
	}
	unsigned short end = 0xffff;
	out_file.write(reinterpret_cast<const char*>(&end), sizeof(end));*/
	out_file.close();
}

void keyBDData::reset() {
	std::time_t now = std::time(nullptr);
	localtime_s(&c_time, &now);     //拷贝出时间
	c_time.tm_sec = 0;
	press_count_min = 0;
	keyBD_count.clear();
}

void keyBDData::updated() {
	std::time_t now = std::time(nullptr);
	std::tm current_time{};
	localtime_s(&current_time, &now);
	current_time.tm_sec = 0;
	if (std::mktime((struct tm*)&current_time) != std::mktime((struct tm*)&c_time)) {
		save_data();
		reset();
	}
}
