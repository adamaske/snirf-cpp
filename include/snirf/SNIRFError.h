#pragma once
#include <string>

namespace SNIRFCPP {
	struct SNIRFError {
		enum Code {
			NONE = 0,
			FILE_NOT_FOUND,
			INVALID_FORMAT,
			READ_ERROR,
			WRITE_ERROR,
			UNKNOWN_ERROR
		};

		SNIRFError(Code code = NONE, std::string message = "")
			: code(code), message(message) {
		}

		Code code;
		std::string message;
	};
}