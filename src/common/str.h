#pragma once
#include <codecvt>
#include <string>
#include <stdint.h>
#include <vector>
#define AL_USE_WIDE_STRING

#ifdef AL_USE_WIDE_STRING
using String = std::wstring;
#define AL_STR(s) L##s
#define AL_TO_STR(n) std::to_wstring(n)
using Char = wchar_t;
#else
using String = std::string;
#define AL_STR(s) s
#define AL_TO_STR(n) std::to_string(n)
using Char = char;
#endif


String ConvertFromNarrowString(const std::string& str);

String ConvertFromWideString(const std::wstring str);

std::string ConvertToNarrowString(const String& str);

std::wstring ConvertToWideString(const String& str);

std::vector<String> SplitString(const String& str,Char dim);

template<typename T>
struct StringCast {
	static T  FromString(const String& o) {
		if constexpr (std::is_same<T, std::string>::value) {
			return ConvertToNarrowString(o);
		}
		else if constexpr(std::is_same<T, std::wstring>::value) {
			return ConvertToWideString(o);
		}
		else if constexpr (std::is_same<T, uint32_t>::value ||
			std::is_same<T, int>::value) {
			return stoi(o);
		}
		else if constexpr (std::is_same<T, float>::value) {
			return stof(o);
		}
		else if constexpr (std::is_same<T, double>::value) {
			return stod(o);
		}
	}

	static String ToString(const T& o) {
	if constexpr (std::is_same<T, std::string>::value) {
		return ConvertFromNarrowString(o);
	}
	else if constexpr(std::is_same<T, std::wstring>::value) {
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
};