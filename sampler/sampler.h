#pragma once
#include "common/common.h"
#include "common/rtti.h"
#include <optional>

class SamplerStream {
	friend class Sampler;
public:
	SamplerStream(uint32 size);
	
	float    Sample1D() ;
	Vector2f Sample2D() ;
	Vector3f Sample3D() ;
	Vector4f Sample4D() ;

private:
	void ResetStream(float* stream);

	float* stream;
	uint32 index,size;
};


class Sampler {
public:
	al_add_ptr_t(Sampler);
	Sampler(rtti::Struct* pStruct,uint32 samplePerPixel,uint32 streamSize);

	const float*	GetSample(const char* name);
	const float*	GetSample(uint32 index);

	std::optional<uint32>  GetSampleIndex(const char* name);

	SamplerStream& GetSamplerStream();
	
	virtual bool	NextSample();
	virtual void	NextPixel(uint32 x,uint32 y);

	virtual Sampler* Clone() = 0;

	virtual ~Sampler() { pStruct = nullptr; }
protected:
	virtual float SampleDimension(uint32 dimension) = 0;
	//helper function for impelementing clone in children
	void CopyFields(Sampler* dst,Sampler* src);

	uint32 pixelX, pixelY;
	uint32 samplePerPixel;
	int32  currentSampleIndex;
	std::shared_ptr<rtti::Struct> pStruct;
	vector<float> sampledData;

	uint32 streamSize;
	SamplerStream stream;
};

template<typename SamplerT,typename StructT,typename ...Args>
SamplerT* CreateSampler(Args... args) {
	static_assert(std::is_base_of_v<rtti::Struct, StructT>, "CreateSampler : the struct type must derived from rtti::Struct");
	static_assert(std::is_base_of_v<Sampler, SamplerT>, "CreateSampler : the sampler type must derived from Sampler");
	return al_new(SamplerT, al_new(StructT), args...);
}