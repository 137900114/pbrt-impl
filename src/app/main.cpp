#include "core.h"
#include <iostream>

constexpr uint32 width = 1000;
constexpr uint32 height = 1000;




int main(){
	al_log_initialize();

	Scene::Ptr scene(new Scene);
	
	const uint32 processor_count = std::thread::hardware_concurrency();

	Sphere::Ptr sphere(new Sphere(1.0f));
	AreaLight::Ptr light(new AreaLight(sphere, Vector3f::I * .5f));
	BSDF::Ptr bsdf(new LambertBSDF(Vector3f(1., 1., 1.)));
	BSDF::Ptr bsdf2(new LambertBSDF(Vector3f(1, 0, 0)));

	ModelID model;
	if (auto v = scene->LoadModel(AL_STR("C:\\Users\\Ì¸Ôìéª\\codes\\pbrt-impl\\assets\\monkey.obj")); v.has_value()) {
		model = v.value();
	}
	else {
		al_warn("fail to load model");
		return 0;
	}


	ConstantEnvironmentLight::Ptr env(new ConstantEnvironmentLight(Vector3f::I * .1f));

	Material::Ptr emission_mat(new Material(bsdf, light, 0, nullptr, nullptr));
	Material::Ptr diffuse_mat(new Material(bsdf2, nullptr, 0, nullptr, nullptr));
	
	Transform trans1(Vector3f(0.f, -1.f, 5.f), Quaternion(Vector3f(1.,0.,0.),Math::pi / 2.f), Vector3f::I);
	Transform trans2(Vector3f(0.f, .875f, 5.f), Quaternion(), Vector3f::I);

	scene->CreateSceneObject(sphere, emission_mat, trans2);
	scene->CreateSceneObject(model , trans1);

	scene->AddLightSource(env);

	scene->Build();

	Transform camTrans(Vector3f(), Quaternion(), Vector3f::I);
	PerspectiveCamera::Ptr camera(new PerspectiveCamera(width, height, camTrans));

	//NormalIntegrator::Ptr integrator(new NormalIntegrator);
	PathIntegrator::Ptr integrator(new PathIntegrator);
	integrator->SetSamplePerPixel(64);
	integrator->SetThreadNumber(processor_count - 1);
	integrator->SetMaxDepth(5);
	integrator->AttachCamera(camera);
	integrator->AttachScene(scene);
	integrator->SetDirectLightEsitimateStrategy(DIRECT_LIGHT_ESITIMATE_STRATEGY_MIS);

	//integrator->DebugBreakAtPixel(197, 207);
	integrator->Render();

	camera->SaveFilm(AL_STR("illumiance.png"));

	al_log_finalize();
}