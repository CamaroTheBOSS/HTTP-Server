#pragma once
#include "result.h"

namespace net {
	Result<std::string> resolveDnsName(const std::string& dnsName);
	Result<int> bindToIp(const std::string& ip);
	Result<std::string> recvData(const int connection);
	Result<bool> sendData(const int connection, const std::string& msg);
}