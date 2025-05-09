#include "HTTPServer.h"

#include <string>
#include <iostream>
#include <variant>

#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "net.h"
#include "errno.h"
#include "thread_safe_queue.h"


HTTPServer::HTTPServer(const HTTPServerOptions& options) :
	options(options) {
	init();
}

void HTTPServer::init() {
	auto ipResult = net::resolveDnsName(options.address);
	if (ipResult.failed()) {
		std::cout << ipResult.errMsg;
		return;
	}
	std::cout << "Resolved IP address for DNS name: " << options.address << " - " << ipResult.result << "\n";

	auto boundSocketResult = net::bindToIp(ipResult.result);
	if (boundSocketResult.failed()) {
		std::cout << boundSocketResult.errMsg;
		return;
	}
	std::cout << "Bound IP address to socket: " << boundSocketResult.result << "\n";
	listenSocket = boundSocketResult.result;
}

void HTTPServer::start() {
	if (listen(listenSocket, SOMAXCONN) < 0) {
		std::cout << "Error on starting listening from listenSocket: " << strerror(errno) << "\n";
		return;
	}

	bool shutdown = false;
	while (!shutdown) {
		int connection = accept(listenSocket, nullptr, nullptr);
		threadPool.push([](const int conn) {
			auto msg = net::recvData(conn);
			if (msg.succeded()) {
				std::cout << msg.result << conn << "\n";
			}
			else {
				std::cout << msg.errMsg;
			}

			auto sendResult = net::sendData(conn, "HTTP/1.1 200 OK\n\n{}");
			if (sendResult.succeded()) {
				std::cout << "Response sent successufuly\n";
			}
			else {
				std::cout << msg.errMsg;
			}
			close(conn);
			return;
			}, std::move(connection));
	}
}
