#include "core.h"
#include <iostream>

constexpr uint32 width = 400;
constexpr uint32 height = 400;

int main(){
	al_log_initialize();

	Scene::Ptr scene(new Scene);
	
	Sphere::Ptr sphere(new Sphere(1.0f));
	AreaLight::Ptr light(new AreaLight(sphere, Vector3f::I * 2.f));
	BSDF::Ptr bsdf(new LambertBSDF(Vector3f(1., 1., 1.)));

	Material::Ptr emission_mat(new Material(bsdf, light, 0, nullptr, nullptr));
	Material::Ptr diffuse_mat(new Material(bsdf, nullptr, 0, nullptr, nullptr));
	
	Transform trans1(Vector3f(0.f, -1.01f, 3.f), Quaternion(), Vector3f::I);
	Transform trans2(Vector3f(0.f, 1.01f, 3.f), Quaternion(), Vector3f::I);


	scene->CreateSceneObject(sphere, emission_mat, trans1);
	scene->CreateSceneObject(sphere, diffuse_mat , trans2);

	scene->Build();

	Transform camTrans(Vector3f(), Quaternion(), Vector3f::I);
	PerspectiveCamera::Ptr camera(new PerspectiveCamera(width, height, camTrans));

	PathIntegrator::Ptr integrator(new PathIntegrator);
	integrator->SetSamplePerPixel(64);
	integrator->SetThreadNumber(16);
	integrator->SetMaxDepth(5);
	integrator->AttachCamera(camera);
	integrator->AttachScene(scene);
	integrator->SetDirectLightEsitimateStrategy(DIRECT_LIGHT_ESITIMATE_STRATEGY_MIS);

	integrator->Render();


	camera->SaveFilm(AL_STR("a.png"));

	al_log_finalize();
}