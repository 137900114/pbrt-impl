#include <codecvt>
#include <string>
#include <stdint.h>
#define AL_USE_WIDE_STRING

#ifdef AL_USE_WIDE_STRING
using String = std::wstring;
#define AL_STR(s) L##s
#define AL_TO_STR(n) std::to_wstring(n)
#else
using String = std::string;
#define AL_STR(s) s
#define AL_TO_STR(n) std::to_string(n)
#endif

String ConvertFromNarrowString(const std::string& str);

String ConvertFromWideString(const std::wstring str);

std::string ConvertToNarrowString(const String& str);

std::wstring ConvertToWideString(const String& str);

template<typename T>
String ToString(const T& o);

template<typename T>
T  FromString(const String& o);