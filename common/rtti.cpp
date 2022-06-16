#include "rtti.h"

namespace rtti {

	static vector<VariableInfo>* structInfo;
	static uint64* structSize;

	void InitializeStructVariable(const char* name, const char* type, uint64 size) {
		al_assert(structSize != nullptr && structInfo != nullptr, "rtti::InitializeStructVariable"
			": a rtti::Variable must be a member of rtti::Struct");
		VariableInfo info{};
		info.name = name;
		info.typeName = type;
		info.size = size;
		info.offset = *structSize;
		
		structInfo->push_back(info);
		*structSize += size;
	}


	const VariableInfo* Struct::GetVariable(const char* name) const {
		al_for(i,0,info.size()) {
			if (!strcmp(name,info[i].name)) {
				return &info[i];
			}
		}
		return nullptr;
	}

	const VariableInfo* Struct::GetVariable(uint32 index) const {
		al_assert(index >= info.size(), "rtti::Struct::GetVariable : index out of bondary");
		return &info[index];
	}

	Struct::Struct(const char* type): size(0) {
		this->type = type;
		structSize = &size;
		structInfo = &info;
	}

	void* VariableInfo::Get(void* ptr) const {
		al_assert(ptr != nullptr, "VariableInfo::Get : null pointer");
		float* res = ((float*)ptr) + offset;
		return res;
	}

	bool VariableIsFloatType(const VariableInfo* info) {
		return strstr(info->typeName, "float") != nullptr;
	}

	bool VariableIsIntType(const VariableInfo* info) {
		return strstr(info->typeName, "int") != nullptr;
	}
}