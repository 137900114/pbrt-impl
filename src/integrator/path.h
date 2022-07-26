#pragma once
#include "integrator/sampled_integrator.h"

class PathIntegrator : public SampledIntegrator {
public:
	al_add_ptr_t(PathIntegrator);
	PathIntegrator();
	virtual void LogStatus() override;
protected:

	virtual uint32   EstimateSampleCount(Scene::Ptr scene) override;
	virtual Vector3f Li(Ray r, Scene::Ptr scene,
		Sampler::Ptr sampler) override;
};