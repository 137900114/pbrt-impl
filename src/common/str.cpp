#include "common/str.h"


static std::wstring NarrowStringToWideString(const std::string& s) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(s);
}

static std::string WideStringToNarrowString(const std::wstring& str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(str);
}


#ifdef AL_USE_WIDE_STRING
//String is wide string

String ConvertFromNarrowString(const std::string& str) {
	return NarrowStringToWideString(str);
}

String ConvertFromWideString(const std::wstring str) {
	return str;
}

std::string ConvertToNarrowString(const String& str) {
	return WideStringToNarrowString(str);
}

std::wstring ConvertToWideString(const String& str) {
	return str;
}
#else
String ConvertFromNarrowString(const std::string& str) {
	return str;
}

String ConvertFromWideString(const std::wstring str) {
	return WideStringToNarrowString(str);
}

std::string ConvertToNarrowString(const String& str) {
	return str;
}

std::wstring ConvertToWideString(const String& str) {
	return NarrowStringToWideString(str);
}
#endif

template<typename T>
String ToString(const T& o) {
	if constexpr (std::is_same_v<T, std::string>) {
		return ConvertFromNarrowString(o);
	}
	else constexpr(std::is_same_v<T, std::wstring>) {
		return ConvertFromWideString(o);
	}
	else {
#ifdef AL_USE_WIDE_STRING
	return to_wstring(o);
#else
	return to_string(o);
#endif
	}
}

template<typename T>
T  FromString(const String& o) {
	if constexpr (std::is_same_v<T, std::string>) {
		return ConvertToNarrowString(o);
	}
	else constexpr(std::is_same_v<T, std::wstring>) {
		return ConvertToWideString(o);
	}
	else if constexpr (std::is_same_v<T, uint32_t> ||
	std::is_same_v<T, int>) {
	return stoi(o);
	}
	else if constexpr (std::is_same_v<T, float>) {
	return stof(o);
	}
	else if constexpr (std::is_same_v<T, double>) {
	return stod(o);
	}
}