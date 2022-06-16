#pragma once
#include "sampler.h"

class HaltonSampler : public Sampler {
public:
	HaltonSampler(rtti::Struct* pStruct, uint32 samplePerPixel) :
	 Sampler(pStruct,samplePerPixel){}
protected:
	virtual float SampleDimension(uint32 sampleIndex, uint32 dimension) override;
};