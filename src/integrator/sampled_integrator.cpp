#include "sampled_integrator.h"
#include "multithread/concurrent_queue.h"
#include "multithread/dispatcher.h"
#include "sampler/rng.h"

void SampledIntegrator::SetSamplePerPixel(uint32 spp) {
	if (spp == 0) {
		al_warn("SampledIntegrator::SetSamplePerPixel : invalid spp, spp should not be 0");
	}
	else {
		samplePerPixel = spp;
	}
}

void SampledIntegrator::SetThreadNumber(uint32 nThread) {
	if (nThread == 0) {
		al_warn("SampledIntegrator::SetThreadNumber : invalid thread num, thread num should not be 0");
	}
	else {
		this->nThread = nThread;
	}
}

void SampledIntegrator::SetMaxDepth(uint32 depth) {
	if (depth == 0) {
		al_warn("SampledIntegrator::SetMaxDepth : invalid depth num, depth num should be greater than 0");
	}
	else {
		maxDepth = depth;
	}
}


void SampledIntegrator::Render() {
	al_assert(camera != nullptr, "SampledIntegrator::Render : a camera should be attached to integrator before rendering");
	al_assert(scene != nullptr, "SampledIntegrator::Render : a scene  should be attached to integrator before rendering");

	//most x86 cpu's cache line size is 64 bytes
	constexpr uint32 tileSize = 16;
	uint32 xTileCount = (rtWidth + tileSize - 1) / tileSize;
	uint32 yTileCount = (rtHeight + tileSize - 1) / tileSize;

	struct Tile {
		uint32 width, height;
		uint32 offsetX, offsetY;
	};
	Dispatcher::Ptr dispatcher(new Dispatcher(nThread));
	ConcurrentQueue<Tile> taskQueue;

	scene->Build();
	al_for(y, 0, yTileCount) {
		al_for(x, 0, xTileCount) {
			Tile tile;
			tile.offsetX = x * tileSize;
			tile.offsetY = y * tileSize;
			tile.width = min(rtWidth - tile.offsetX, tileSize);
			tile.height = min(rtHeight - tile.offsetY, tileSize);

			taskQueue.push(tile);
		}
	}

	uint32 sampleCount = EstimateSampleCount(scene);
	Sampler::Ptr sampler(CreateSampler<RNGSampler, CameraSample>(samplePerPixel, sampleCount,114514));
	mutex filmWriteMutex;
	Camera::Ptr camera = this->camera;

	uint32 timeSampleIndex = sampler->GetSampleIndex("time").value();
	uint32 uvSampleIndex = sampler->GetSampleIndex("uv").value();
	uint32 lenSampleIndex = sampler->GetSampleIndex("len").value();

	uint32 totalTaskCount = taskQueue.size();


	dispatcher->Dispatch([&](uint32 threadId) {
		Sampler::Ptr localSampler;
		//main thread
		if (threadId == 0) {
			localSampler = sampler;
		}
		else {
			localSampler = Sampler::Ptr(sampler->Clone());
		}

		while (true) {
			if (threadId == 0) {
				al_log("remaining tasks/total task : {} / {}", taskQueue.size(), totalTaskCount);
			}

			optional<Tile> res = taskQueue.try_pop();
			if (!res.has_value()) {
				break;
			}
			Tile task = res.value();
			
			al_for(yi, 0, task.height) {
				al_for(xi, 0, task.width) {
					uint32 x = task.offsetX + xi;
					uint32 y = task.offsetY + yi;

					//for debugging
					CheckDebugBreak(x, y);

					

					localSampler->NextPixel(x, y);
					al_for(s, 0, samplePerPixel) {
						
						localSampler->NextSample();

						float s_time = *localSampler->GetSample(timeSampleIndex);
						Vector2f s_uv(localSampler->GetSample(uvSampleIndex));
						Vector2f s_len(localSampler->GetSample(lenSampleIndex));

						Ray r = camera->GenerateRay(s_time, s_uv, s_len, x, y);

						Vector3f L = Li(r, scene, localSampler);

						if (Math::contains_nan(L)) {

							al_warn("SampledIntegrator::Render : x {} y {} sample {} (s_time{},s_uv,s_len{}),"
								"nan radiance aquired.0 value is written to film", x, y, s, s_time, s_uv, s_len);
							L = Vector3f();
						}
						else if (Math::contains_inf(L)) {
							al_warn("SampledIntegrator::Render : x {} y {} sample {} (s_time{},s_uv{},s_len{}),"
								"inf radiance aquired.0 value is written to film", x, y, s, s_time, s_uv, s_len);
							L = Vector3f();
						}
						else if (L.x < 0.f || L.y < 0.f || L.z < 0.f) {
							al_warn("SampledIntegrator::Render : x {} y {} sample {} (s_time{},s_uv{},s_len{}),"
								"negative radiance L {} aquired.0 value is written to film",
								x, y, s, s_time, s_uv, s_len, L);
							L = Math::vmax(L, Vector3f());
						}
						
					

						camera->WriteFilm(L, x, y);
					}

					
				}
			}
		}
	});
}

void SampledIntegrator::SetLightSampleStrategy(LIGHT_SAMPLE_STRATEGY strategy) {
	this->lightSampleStrategy = strategy;
}

void SampledIntegrator::SetDirectLightEsitimateStrategy(DIRECT_LIGHT_ESITIMATE_STRATEGY strategy) {
	this->esitimateStrategy = strategy;
}

