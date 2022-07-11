#include "sampler.h"


Sampler::Sampler(rtti::Struct* target,uint32 extraSample,uint32 samplePerPixel):
	pStruct(target),samplePerPixel(samplePerPixel),currentSampleIndex(0),extraSample(extraSample){
	al_for(i,0,target->GetVariableInfo().size()) {
		const rtti::VariableInfo* info = &target->GetVariableInfo()[i];
		al_assert(rtti::VariableIsFloatType(info),
			"Sampler::Sampler : only float types are supported for sampling"
			".However the {}th parameter '{}' in struct '{}' is '{}' ",
			i + 1, info->name, target->GetTypeName(), info->typeName);
	}
	sampledData.resize(pStruct->GetSize() + extraSample);
}

Sampler::~Sampler() {
	al_delete(pStruct);
}

const float* Sampler::GetSample(const char* name) {
	auto index = GetSampleIndex(name);
	if (!index.has_value())
		return nullptr;
	return GetSample(index.value());
}

const float* Sampler::GetSample(uint32 index) {
	if (index >= pStruct->GetVariableInfo().size())
		return nullptr;
	const rtti::VariableInfo* info = &pStruct->GetVariableInfo()[index];
	return (float*)info->Get(sampledData.data());
}

std::optional<float>  Sampler::ExtraSample(uint32 index) {
	if (index >= extraSample) {
		return {};
	}
	return sampledData[pStruct->GetSize() + index];
}

std::optional<uint32>   Sampler::GetSampleIndex(const char* name) {
	auto& variables = pStruct->GetVariableInfo();
	al_for(i,0,variables.size()) {
		if (!strcmp(variables[i].name,name)) {
			return i;
		}
	}
	return {};
}

bool Sampler::NextSample() {
	currentSampleIndex++;
	if (currentSampleIndex > samplePerPixel)
		return false;

	al_for(i,0,pStruct->GetSize() + extraSample) {
		sampledData[i] = SampleDimension(i);
	}

	return true;
}

void Sampler::NextPixel(uint32 x,uint32 y) {
	currentSampleIndex = 0;
	pixelX = x, pixelY = y;
}