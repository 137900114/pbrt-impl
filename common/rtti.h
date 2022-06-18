#pragma once
#include "common/common.h"
#include "math/math.h"

namespace rtti {

	template<typename T>
	struct TypeTrait {
		static constexpr const char* name = "unknown";
		static constexpr uint64		 size = sizeof(T) / 4;
	};

	template<>
	struct TypeTrait<float> {
		static constexpr const char* name = "float";
		static constexpr uint64		 size = 1;
	};

	template<>
	struct TypeTrait<int64> {
		static constexpr const char* name = "int64";
		static constexpr uint64		 size = 2;
	};

	template<>
	struct TypeTrait<int32> {
		static constexpr const char* name = "int32";
		static constexpr uint64		 size = 1;
	};

	template<>
	struct TypeTrait<uint64> {
		static constexpr const char* name = "uint64";
		static constexpr uint64		 size = 2;
	};

	template<>
	struct TypeTrait<uint32> {
		static constexpr const char* name = "uint32";
		static constexpr uint64		 size = 1;
	};


	template<>
	struct TypeTrait<Vector2f> {
		static constexpr const char* name = "float2";
		static constexpr uint64		 size = 2;
	};

	template<>
	struct TypeTrait<Vector3f> {
		static constexpr const char* name = "float3";
		static constexpr uint64		 size = 3;
	};

	template<>
	struct TypeTrait<Vector4f> {
		static constexpr const char* name = "float4";
		static constexpr uint64		 size = 4;
	};

	struct VariableInfo {
		const char* name;
		const char* typeName;
		uint64 size;
		uint64 offset;

		void* Get(void* ptr) const;
	};

	bool VariableIsFloatType(const VariableInfo* info);

	bool VariableIsIntType(const VariableInfo* info);

	struct Struct {
		Struct(const char* type);

		uint64 GetSize() const { return size; }
		const VariableInfo* GetVariable(uint32 index) const;
		const VariableInfo* GetVariable(const char* name) const;

		const vector<VariableInfo>& GetVariableInfo() { return info; }

		const char* GetTypeName() { return type; }
	private:
		vector<VariableInfo> info;
		uint64 size;
		const char* type;
	};

	//this function should not be called externally
	void InitializeStructVariable(const char* name,const char* type,uint64 size);

	template<typename T>
	struct Variable {
		Variable(const char* name) {
			InitializeStructVariable(name, TypeTrait<T>::name, TypeTrait<T>::size);
		}
	};	
}


#define RTTI_STRUCT(S) struct S : public rtti::Struct{\
	S():rtti::Struct(#S){}

#define RTTI_STRUCT_END()};