//see note 14.5
Vector3f DirectLightBSDFImportanceSample(const SurfaceIntersection& isect, const Ray& r, Light::Ptr light, Sampler::Ptr sampler,
	Scene::Ptr scene, float* scatterPdf) {
	SamplerStream& stream = sampler->GetSamplerStream();
	Vector2f sBsdf = stream.Sample2D();

	Material::Ptr mat = isect.material;
	Vector3f wi = mat->SampleBSDF(isect, sBsdf, r.d, scatterPdf);

	//? why abs
	Vector3f bsdf = mat->EvaluateBSDF(r.d, isect, wi) * std::abs(Math::dot(isect.shadingNormal, wi));
	
	Vector3f L;
	//scene intersection test is time consuming
	if (!Math::zero(bsdf)) {
		Ray visTester(isect.isect.position, wi);
		SurfaceIntersection testRes;
		if (scene->Intersect(visTester, testRes)) {
			auto v = testRes.material->GetEmission();

			if (v.has_value() && 
				//compare address of two pointer
				(void*)v.value().get() == (void*)light.get()) {
				L = light->SurfaceEmissionIntensity(testRes.isect, wi);
			}
		}
		else if (light->IsInfinity()) {
			//a envoriment light
			L = light->InfiniteIntensity(r);
		}
	}
	
	return L * bsdf;
}

//see note 14.5
Vector3f DirectLightLightingImportanceSample(const SurfaceIntersection& isect, const Ray& r, Light::Ptr light, Sampler::Ptr sampler,
	Scene::Ptr scene, float* lightPdf) {
	SamplerStream& stream = sampler->GetSamplerStream();
	Vector2f sLight = stream.Sample2D();
	
	VisiblityTester visTester;
	Vector3f wi,Li,bsdf;
	if (light->IsDeltaType()) {
		Vector3f wi;
		Li = light->DeltaIntensity(isect.isect, &wi, &visTester);
	}
	else {
		Li = light->SampleIntensity(isect.isect, sLight, &wi, lightPdf, &visTester);
	}
	if (!Math::zero(Li)) {
		//this sample is not visible
		if (!visTester.Visible(scene)) {
			Li = Vector3f();
		}
		else {
			Material::Ptr mat = isect.material;
			bsdf = mat->EvaluateBSDF(r.d, isect, wi) * std::abs(Math::dot(isect.shadingNormal, wi));
			Li = Li * bsdf;
		}
	}
	return Li;
}

//see note 14.5
Vector3f SampledIntegrator::EsitimateDirectLight(const SurfaceIntersection& isect, const Ray& r, Light::Ptr light, Sampler::Ptr sampler) {
	switch (esitimateStrategy) {
		case DIRECT_LIGHT_ESITIMATE_STRATEGY_BSDF: {
			float pdf;
			Vector3f L = DirectLightBSDFImportanceSample(isect, r, light, sampler, scene, &pdf);
			return L / pdf;
		}
		case DIRECT_LIGHT_ESITIMATE_STRATEGY_LIGHT: {
			float pdf;
			Vector3f L = DirectLightLightingImportanceSample(isect, r, light, sampler, scene, &pdf);
			if (!light->IsDeltaType()) {
				return L / pdf;
			}
			else {
				return L;
			}
		}
		case DIRECT_LIGHT_ESITIMATE_STRATEGY_MIS: {
			float lightPdf, scatterPdf;
			Vector3f L_light = DirectLightLightingImportanceSample(isect, r, light, sampler, scene, &lightPdf);
			if (light->IsDeltaType()) {
				return L_light;
			}
			Vector3f L_scatter = DirectLightBSDFImportanceSample(isect, r, light, sampler, scene, &scatterPdf);

			//sample weight
			float nl = lightPdf * lightPdf, ns = scatterPdf * scatterPdf;
			float W_light = nl / (nl + ns);
			float W_scatter = ns / (nl + ns);

			return L_light * (W_light / lightPdf) + L_scatter * (W_scatter / scatterPdf);
		}
	}
	al_warn("SampledIntegrator::EsitimateDirectLight : invalid estimate strategy {}",(uint32)esitimateStrategy);
	return Vector3f();
}

//see note 14.5
Vector3f SampledIntegrator::SampleDirectLightSource(const SurfaceIntersection& isect,const Ray& r,Sampler::Ptr sampler) {
	switch (lightSampleStrategy) {
		case LIGHT_SAMPLE_STRATEGY_ONE: {
			uint32 lightSourceCount = scene->GetLightSourceCount();
			if (lightSourceCount == 0) return Vector3f();
			
			SamplerStream& stream = sampler->GetSamplerStream();
			
			float sLight = stream.Sample1D() * (float)lightSourceCount;
			uint32 iLight = std::min((uint32)sLight, lightSourceCount - 1);
			
			Light::Ptr light = scene->GetLightSource(iLight);
			
			//Light::Ptr light = scene->PickOneFiniteLightSources(stream.Sample1D());
			//divide the pdf of the light sample
			Vector3f L;
			//light sources doesn't sample it self
			if (auto v = isect.material->GetEmission();v.has_value()
				&& (void*)v.value().get() == (void*)light.get()) {
				L = Vector3f();
			}
			else {
				L = EsitimateDirectLight(isect, r, light, sampler);
			}
			return L * lightSourceCount;
		}
		case LIGHT_SAMPLE_STRATEGY_ALL: {
			Vector3f L;
			al_for(i, 0, scene->GetLightSourceCount()) {
				Light::Ptr light = scene->GetLightSource(i);
				//light sources doesn't sample it self
				if (auto v = isect.material->GetEmission(); !v.has_value()
					|| (void*)v.value().get() != (void*)light.get()) {
					L = L + EsitimateDirectLight(isect, r, light, sampler);
				}
			}
			return L;
		}
	}
	return Vector3f();
}


