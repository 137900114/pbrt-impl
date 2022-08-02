#pragma once
#include "common/common.h"
#include <map>
//command line arguments parser

class ParamParser {
public:
	al_add_ptr_t(ParamParser);

	ParamParser(int argc,const char** argvs);

	template<typename T>
	void Set(const String& key, const T& value) {
		if constexpr (std::is_same_v<T, string>) {
			return InternalSet(key, ConvertFromNarrowString(value));
		}
		else if constexpr (std::is_same_v<T, wstring>) {
			return InternalSet(key, ConvertFromWideString(value));
		}
		else {
			return InternalSet(key, StringCast<T>::ToString(value));
		}
	}

	template<typename T>
	optional<T> Get(const String& key) {
		if (auto res = InternalGet(key);res.has_value()) {
			try {
				return { StringCast<T>::FromString(res.value()) };
			}
			catch (...) {
				al_warn("fail to convert {} from string", ConvertToNarrowString(key));
				return {};
			}
		}
		else {
			al_warn("fail to find key {}", ConvertToNarrowString(key));
			return {};
		}
		
	}

	bool Dump(const String& path);
	void Load(const String& path);
private:
	void InternalSet(const String& key,const String& value);
	optional<String> InternalGet(const String& key);

	map<String, String> table;
};