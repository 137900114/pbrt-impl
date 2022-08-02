#include "common/file.h"
#include "common.h"

std::optional<FILE*> OpenFile(const String& path,const String& mode) {
	FILE* f = nullptr;
#ifdef AL_USE_WIDE_STRING
	if (errno_t error = _wfopen_s(&f, path.c_str(), mode.c_str()); error != 0) {
		al_log("::OpenFile : fail to open file {0} , reason {1}", ConvertToNarrowString(path), string(strerror(error)));
		return {};
	}
#else
	FILE* f = fopen(path.c_str(), mode.c_str());
	if (f == nullptr) {
		al_log("::OpenFile : fail to open file {0} , reason {1}", ConvertToNarrowString(path), string(strerror(error)));
		return {};
	}
#endif // AL_USE_WIDE_STRING
	
	return { f };
}

void Close(FILE* file) {
	if (file != nullptr) {
		fclose(file);
	}
}

uint32 GetFileSize(FILE* f) {
	fseek(f, 0, SEEK_END);
	uint32 size = (uint32)ftell(f);
	fseek(f, 0, SEEK_SET);
	return size;
}