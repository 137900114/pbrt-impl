#pragma once
#include "common/common.h"
#include "scene/texture.h"
#include "scene/scene.h"
#include "camera/camera.h"
#include "sampler/sampler.h"

class Integrator {
public:
	al_add_ptr_t(Integrator);
	//by default the spp is 16
	Integrator(): rtWidth(0),rtHeight(0) {}
	void AttachCamera(Camera::Ptr camera);
	void AttachScene(Scene::Ptr   scene);

	virtual void Render() = 0;
	virtual void LogStatus() = 0;

	virtual ~Integrator();
protected:
	Camera::Ptr camera;
	Scene::Ptr  scene;
	uint32 rtWidth, rtHeight;
};

enum LIGHT_SAMPLE_STRATEGY {
	LIGHT_SAMPLE_STRATEGY_ALL,
	LIGHT_SAMPLE_STRATEGY_ONE
};

class SampledIntegrator : public Integrator {
public:
	al_add_ptr_t(SampledIntegrator);
	SampledIntegrator() :
		samplePerPixel(16),
		nThread(1),maxDepth(5),
		lightSampleStrategy(LIGHT_SAMPLE_STRATEGY_ONE) 
	{}

	void SetSamplePerPixel(uint32 spp);
	void SetThreadNumber(uint32 nThread);
	void SetMaxDepth(uint32 depth);
	void SetLightSampleStrategy(LIGHT_SAMPLE_STRATEGY strategy);

	virtual void Render() override;
protected:
	virtual Vector3f Li(Ray r,Scene::Ptr scene,
		Sampler::Ptr sampler) = 0;
	virtual uint32   EstimateSampleCount(Scene::Ptr scene) = 0;


	uint32 samplePerPixel;
	uint32 nThread;
	uint32 maxDepth;
	LIGHT_SAMPLE_STRATEGY lightSampleStrategy;
};