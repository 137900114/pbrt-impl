#pragma once
#include "common/common.h"
#include <map>
//command line arguments parser

class ParamParser;

struct ParameterTable {
	const char* key;
	const char* discribtion;
	const char* default_value;
	function<void(ParamParser*,ParameterTable*,uint32)> callback;
	bool  should_be_dumped;
	bool  contains_value;
	uint32 name_count;
	const char* names[4];
};


class ParamParser {
public:
	al_add_ptr_t(ParamParser);

	ParamParser(int argc,const char** argvs,uint32 tableCount,ParameterTable* table);

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
			//al_warn("fail to find key {}", ConvertToNarrowString(key));
			return {};
		}
		
	}

	template<typename T>
	T Require(const String& key) {
		auto v = this->Get<T>(key);
		al_assert(v.has_value(), "this parameter must have a value");
		return v.value();
	}

	bool Dump(const String& path);
	void Load(const String& path);
private:
	void InternalSet(const String& key,const String& value);
	optional<String> InternalGet(const String& key);

	map<String, String> table;
	vector<String> dumpList;
};