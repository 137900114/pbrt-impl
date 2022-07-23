#include "integrator.h"


void Integrator::AttachCamera(Camera::Ptr camera) {
	if (camera != nullptr) {
		this->camera = camera;
	}
	rtWidth = camera->GetFilm()->GetWidth();
	rtHeight = camera->GetFilm()->GetHeight();
}

void Integrator::AttachScene(Scene::Ptr   scene) {
	this->scene = scene;
}

Integrator::~Integrator() {
	camera = nullptr;
	scene = nullptr;
}
