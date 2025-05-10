#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include "thread_pool.h"
#include "requests.h"

namespace http {
	using EndpointMap = std::unordered_map<std::string, std::function<void(const ParsedRequest&, Response&)>>;

	struct HTTPServerOptions {
		std::string address{ "" };
		int port{ 8080 };
	};

	class HTTPServer {
	public:
		HTTPServer(const HTTPServerOptions& options, EndpointMap&& endpoints = {});
		void start();
	private:
		void init();

		HTTPServerOptions options;
		int listenSocket = -1;
		ThreadPool threadPool;
		const EndpointMap endpoints;
	};
}

