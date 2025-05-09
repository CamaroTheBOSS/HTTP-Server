#pragma once
#include <string>
#include <sstream>


template <typename T>
struct Result {
	static Result<T> success(T&& result) {
		return Result{ std::forward<T>(result), "" };
	}
	static Result<T> failure(std::string&& errMsg) {
		return Result{ T{}, std::move(errMsg) };
	}


	bool succeded() {
		return errMsg.empty();
	}
	bool failed() {
		return !succeded();
	}

	T result;
	std::string errMsg;
};