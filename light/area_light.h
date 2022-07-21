#pragma once
#include "light/light.h"

class ScenePrimitive;

class AreaLight : public Light {
public:
	al_add_ptr_t(AreaLight);

	AreaLight(shared_ptr<ScenePrimitive> primitive, const Vector3f& intensity);

	virtual Vector3f SampleIntensity(const Intersection& isect,const Vector2f& seed, Vector3f* wi, float* pdf, VisiblityTester* tester) override;

	virtual Vector3f SurfaceEmissionIntensity(const Intersection& isect, const Vector3f& w) override;

	void SetTransform(Transform::Ptr trans);
private:
	shared_ptr<ScenePrimitive> primitive;
	Transform::Ptr trans;
};