#pragma once
#include "integrator/integrator.h"

enum LIGHT_SAMPLE_STRATEGY {
	LIGHT_SAMPLE_STRATEGY_ALL,
	LIGHT_SAMPLE_STRATEGY_ONE
};

enum DIRECT_LIGHT_ESITIMATE_STRATEGY {
	DIRECT_LIGHT_ESITIMATE_STRATEGY_MIS,
	DIRECT_LIGHT_ESITIMATE_STRATEGY_LIGHT,
	DIRECT_LIGHT_ESITIMATE_STRATEGY_BSDF
};

class SampledIntegrator : public Integrator {
public:
	al_add_ptr_t(SampledIntegrator);
	SampledIntegrator() :
		samplePerPixel(16),
		nThread(1), maxDepth(5),
		lightSampleStrategy(LIGHT_SAMPLE_STRATEGY_ONE),
		esitimateStrategy(DIRECT_LIGHT_ESITIMATE_STRATEGY_MIS)
	{}

	void SetSamplePerPixel(uint32 spp);
	void SetThreadNumber(uint32 nThread);
	void SetMaxDepth(uint32 depth);
	void SetLightSampleStrategy(LIGHT_SAMPLE_STRATEGY strategy);
	void SetDirectLightEsitimateStrategy(DIRECT_LIGHT_ESITIMATE_STRATEGY strategy);

	virtual void Render() override;
protected:
	virtual Vector3f Li(Ray r, Scene::Ptr scene,
		Sampler::Ptr sampler) = 0;
	virtual uint32   EstimateSampleCount(Scene::Ptr scene) = 0;

	Vector3f EsitimateDirectLight(const SurfaceIntersection& isect,const Ray& r, Light::Ptr light, Sampler::Ptr sampler);
	Vector3f SampleDirectLightSource(const SurfaceIntersection& isect,const Ray& r,
		Sampler::Ptr sampler);


	uint32 samplePerPixel;
	uint32 nThread;
	uint32 maxDepth;
	LIGHT_SAMPLE_STRATEGY lightSampleStrategy;
	DIRECT_LIGHT_ESITIMATE_STRATEGY esitimateStrategy;
};