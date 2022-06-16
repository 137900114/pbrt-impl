#include "sampler.h"


Sampler::Sampler(rtti::Struct* target,uint32 samplePerPixel):
	pStruct(target),samplePerPixel(samplePerPixel),currentSampleIndex(-1){
	al_for(i,0,target->GetVariableInfo().size()) {
		const rtti::VariableInfo* info = &target->GetVariableInfo()[i];
		al_assert(rtti::VariableIsFloatType(info), 
			"Sampler::Sampler : only float type are supported for sampling");
	}
	sampledData.resize(pStruct->GetSize());
}

Sampler::~Sampler() {
	al_delete(pStruct);
}

float* Sampler::GetSample(const char* name) {
	uint32 index = GetSampleIndex(name);
	return GetSample(index);
}

float* Sampler::GetSample(uint32 index) {
	if (index >= pStruct->GetVariableInfo().size())
		return nullptr;
	const rtti::VariableInfo* info = &pStruct->GetVariableInfo()[index];
	return (float*)info->Get(sampledData.data());
}

uint32  Sampler::GetSampleIndex(const char* name) {
	auto& variables = pStruct->GetVariableInfo();
	al_for(i,0,variables.size()) {
		if (!strcmp(variables[i].name,name)) {
			return i;
		}
	}
	return infinity_i;
}

bool Sampler::NextSample() {
	currentSampleIndex++;
	if (currentSampleIndex >= samplePerPixel - 1)
		return false;

	al_for(i,0,pStruct->GetSize()) {
		sampledData[i] = SampleDimension(currentSampleIndex, i);
	}

	return true;
}

void Sampler::NextPixel() {
	currentSampleIndex = 0;
}