#pragma once
#include "common/str.h"
#include <optional>

std::optional<FILE*> OpenFile(const String& path,const String& mode);

void Close(FILE* file);

uint32_t GetFileSize(FILE* f);