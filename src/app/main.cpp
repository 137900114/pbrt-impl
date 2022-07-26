#include "common/common.h"
#include "sampler/halton.h"
#include "scene/texture.h"
#include "scene/scene.h"
#include <iostream>
#include "light/area_light.h"


int main(){
	al_log_initialize();

	Scene::Ptr scene(new Scene);
	
	Sphere::Ptr sphere(new Sphere(1.0f));
	AreaLight::Ptr light(new AreaLight(sphere, Vector3f(1.f, 1.f, 1.f)));
	BSDF::Ptr bsdf(new LambertBSDF(Vector3f(1., 1., 1.)));

	Material::Ptr emission_mat(new Material(bsdf, light, 0, nullptr, nullptr));
	Material::Ptr diffuse_mat(new Material(bsdf, nullptr, 0, nullptr, nullptr));
	
	Transform trans1(Vector3f(0.f, 0.f, 5.f), Quaternion(), Vector3f(1.f, 1.f, 1.f));
	Transform trans2(Vector3f(0.f, 5.f, 5.f), Quaternion(), Vector3f(1.f, 1.f, 1.f));

	scene->CreateSceneObject(sphere, emission_mat, trans1);
	scene->CreateSceneObject(sphere, diffuse_mat , trans2);

	scene->Build();

	al_log_finalize();
}