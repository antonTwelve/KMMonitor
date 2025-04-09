#pragma once
#include <fstream>
#include "hv/HttpServer.h"

namespace utils
{
	int send_file_GET(HttpRequest* req, HttpResponse* resp, char* file_path);
	void mem_cpy_move(char** dst, void const* src, size_t size);
};

