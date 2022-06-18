#pragma once
#include "common/common.h"
#include "common/rtti.h"


class Sampler {
public:
	Sampler(rtti::Struct* pStruct,uint32 samplePerPixel);

	~Sampler();

	const float*	GetSample(const char* name);
	const float*	GetSample(uint32 index);

	uint32  GetSampleIndex(const char* name);
	
	virtual bool	NextSample();
	virtual void	NextPixel(uint32 x,uint32 y);
protected:
	virtual float SampleDimension(uint32 dimension) = 0;

	uint32 pixelX, pixelY;
	uint32 samplePerPixel;
	int32  currentSampleIndex;
	rtti::Struct* pStruct;
	vector<float> sampledData;
};

template<typename SamplerT,typename StructT,typename ...Args>
SamplerT* CreateSampler(Args... args) {
	static_assert(std::is_base_of_v<rtti::Struct, StructT>, "CreateSampler : the struct type must derived from rtti::Struct");
	static_assert(std::is_base_of_v<Sampler, SamplerT>, "CreateSampler : the sampler type must derived from Sampler");
	return al_new(SamplerT, al_new(StructT), args...);
}