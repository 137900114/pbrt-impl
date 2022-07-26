#include "sampler.h"


Sampler::Sampler(rtti::Struct* target,uint32 samplePerPixel,uint32 streamSize):
	pStruct(target),samplePerPixel(samplePerPixel),currentSampleIndex(0),
	stream(streamSize),pixelX(0),pixelY(0),streamSize(streamSize)
{
	al_for(i,0,target->GetVariableInfo().size()) {
		const rtti::VariableInfo* info = &target->GetVariableInfo()[i];
		al_assert(rtti::VariableIsFloatType(info),
			"Sampler::Sampler : only float types are supported for sampling"
			".However the {}th parameter '{}' in struct '{}' is '{}' ",
			i + 1, info->name, target->GetTypeName(), info->typeName);
	}
	sampledData.resize(pStruct->GetSize() + streamSize);
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


std::optional<uint32>   Sampler::GetSampleIndex(const char* name) {
	auto& variables = pStruct->GetVariableInfo();
	al_for(i,0,variables.size()) {
		if (!strcmp(variables[i].name,name)) {
			return i;
		}
	}
	return {};
}

SamplerStream& Sampler::GetSamplerStream() {
	al_assert(currentSampleIndex != 0, "Sampler::GetSamplerStream : fail to get stream.Call Sampler::NextSample() to "
		"initialize the sampler");
	return stream;
}

bool Sampler::NextSample() {
	currentSampleIndex++;
	if (currentSampleIndex > samplePerPixel)
		return false;

	al_for(i,0,pStruct->GetSize() + stream.size) {
		sampledData[i] = SampleDimension(i);
	}

	stream.ResetStream(sampledData.data() + pStruct->GetSize());

	return true;
}

void Sampler::NextPixel(uint32 x,uint32 y) {
	currentSampleIndex = 0;
	pixelX = x, pixelY = y;
}

SamplerStream::SamplerStream(uint32 size):
stream(nullptr),size(size),index(0) {}

float SamplerStream::Sample1D()  {
	al_assert(index < size, "SamplerStream::Sample1D() : sample out of bondary");
	float rv = stream[index];
	index++;
	return rv;
}

Vector2f SamplerStream::Sample2D() {
	al_assert(index < size - 1, "SampleStream::Sample2D : sample out of bondary");
	Vector2f rv(&stream[index]);
	index += 2;
	return rv;
}
Vector3f SamplerStream::Sample3D()  {
	al_assert(index < size - 2, "SampleStream::Sample2D : sample out of bondary");
	Vector3f rv(&stream[index]);
	index += 3;
	return rv;
}

Vector4f SamplerStream::Sample4D()  {
	al_assert(index < size - 4, "SampleStream::Sample2D : sample out of bondary");
	Vector4f rv(&stream[index]);
	index += 4;
	return rv;
}

void SamplerStream::ResetStream(float* stream) {
	index = 0;
	this->stream = stream;
}

void Sampler::CopyFields(Sampler* dst, Sampler* src) {
	dst->currentSampleIndex = src->currentSampleIndex;
	dst->pixelX = src->pixelX, dst->pixelY = src->pixelY;
	dst->pStruct = src->pStruct;
	dst->sampledData = src->sampledData;
	dst->samplePerPixel = src->samplePerPixel;
	al_assert(dst->stream.size == src->stream.size, "Sampler::CopyField : the dst and src sampler must have the same sampler stream size");
	dst->stream.ResetStream(dst->sampledData.data() + pStruct->GetSize());
	//so dst and src will start from the same position
	dst->stream.index = src->stream.index;
}