#pragma once
#include "sampler/sampler.h"
#include <random>

class RNGSampler : public Sampler {
public:
	RNGSampler(rtti::Struct* pStruct, uint32 samplePerPixel, uint32 streamSize,uint32 seed);

	virtual bool	NextSample() override;
	virtual void	NextPixel(uint32 x, uint32 y) override;

	virtual Sampler* Clone() override;
private:
	virtual float SampleDimension(uint32 dimension) override;

	uniform_real_distribution<float> distrib;
	default_random_engine generator;
	uint32 seed;
};