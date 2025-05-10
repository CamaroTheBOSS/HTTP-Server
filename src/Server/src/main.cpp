// Server.cpp: definiuje punkt wejścia dla aplikacji.
//

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "HTTPServer.h"

int main()
{
	http::EndpointMap endpoints;
	endpoints.emplace("/",
		[](const http::ParsedRequest& request, http::Response& response) {
			response.code = 200;
			response.body = R"(
				<!DOCTYPE html>
				<html>
					<body>
						<h1>My First Heading</h1>
						<p>My first paragraph.</p>
					</body>
				</html>
			)";
		}
	);

	http::HTTPServerOptions options{ "172.28.239.52", 8080 };
	http::HTTPServer server{ options, std::move(endpoints)};
	server.start();
}
