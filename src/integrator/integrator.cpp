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

void Integrator::DebugBreakAtPixel(uint32 x, uint32 y) {
#ifdef DEBUG
	bx.push_back(x), by.push_back(y);
#endif
}


void Integrator::CheckDebugBreak(uint32 x,uint32 y) {
#ifdef DEBUG
	al_for(i,0,bx.size()) {
		if (bx[i] == x && by[i] == y) {
			al_debug_break;
		}
	}
#endif
}

void NormalIntegrator::Render() {
	//disable tone mapping
	camera->SetToneMappingAlgorithm(ClampToneMapping);
	al_for(x,0,rtHeight) {
		al_for(y,0,rtWidth) {
			CheckDebugBreak(x, y);

			Ray r = camera->GenerateRay(0, Vector2f(.5f,.5f), Vector2f(), x, y);
			Vector3f L;
			SurfaceIntersection isect;
			if (scene->Intersect(r,isect)) {
				if (shadingNormal)
					L = isect.shadingNormal * .5f + .5f;
				else if (showTangent)
					L = isect.isect.tangent;
				else
					L = isect.isect.normal * .5f + .5f;

			}
			camera->WriteFilm(L, x, y);
		}
	}
}

void NormalIntegrator::ShowShadingNormal(bool v) {
	if (!v) {
		shadingNormal = false, showTangent = false;
	}
	else {
		shadingNormal = true, showTangent = false;
	}
}

void NormalIntegrator::ShowTangent(bool v) {
	if (!v) {
		shadingNormal = false, showTangent = false;
	}
	else {
		showTangent = true; shadingNormal = false;
	}
}

void NormalIntegrator::LogStatus() {

}

