#include "static.h"
#include "requests.h"

#include <fstream>

namespace html {
	static constexpr const char* errorPage = R"(
		<!DOCTYPE html>
		<html>
			<body>
				<h1>500 Internal Server Error</h1>
			</body>
		</html>
)";

	std::string readHtml(const std::filesystem::path& path) {
		std::ofstream file(path, std::ios::in);
		if (!file) {
			return errorPage;
		}
		std::stringstream ss;
		ss << file.rdbuf();
		return ss.str();
	}

	bool readHtmlIfExists(const std::filesystem::path& path, http::Response& response) {
		std::ofstream file(path, std::ios::in);
		if (!file) {
			response.code = 404;
			return false;
		}
		std::stringstream ss;
		ss << file.rdbuf();
		response.body = ss.str();
		if (path.string().ends_with(".js")) {
			response.headers.emplace("Content-Type", "text/javascript; charset=utf-8");
		}
		else if (path.string().ends_with(".css")) {
			response.headers.emplace("Content-Type", "text/css");
		}
		else {
			response.headers.emplace("Content-Type", "text/html");
		}
		return true;
	}
}