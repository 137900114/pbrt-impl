#include "integrator.h"


void Integrator::AttachCamera(Camera::Ptr camera) {
	if (camera != nullptr) {
		this->camera = camera;
	}
	rtWidth = camera->GetWidth();
	rtHeight = camera ->GetHeight();
}

void Integrator::AttachScene(Scene::Ptr   scene) {
	this->scene = scene;
}

Integrator::~Integrator() {
	camera = nullptr;
	scene = nullptr;
}
