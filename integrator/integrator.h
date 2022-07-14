#pragma once
#include "common/common.h"
#include "scene/Texture.h"
#include "scene/Scene.h"

class Integrator {
public:
	al_add_ptr_t(Integrator);
	//by default the spp is 16
	Integrator(): rtWidth(0),rtHeight(0) {}
	void AttachOutput(Texture::Ptr rt) {
		renderTarget = rt;
		if (rt != nullptr) {
			rtWidth = rt->GetHeight();
			rtHeight = rt->GetWidth();
		}
	}

	virtual void Render(Scene::Ptr scene) = 0;
	virtual void LogStatus() = 0;
protected:
	Texture::Ptr renderTarget;
	uint32 rtWidth, rtHeight;
};


class PathIntegrator : public Integrator {
public:
	al_add_ptr_t(PathIntegrator);
	PathIntegrator() :
		samplePerPixel(16),
		nThread(1),maxDepth(5) {}

	PathIntegrator& SetSamplePerPixel(uint32 spp);
	PathIntegrator& SetThreadNumber(uint32 nThread);
	PathIntegrator& SetMaxDepth(uint32 depth);

	virtual void Render(Scene::Ptr scene) override;
	virtual void LogStatus() override;
private:
	uint32 samplePerPixel;
	uint32 nThread;
	uint32 maxDepth;
};