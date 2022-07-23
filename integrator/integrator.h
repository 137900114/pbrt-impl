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
