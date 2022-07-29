#pragma once
#include "sampler.h"

//TODO need to be fixed in the future
class HaltonSampler : public Sampler {
public:
	HaltonSampler(rtti::Struct* pStruct, uint32 samplePerPixel,uint32 streamSize);

	virtual bool	NextSample() override;
	virtual void	NextPixel(uint32 x, uint32 y) override;

	virtual Sampler* Clone() override;
protected:
	virtual float SampleDimension(uint32 dimension) override;
	
	uint32 GetInternalIndex(uint32 sampleIndex);

	uint32 internalIndex;
};