#include "common/common.h"

AL_PRIVATE std::wstring NarrowStringToWideString(const std::string& s) {
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	return wstrResult;
}

AL_PRIVATE std::string WideStringToNarrowString(const std::wstring& str) {
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
	return 
}
#endif