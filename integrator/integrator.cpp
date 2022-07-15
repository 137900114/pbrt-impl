#include "integrator.h"
#include "multithread/concurrent_queue.h"
#include "multithread/dispatcher.h"
#include "sampler/halton.h"
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
	al_assert(camera != nullptr,"SampledIntegrator::Render : a camera should be attached to integrator before rendering");
	al_assert(scene  != nullptr,"SampledIntegrator::Render : a scene  should be attached to integrator before rendering");

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
	al_for(y, 0, xTileCount) {
		al_for(x, 0, yTileCount) {
			Tile tile;
			tile.offsetX = x * tileSize;
			tile.offsetY = y * tileSize;
			tile.width = min(rtWidth - tile.offsetX, tileSize);
			tile.height = min(rtHeight - tile.offsetY, tileSize);

			taskQueue.push(tile);
		}
	}

	uint32 sampleCount = EstimateSampleCount(scene);
	Sampler::Ptr sampler(CreateSampler<HaltonSampler, CameraSample>(samplePerPixel, sampleCount));
	mutex filmWriteMutex;
	Camera::Ptr camera = this->camera;

	uint32 timeSampleIndex = sampler->GetSampleIndex("time").value();
	uint32 uvSampleIndex = sampler->GetSampleIndex("uv").value();
	uint32 lenSampleIndex = sampler->GetSampleIndex("len").value();

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
			optional<Tile> res = taskQueue.try_pop();
			if (!res.has_value()) {
				break;
			}
			Tile task = res.value();
			al_for(yi, 0, task.width) {
				al_for(xi, 0, task.height) {
					uint32 x = task.offsetX + xi;
					uint32 y = task.offsetY + yi;

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
								"negative radiance L({},{},{}) aquired.0 value is written to film",
								x, y, s, s_time, s_uv, s_len, L.x, L.y, L.z);
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