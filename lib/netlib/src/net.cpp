#include "net.h"

#include <string>

#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

namespace net {
	Result<std::string> resolveDnsName(const std::string& dnsName) {
		addrinfo address;
		memset(&address, 0, sizeof address);
		address.ai_family = AF_UNSPEC;
		address.ai_socktype = SOCK_STREAM;

		addrinfo* results;
		if (int err = getaddrinfo(dnsName.c_str(), nullptr, &address, &results); err) {
			return Result<std::string>::failure("Unresolved DNS name '" + dnsName + "'. " + gai_strerror(err) + "\n");
		}

		std::string ipStr;
		for (addrinfo* addr = results; addr != nullptr; addr = addr->ai_next) {
			if (addr->ai_family == AF_INET) { // IPv4
				char ipChars[INET_ADDRSTRLEN];
				sockaddr_in* ipv4 = (sockaddr_in*)(addr->ai_addr);
				inet_ntop(AF_INET, &ipv4->sin_addr, ipChars, INET_ADDRSTRLEN);
				ipStr = std::string{ ipChars };
				break;
			}
			else { // IPv6
				char ipChars[INET6_ADDRSTRLEN];
				sockaddr_in6* ipv6 = (sockaddr_in6*)(addr->ai_addr);
				inet_ntop(AF_INET6, &ipv6->sin6_addr, ipChars, INET_ADDRSTRLEN);
				ipStr = std::string{ ipChars };
				break;
			}
		}
		freeaddrinfo(results);
		return Result<std::string>::success(std::move(ipStr));
	}

	Result<int> bindToIp(const std::string& ip) {
		int listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listenSocket < 0) {
			return Result<int>::failure("Error when acquiring listening socket. " + std::string{ strerror(errno) } + "\n");
		}

		int mode = 1;
		if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &mode, sizeof(int)) == -1) {
			return Result<int>::failure("Error when setting socket options to avoid 'Address already in use' error. " + std::string{ strerror(errno) } + "\n");
		}

		sockaddr_in listenSocketAddress = { 0 };
		listenSocketAddress.sin_family = AF_INET;
		listenSocketAddress.sin_port = htons(8080);
		inet_pton(AF_INET, ip.c_str(), &listenSocketAddress.sin_addr.s_addr);
		if (bind(listenSocket, reinterpret_cast<sockaddr*>(&listenSocketAddress), sizeof(listenSocketAddress)) < 0) {
			return Result<int>::failure("Error when binding listening socket to IP address: " + ip + ". " + std::string{ strerror(errno) } + "\n");
		}
		return Result<int>::success(std::move(listenSocket));
	}

	Result<std::string> recvData(const int connection) {
		char buffer[4096];
		int bytesRead = recv(connection, buffer, 4096, 0);
		if (bytesRead > 0) {
			return Result<std::string>::success(std::string(buffer, bytesRead));
		}
		else if (bytesRead == 0) {
			return Result<std::string>::failure("Client closed connection.\n");
		}
		return Result<std::string>::failure("Error occured when receiving data from the client: " + std::string{ strerror(errno) } + "\n");
	}

	Result<bool> sendData(const int connection, const std::string& msg) {
		int bytesSent = send(connection, msg.c_str(), msg.size(), 0);
		if (bytesSent > 0) {
			return Result<bool>::success(true);
		}
		else if (bytesSent == 0) {
			return Result<bool>::failure("Client closed connection.\n");
		}
		return Result<bool>::failure("Error occured when sending data to the client: " + std::string{ strerror(errno) } + "\n");
	}

	
}