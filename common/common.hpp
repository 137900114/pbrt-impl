#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <new>

using namespace std;

#include <string.h>
#include <stdint.h>

using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8  = uint8_t ;

using int64  = int64_t;
using int32  = int32_t;
using int16  = int16_t;
using int8   = int8_t ;

#define zero_memory(p) memset( (p) , 0 , sizeof( *(p) ) )
#define zero_memory_s(p,size) memset( (p) , 0 , (size) )
#define copy_memory(dst,src) memcpy( (dst) , (src) ,sizeof( *(dst) ) )
#define copy_memory_s(dst,src,size) memcpy( (dst) , (src) , (size) )

#define al_for(i,s,n) for(uint32 i = s;i < n;i++)

#define al_malloc(size) malloc((size))
#define al_mfree(p)   free((p))

template<typename T,typename ...Args>
T* al_new_impl(Args... args) {
	void* tmp = al_malloc(sizeof(T));
	if (tmp) {
		new(tmp) T(args...);
	}
	return (T*)tmp;
}

template<typename T,typename ...Args>
T* al_new_arr_impl(uint32 count,Args... args) {
	T* tmp = (T*)al_malloc(sizeof(T) * count);
	al_for(i,0,count){
		new(tmp + i) T(args...);
	}
	return tmp;
}

template<typename T>
void al_delete_impl(T* p) {
	p->~T();
	al_mfree(p);
}


#define al_new(T,...)  al_new_impl<T>(__VA_ARGS__)
#define al_new_arr(T,count,...) al_new_arr_impl<T>(count,__VA_ARGS__)
#define al_delete(p)   al_delete_impl(p)

#define al_countof(arr) (sizeof(arr) / sizeof(*arr))

#ifdef DEBUG
#define al_debug_break __debugbreak()
#else
#define al_debug_break 
#endif


template<typename T>
using ptr = shared_ptr<T>;

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

inline ptr<spdlog::logger> _al_logger;

#define al_log_initialize() _al_logger = spdlog::stdout_color_mt("Alex");\
							_al_logger->set_level(spdlog::level::trace);\
							_al_logger->set_pattern("%^[%T] %n: %v%$");

#define al_log_finalize()   _al_logger = nullptr;

#define al_log(...) _al_logger->info(__VA_ARGS__)
#define al_assert(expr,...) if(!(expr)) {_al_logger->warn(__VA_ARGS__);al_debug_break;exit(-1);}


using String = wstring;
#define al_string(s) L##s


String ConvertToString(const std::wstring& s) {
	return s;
}

String ConvertToString(const std::string& s) {
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	return wstrResult;
}

//an output parameter
#define param_out 