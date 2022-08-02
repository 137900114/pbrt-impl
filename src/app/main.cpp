#include "core.h"
#include "integrator/integrator.h"
#include "scene/scene_primitive.h"
#include <iostream>

constexpr uint32 width = 1000;
constexpr uint32 height = 1000;


int main(int argc,const char** argvs){
	al_log_initialize();

	Scene::Ptr scene(new Scene);
	
	const uint32 processor_count = std::thread::hardware_concurrency();

	ConstantEnvironmentLight::Ptr env(new ConstantEnvironmentLight(Vector3f::I * .1f));
	

	Plane::Ptr  plane(new Plane(40.f, 40.f));
	BSDF::Ptr bsdf(new LambertBSDF(Vector3f(6, 102, 255) / 255.f));
	Material::Ptr diffuse_mat2(new Material(bsdf, nullptr, 0, nullptr));
	Transform trans3(Vector3f(0.f, -10.f, 13.f), Quaternion(), Vector3f::I);
	scene->CreateSceneObject(plane, diffuse_mat2, trans3);

	Plane::Ptr  plane2(new Plane(10.f, 10.f));
	Transform trans4(Vector3f(-10.f, -5.f, 13.f), Quaternion(Vector3f(0.f, 0.f, 1.f), Math::pi / 2.f), Vector3f::I);
	BSDF::Ptr bsdf2(new LambertBSDF(Vector3f(0.8, 1., 1.)));
	AreaLight::Ptr light(new AreaLight(plane2, Vector3f::I * 3.f));
	Material::Ptr emission_mat(new Material(bsdf2, light, 0, nullptr));
	scene->CreateSceneObject(plane2, emission_mat, trans4);

	Plane::Ptr  plane3(new Plane(30.f, 10.f));
	BSDF::Ptr bsdf3(new LambertBSDF(Vector3f(0.5, 1, 0)));
	Material::Ptr diffuse_mat(new Material(bsdf3, nullptr, 0, nullptr));
	Transform trans5(Vector3f(8.f, -7.5f, 13.f), Quaternion(Vector3f(0.f, 0.f, 1.f), Math::pi / 2.f), Vector3f::I);
	scene->CreateSceneObject(plane3, diffuse_mat, trans5);

	Sphere::Ptr sphere(new Sphere(1.0f));
	AreaLight::Ptr light2(new AreaLight(sphere, Vector3f(1.0f, 0.f, 1.f) * 10.f));
	BSDF::Ptr bsdf4(new LambertBSDF(Vector3f(0.8, 1., 1.)));
	Material::Ptr emission_mat2(new Material(bsdf4, light2, 0, nullptr));
	Transform trans1(Vector3f(3.f, -9.f, 13.f), Quaternion(Vector3f(1., 0., 0.), Math::pi / 2.f), Vector3f::I);
	scene->CreateSceneObject(sphere, emission_mat2, trans1);

	ModelID model;
	if (auto v = scene->LoadModel(AL_STR("C:\\Users\\谈造楠\\codes\\pbrt-impl\\assets\\model\\model.pmx")); v.has_value()) {
		//if (auto v = scene->LoadModel(AL_STR("C:\\Users\\谈造楠\\codes\\pbrt-impl\\assets\\monkey.obj")); v.has_value()) {
		model = v.value();
	}
	else {
		al_warn("fail to load model");
		return 0;
	}
	Transform trans2(Vector3f(0.f, -10.f, 13.f), Quaternion(), Vector3f::I * .5f);
	scene->CreateSceneObject(model, trans2);
	//scene->AddLightSource(env);

	scene->Build();

	Transform camTrans(Vector3f(), Quaternion(), Vector3f::I);
	PerspectiveCamera::Ptr camera(new PerspectiveCamera(width, height, camTrans));

	//NormalIntegrator::Ptr integrator(new NormalIntegrator);
	PathIntegrator::Ptr integrator(new PathIntegrator);
	integrator->SetSamplePerPixel(128);
	integrator->SetThreadNumber(16);
	integrator->SetMaxDepth(5);
	integrator->AttachCamera(camera);
	integrator->AttachScene(scene);
	integrator->SetDirectLightEsitimateStrategy(DIRECT_LIGHT_ESITIMATE_STRATEGY_MIS);

	integrator->Render();

	camera->SaveFilm(AL_STR("miku_illumance4.png"));

	al_log_finalize();
}