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


std::vector<String> SplitString(const String& str,Char dim) {
	std::vector<String> res;
	size_t offset = 0;
	while (true) {
		auto i = str.find_first_of(dim,offset);
		res.push_back(str.substr(0, i));
		offset = i + 1;
		if (offset >= str.size()) break;
	}
	return std::move(res);
}