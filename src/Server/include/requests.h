#pragma once
#include <unordered_map>
#include <string>

#include "result.h"

namespace http {
	enum class Method { GET, PUT, POST, DELETE, PATCH, HEAD, OPTIONS, TRACE };
	enum class Version { One, OneOne, Two, Three };
	class ParsedRequest {
	public:
		static Result<ParsedRequest> parse(const std::string& rawRequest);

		Version version;
		Method method;
		std::string endpoint;
		std::unordered_map<std::string, std::string> headers;
		std::unordered_map<std::string, std::string> urlParams;
		std::string body;
	};

	class Response {
	public:
		std::string serialize() const;

		Version version = Version::OneOne;
		int code;
		std::unordered_map<std::string, std::string> headers;
		std::string body;
	};
}


