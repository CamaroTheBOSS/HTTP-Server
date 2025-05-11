// Server.cpp: definiuje punkt wejścia dla aplikacji.
//

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "HTTPServer.h"
#include "static.h"

int main()
{
	const std::string staticRoot = "../../../../../static";
	http::EndpointMap endpoints;
	http::HTTPServerOptions options{ "172.28.239.52", 8080, staticRoot };
	http::HTTPServer server{ options, std::move(endpoints)};
	server.start();
}
