#include "rng.h"

RNGSampler::RNGSampler(rtti::Struct* pStruct, uint32 samplePerPixel, uint32 streamSize,uint32 seed)
	:Sampler(pStruct,samplePerPixel,streamSize),generator(seed),distrib(0.f,1.f),seed(seed) {}

bool RNGSampler::NextSample() {
	return Sampler::NextSample();
}

void RNGSampler::NextPixel(uint32 x, uint32 y) {
	Sampler::NextPixel(x, y);
}

Sampler* RNGSampler::Clone() {
	RNGSampler* sampler = CreateSampler <RNGSampler, SEmpty >(samplePerPixel, streamSize, seed);
	CopyFields(sampler, this);
	sampler->distrib = distrib;
	sampler->generator = generator;
	return sampler;
}


float RNGSampler::SampleDimension(uint32 dimension) {
	return distrib(generator);
}