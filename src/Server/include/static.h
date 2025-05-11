#pragma once
#include <string>
#include <filesystem>
#include "requests.h"

namespace html {
	std::string readHtml(const std::filesystem::path& path);
	bool readHtmlIfExists(const std::filesystem::path& path, http::Response& response);
}