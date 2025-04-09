#include "hv/HttpServer.h"
#include "hv/WebSocketServer.h"
#include "hv/EventLoop.h"
#include "hv/htime.h"
#include "appconsts.h"
#include "keyBDListener.h"
#include "mouseListener.h"
#include "utils.h"
#include "tray.h"
#include "define.h"


#ifdef HIDE_CMD_WINDOW
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")//不显示窗口
#endif

unsigned short get_port();
int is_port_idle(unsigned short);

int main() {
#ifdef START_KM_LISTEN
	keyBDListener k_listener;
	k_listener.start_listen();
	mouseListener m_listener;
	m_listener.start_listen();
#endif

	////网页服务部分	
	HttpService router;

#ifndef VUE_DEBUG
	router.Static("/", "./res");
	//router.preprocessor = [](HttpRequest* req, HttpResponse* resp) {
	//	// 检查客户端 IP，允许仅本地访问
	//	if (req->host != app_const::ALLOWED_HOST) {
	//		return HTTP_STATUS_FORBIDDEN;
	//	}
	//	return HTTP_STATUS_OK;
	//	};
#else
	//测试代码, 转接请求到vue
	router.Proxy("/test/", "http://localhost:5173/");
	router.Proxy("/@vite/", "http://localhost:5173/@vite/");
	router.Proxy("/src/", "http://localhost:5173/src/");
	router.Proxy("/node_modules/", "http://localhost:5173/node_modules/");
	router.Proxy("/@id/", "http://localhost:5173/@id/");
#endif

	router.GET("/allData/mouseData",
		[](HttpRequest* req, HttpResponse* resp) {
			////限制访问IP为localhost, 限定本机访问, 禁止外部访问
			//if (req->host != app_const::ALLOWED_HOST)
			//	return 0;
			return utils::send_file_GET(req, resp, (char*)"mouseData");
		}
	);

	router.GET("/allData/keyBDData",
		[](HttpRequest* req, HttpResponse* resp) {
			//if (req->host != app_const::ALLOWED_HOST)
			//	return 0;
			return utils::send_file_GET(req, resp, (char*)"keyBDData");
		}
	);

#ifdef START_KM_LISTEN
	//websocket两次发送之间的时间间隔
	const int WS_INTERVAL_MS = 500;
	WebSocketService ws;
	ws.onopen = [&m_listener, &k_listener](const WebSocketChannelPtr& channel, const HttpRequestPtr& req) {
		//if (req->host != app_const::ALLOWED_HOST) {
		//	channel->close();
		//	return;
		//}
		//printf("onopen: GET %s\n", req->host.c_str());
		//printf("onopen: GET %s\n", req->Path().c_str());
		if (strcmp("/ws/mdata", req->Path().c_str()) == 0) {
			hv::setInterval(WS_INTERVAL_MS, [channel, &m_listener](hv::TimerID id) {
				if (channel->isConnected()) {
					int send_size = 0;
					char* send_buf = m_listener.get_data_binary_buffer(send_size);
					channel->send(send_buf, send_size, WS_OPCODE_BINARY);
				}
				else {
					hv::killTimer(id);
				}
				});
		}
		else if (strcmp("/ws/kdata", req->Path().c_str()) == 0) {
			hv::setInterval(WS_INTERVAL_MS, [channel, &k_listener](hv::TimerID id) {
				if (channel->isConnected()) {
					int send_size = 0;
					char* send_buf = k_listener.get_data_binary_buffer(send_size);
					channel->send(send_buf, send_size, WS_OPCODE_BINARY);
				}
				else {
					hv::killTimer(id);
				}
				});
		}
		else {
			//未知路径, 直接关闭
#ifndef VUE_DEBUG
			channel->close();
#endif
		}
		};
	ws.onmessage = [](const WebSocketChannelPtr& channel, const std::string& msg) {
		//接收到消息
		};
	ws.onclose = [](const WebSocketChannelPtr& channel) {
		//websocket关闭
#ifdef DEBUG_VERSION
		printf("close\n");
#endif
		};
#endif

	http_server_t server;
	strcpy(server.host, "localhost");
#ifdef RELEASE_VERSION
	server.port = get_port();
#elif defined(DEBUG_VERSION)
	//测试端口固定
	server.port = 8079;
#endif
	//没有可用端口, 退出
	if (!server.port) return 0;
	server.service = &router;

#ifdef START_KM_LISTEN
	server.ws = &ws;
#endif

	//启动系统托盘
	tray* tray_object = new tray;	//需要开在堆上
	tray_object->start(&server, (int(*)(void*))http_server_stop, server.port);

	//启动网页服务
	http_server_run(&server);

#ifdef START_KM_LISTEN
	k_listener.stop_listen();
	m_listener.stop_listen();
#endif
	return 0;
}

/**
* 判断一个端口是否可用, 返回0为可用, 否则不可用
*/
int is_port_idle(unsigned short port)
{
	WSADATA wsData;
	WSAStartup(MAKEWORD(2, 2), &wsData);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	int ret = ::bind(s, (LPSOCKADDR)&addr, sizeof(addr));
	closesocket(s);
	::WSACleanup();
	return ret;
}

/**
* 返回一个可用的端口
*/
unsigned short get_port()
{
	srand(time(NULL));
	unsigned short port = 10000 + rand() % 30000;;
	//最多尝试100个端口
	for (int i = 0; i < 100; i++)
	{
		if (!is_port_idle(port))return port;
		port += 1;
	}
	//没有找到可用的端口, 返回0
	return 0;
}