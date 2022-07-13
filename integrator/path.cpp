#include "integrator.h"

PathIntegrator& PathIntegrator::SetSamplePerPixel(uint32 spp) {
	return *this;
}

PathIntegrator& PathIntegrator::SetThreadNumber(uint32 nThread) {
	return *this;
}

PathIntegrator& PathIntegrator::SetMaxDepth(uint32 depth) {
	return *this;
}

void PathIntegrator::Render(Scene::Ptr scene) {
}

void PathIntegrator::LogStatus() {
}