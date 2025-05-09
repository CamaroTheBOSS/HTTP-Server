#pragma once
#include <string>
#include "thread_pool.h"

struct HTTPServerOptions {
	std::string address{ "" };
	int port{ 8080 };
};

class HTTPServer {
public:
	HTTPServer(const HTTPServerOptions& options);
	void start();
private:
	void init();

	HTTPServerOptions options;
	int listenSocket = -1;
	ThreadPool threadPool;
};