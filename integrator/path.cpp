#include "integrator.h"

PathIntegrator& PathIntegrator::SetSamplePerPixel(uint32 spp) {
	if (spp == 0) {
		al_warn("PathIntegrator::SetSamplePerPixel : invalid spp, spp should not be 0");
	}
	else {
		samplePerPixel = spp;
	}

	return *this;
}

PathIntegrator& PathIntegrator::SetThreadNumber(uint32 nThread) {
	if (nThread == 0) {
		al_warn("PathIntegrator::SetThreadNumber : invalid thread num, thread num should not be 0");
	}
	else {
		this->nThread = nThread;
	}
	return *this;
}

PathIntegrator& PathIntegrator::SetMaxDepth(uint32 depth) {
	if (depth == 0) {
		al_warn("PathIntegrator::SetMaxDepth : invalid depth num, depth num should be greater than 0");
	}
	else {
		maxDepth = depth;
	}
	return *this;
}

void PathIntegrator::Render(Scene::Ptr scene) {


}

void PathIntegrator::LogStatus() {
	al_log(	"==========================================\n"
			"path integrator status :\n"
			"------------------------------------------\n"
			"render target width :{} render target height: {}\n"
			"sample per pixel    :{}\n"
			"number of thread    :{}\n"
			"max depth           :{}\n"
			"==========================================",
		rtWidth,rtHeight,samplePerPixel,
		nThread,maxDepth);
}