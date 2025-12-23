#pragma once

namespace SNIRFCPP {
	namespace Utils {
		inline bool EndsWith(const std::string& str, const std::string& suffix) {
			if (str.length() < suffix.length()) {
				return false;
			}
			return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
		}
	}
}