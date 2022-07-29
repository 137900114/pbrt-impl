#include "path.h"
#include "light/area_light.h"

uint32   PathIntegrator::EstimateSampleCount(Scene::Ptr scene) {
	//TODO estimate how many sample are needed
	return 256;
}

Vector3f PathIntegrator::Li(Ray _r, Scene::Ptr scene,Sampler::Ptr sampler) {
	uint32 depth = 0;
	Vector3f L;
	Ray r = _r;
	Vector3f pathAttenuation = Vector3f(1.f, 1.f, 1.f);

	while (depth < maxDepth) {
		SurfaceIntersection isect;
		if (!scene->Intersect(r, isect)) {
			al_for(i, 0, scene->GetLightSourceCount()) {
				Light::Ptr light = scene->GetLightSource(i);
				if (light->IsInfinity()) {
					L = L + light->InfiniteIntensity(r) * pathAttenuation;
				}
			}
			break;
		}

		//deal with surface emission
		//the following emissive material after the first bounce should not be accounted
		//because it has been calculated while esitimating direct lights
		if (depth == 0) {
			if (auto v = isect.material->GetEmission(); v.has_value()) {
				Light::Ptr emission = v.value();
				L = L + emission->SurfaceEmissionIntensity(isect.isect, r.d) * pathAttenuation;
			}
		}

		//Esitimate direct lighting
		L = L + SampleDirectLightSource(isect, r, sampler) * pathAttenuation;
		
		SamplerStream& stream = sampler->GetSamplerStream();

		//generate next ray randomly
		Material::Ptr mat = isect.material;
		float scatterPdf;
		Vector3f wi = mat->SampleBSDF(isect, stream.Sample2D(), r.d, &scatterPdf);

		//push the new ray's origin point a little further
		Vector3f o = isect.isect.position + isect.isect.normal * Math::eta;
		
		Ray newRay = Ray(o, wi);

		Vector3f f = mat->EvaluateBSDF(r.d, isect, newRay.d);
		pathAttenuation = pathAttenuation * f * std::abs(Math::dot(wi, isect.shadingNormal)) / scatterPdf;

		//stop constructing new path by russia roulette
		float q = std::max(0.01f,1.f - std::max(pathAttenuation.x,std::max(pathAttenuation.y,pathAttenuation.z)));
		if (stream.Sample1D() < q)  break;
		pathAttenuation = pathAttenuation / (1 - q);

		r = newRay;
		depth++;
	}

	return L;
}



void PathIntegrator::LogStatus() {
	//TODO
}
