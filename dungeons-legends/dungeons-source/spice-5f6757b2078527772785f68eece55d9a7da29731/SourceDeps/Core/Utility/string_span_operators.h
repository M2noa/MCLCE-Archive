#pragma once

inline std::string operator+(string_span lhs, char rhs) {
	return gsl::to_string(lhs) + rhs;
}

inline std::string operator+(std::string&& lhs, char rhs) {
	lhs += rhs;
	return std::move(lhs);
}

inline std::string operator+(std::string&& lhs, string_span rhs) {
	lhs.append(rhs.data(), rhs.size());
	return std::move(lhs);
}

inline std::string operator+(const std::string& lhs, string_span rhs) {
	return std::string(lhs) + rhs;
}

inline std::string operator+(string_span lhs, string_span rhs) {
	return gsl::to_string(lhs) + rhs;
}


