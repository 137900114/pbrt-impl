#pragma once
#include "integrator/integrator.h"

class DirectLightIntegrator : public SampledIntegrator {
public:
	al_add_ptr_t(DirectLightIntegrator);
	DirectLightIntegrator();

	virtual void LogStatus() override;
protected:

	virtual uint32   EstimateSampleCount(Scene::Ptr scene) override;
	virtual Vector3f Li(Ray r, Scene::Ptr scene,
		Sampler::Ptr sampler) override;
};