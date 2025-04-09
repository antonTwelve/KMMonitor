#include "utils.h"

int utils::send_file_GET(HttpRequest* req, HttpResponse* resp, char* file_path) {
	std::ifstream file_in(file_path, std::ios::binary);
	if (!file_in.is_open())
		return 200;
	//获取文件大小
	file_in.seekg(0, std::ios::end);
	std::streampos file_size = file_in.tellg();
	file_in.seekg(0, std::ios::beg);
	//开缓冲区
	char* file_buffer = new char[file_size];
	//读取
	file_in.read(file_buffer, file_size);
	int resp_ret = resp->Data(file_buffer, file_size, false);

	file_in.close();
	delete[] file_buffer;
	return resp_ret;
}

void utils::mem_cpy_move(char** dst, void const* src, size_t size)
{
	memcpy(*dst, src, size);
	(*dst) += size;
}
