#include "requests.h"

namespace http {
	std::unordered_map<std::string_view, Method> httpMethods {
		{"GET", Method::GET},
		{"PUT", Method::PUT},
		{"POST", Method::POST},
		{"DELETE", Method::DELETE},
		{"PATCH", Method::PATCH},
		{"HEAD", Method::HEAD},
		{"OPTIONS", Method::OPTIONS},
		{"TRACE", Method::TRACE},
	};

	std::unordered_map<std::string_view, Version> httpVersions {
		{"HTTP/1.0", Version::One},
		{"HTTP/1", Version::One},
		{"HTTP/1.1", Version::OneOne},
		{"HTTP/2", Version::Two},
		{"HTTP/2.0", Version::Two},
		{"HTTP/3", Version::Three},
		{"HTTP/3.0", Version::Three},
	};

	std::unordered_map<Version, std::string> httpVersionsToString {
		{Version::One, "HTTP/1.0"},
		{Version::OneOne, "HTTP/1.1"},
		{Version::Two, "HTTP/2"},
		{Version::Three, "HTTP/3"},
	};

	std::unordered_map<int, std::string> codeToReason{
		{ 200, "OK" },
		{ 201, "Created" },
		{ 300, "Multiple Choices"},
		{ 400, "Bad Request" },
		{ 401, "Unauthorized" },
		{ 402, "Payment Required" },
		{ 403, "Forbidden" },
		{ 404, "Not Found" },
		{ 405, "Method Not Allowed" },
		{ 500, "Internal Server Error" },
		{ 501, "Not Implemented" },
		{ 503, "Service Unavailable" },
	};

	static const std::string endLineChar = "\r\n";

	static Result<size_t> parseMethod(const std::string& rawRequest, ParsedRequest& parsedRequest, size_t start) {
		size_t end = rawRequest.find(' ', start);
		std::string_view methodStr{ rawRequest.begin(), rawRequest.begin() + end };
		auto it = httpMethods.find(methodStr);
		if (it == httpMethods.cend()) {
			return Result<size_t>::failure("Unrecognized HTTP method '" + std::string{ methodStr } + "'\n");
		}
		parsedRequest.method = it->second;
		return Result<size_t>::success(std::move(end));
	}

	static size_t parseEndpoint(const std::string& rawRequest, ParsedRequest& parsedRequest, size_t start) {
		size_t urlEnd = rawRequest.find(' ', start);
		std::string url = std::string{ rawRequest.begin() + start, rawRequest.begin() + urlEnd };
		size_t endpointEnd = url.find('?');
		if (endpointEnd == std::string::npos) {
			parsedRequest.endpoint = std::move(url);
			return urlEnd;
		}
		parsedRequest.endpoint = url.substr(0, endpointEnd);

		// Parse url parameters
		size_t keyStart = endpointEnd + 1;
		size_t keyEnd = 0;
		size_t valueEnd = 0;
		do {
			keyEnd = url.find('=', keyStart);
			valueEnd = url.find('&', keyEnd + 1);
			if (keyEnd == std::string::npos) {
				return urlEnd;
			}
			size_t pairEnd = valueEnd;
			if (valueEnd == std::string::npos) {
				pairEnd = url.size();
			}
			parsedRequest.urlParams.emplace(
				url.substr(keyStart, keyEnd - keyStart),
				url.substr(keyEnd + 1, pairEnd - keyEnd - 1)
			);
			keyStart = pairEnd + 1;
		} while (valueEnd != std::string::npos);
		
		return urlEnd;
	}

	static Result<size_t> parseVersion(const std::string& rawRequest, ParsedRequest& parsedRequest, size_t start) {
		size_t end = rawRequest.find(endLineChar, start);
		std::string_view versionStr{ rawRequest.begin() + start, rawRequest.begin() + end };
		auto it = httpVersions.find(versionStr);
		if (it == httpVersions.cend()) {
			return Result<size_t>::failure("Unrecognized HTTP version '" + std::string{ versionStr } + "'\n");
		}
		parsedRequest.version = it->second;
		return Result<size_t>::success(std::move(end));
	}

	static size_t parseHeaders(const std::string& rawRequest, ParsedRequest& parsedRequest, size_t lineStart) {
		size_t lineEnd = rawRequest.find(endLineChar, lineStart);
		while (lineStart != lineEnd) {
			size_t keyEnd = rawRequest.find(':', lineStart);
			parsedRequest.headers.emplace(
				rawRequest.substr(lineStart, keyEnd - lineStart),
				rawRequest.substr(keyEnd + 2, lineEnd - keyEnd - 2)
			);
			lineStart = lineEnd + endLineChar.size();
			lineEnd = rawRequest.find(endLineChar, lineStart);
		}
		return lineEnd;
	}

	Result<ParsedRequest> ParsedRequest::parse(const std::string& rawRequest) {
		ParsedRequest request;
		auto posResult = parseMethod(rawRequest, request, 0);
		if (posResult.failed()) {
			return Result<ParsedRequest>::failure(std::move(posResult.errMsg));
		}
		size_t pos = posResult.result + 1;
		pos = parseEndpoint(rawRequest, request, pos) + 1;
		posResult = parseVersion(rawRequest, request, pos);
		if (posResult.failed()) {
			return Result<ParsedRequest>::failure(std::move(posResult.errMsg));
		}
		pos = posResult.result + endLineChar.size();
		pos = parseHeaders(rawRequest, request, pos) + endLineChar.size();
		request.body = rawRequest.substr(pos, rawRequest.size() - pos);
		return Result<ParsedRequest>::success(std::move(request));
	}

	std::string Response::serialize() const {
		auto it = codeToReason.find(code);
		int statusCode = code;
		if (it == codeToReason.cend()) {
			statusCode = code / 100 * 100;
		}
		std::string rawResponse{ httpVersionsToString[version] + " " + std::to_string(statusCode) + " " + codeToReason[statusCode] + endLineChar};
		for (auto it = headers.cbegin(); it != headers.cend(); it++) {
			rawResponse += it->first + ": " + it->second + endLineChar;
		}
		rawResponse += endLineChar + body;
		return rawResponse;
	}
}