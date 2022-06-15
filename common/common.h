#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <new>
#include <codecvt>
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


#define al_new(T,...)  new T(__VA_ARGS__)
#define al_new_arr(T,count,...) new T[count](__VA_ARGS__)
#define al_delete(p)   delete p

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


inline String ConvertToString(const std::wstring& s) {
	return s;
}

inline String ConvertToString(const std::string& s) {
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	return wstrResult;
}


inline std::string WideString2String(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

//an output parameter
#define param_out 

#include "optick/optick.h"

//CPU
#define al_profile_frame(name)				OPTICK_FRAME(name)
//avaliable categroys
//None,AI,Animation,Audio,Debug,Camera,Cloth
//GameLogic,Input,Navigation,Network,Physics
//Rendering,Scene,Script,Streaming,UI
//VFX,Visibility,Wait,WaitEmpty
// IO:
// IO
// GPU:
//GPU_Cloth,GPU_Lighting,GPU_PostFX,GPU_Reflections
//GPU_Scene,GPU_Shadows,GPU_UI,GPU_VFX,GPU_Water
#define al_profile_event()					OPTICK_EVENT()
#define al_profile_category(msg,categroy)	OPTICK_CATEGORY(msg,Optick::Category::##categroy)
#define al_profile_thread(name)			    OPTICK_THREAD(name)
#define al_profile_tag(name,...)		    OPTCIK_TAG(name,__VA_ARGS__)


//Automation
#define al_profile_start_capture()			OPTICK_START_CAPTURE()
#define al_profile_stop_capture()			OPTICK_STOP_CAPTURE()
#define al_profile_save_capture(file)       OPTICK_SAVE_CAPTURE(file)