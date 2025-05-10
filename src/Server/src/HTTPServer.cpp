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
#include "HTTPParser.h"

namespace http {
	HTTPServer::HTTPServer(const HTTPServerOptions& options, EndpointMap&& endpoints) :
		options(options),
		endpoints(std::move(endpoints)) {
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

	static void sendResponse(const int connection, const Response& response) {
		auto sendResult = net::sendData(connection, response.serialize());
		if (sendResult.failed()) {
			std::cout << sendResult.errMsg;
		}
		close(connection);
	}

	void HTTPServer::start() {
		if (listen(listenSocket, SOMAXCONN) < 0) {
			std::cout << "Error on starting listening from listenSocket: " << strerror(errno) << "\n";
			return;
		}

		bool shutdown = false;
		while (!shutdown) {
			int connection = accept(listenSocket, nullptr, nullptr);
			threadPool.push([&](const int conn) {
				try {
					Response response;
					auto msg = net::recvData(conn);
					if (msg.failed()) {
						std::cout << msg.errMsg;
						response.code = 500;
						return sendResponse(conn, response);
					}
					auto request = ParsedRequest::parse(msg.result);
					if (request.failed()) {
						std::cout << request.errMsg;
						response.code = 400;
						response.body = request.errMsg;
						return sendResponse(conn, response);
					}
					auto it = endpoints.find(request.result.endpoint);
					if (it == endpoints.cend()) {
						std::cout << "Endpoint '" + request.result.endpoint + "' not implemented";
						response.code = 501;
						return sendResponse(conn, response);
					}
					it->second(request.result, response);

					return sendResponse(conn, response);
				}
				catch (std::exception e) {
					Response response;
					response.code = 500;
				}
				
				}, std::move(connection));
		}
	}

}

