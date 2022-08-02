#include "parser.h"
#include <filesystem>
#include <sstream>

bool ParamParser::Dump(const String& path) {
	FILE* f;
	if (auto v = OpenFile(path,AL_STR("r"));v.has_value()) {
		f = v.value();
	}
	else {
		al_log("ParamParser::Dump : fail to open file {}", ConvertToNarrowString(path));
		return false;
	}
	String content;

	for (auto p : table) {
		content += p.first + AL_STR("=") + p.second + AL_STR("\n");
	}
	//convert string to utf-8 encoding
	string utf8Content = ConvertToNarrowString(path);
	fwrite(utf8Content.c_str(), utf8Content.size(), 1, f);
	return true;
}

void ParamParser::Load(const String& path) {
	FILE* f;
	if (auto v = OpenFile(path, AL_STR("r")); v.has_value()) {
		f = v.value();
	}
	else {
		al_log("ParamParser::Load : fail to open file {}", ConvertToNarrowString(path));
		return;
	}
	uint32 fsize = GetFileSize(f);
	char* buffer = (char*)malloc(fsize);
	fread(buffer, fsize, 1, f);
	String content = ConvertFromNarrowString(buffer);
#ifdef AL_USE_WIDE_STRING
	wstringstream ss(content);
#else
	stringstream ss(content);
#endif
	String line;
	while (getline(ss,line)) {
		uint32 eq = line.find_first_of(AL_STR("="));
		String key = line.substr(0, eq);
		String value = line.substr(eq + 1, line.size());
		InternalSet(key, value);
	}

	free(buffer);
}

void ParamParser::InternalSet(const String& key, const String& value) {
	table[key] = value;
}

optional<String> ParamParser::InternalGet(const String& key) {
	if (auto res = table.find(key);res != table.end()) {
		return { res->second };
	}
	return {};
}