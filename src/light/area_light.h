#pragma once
#include "light/light.h"

class ScenePrimitive;

//an arealight should be created for a material
//sharing arealight among a set of materials is dangerous
class AreaLight : public Light {
public:
	al_add_ptr_t(AreaLight);

	AreaLight(shared_ptr<ScenePrimitive> primitive, const Vector3f& intensity);

	virtual Vector3f SampleIntensity(const Intersection& isect,const Vector2f& seed, Vector3f* wi, float* pdf, VisiblityTester* tester) override;

	virtual Vector3f SurfaceEmissionIntensity(const Intersection& isect, const Vector3f& w) override;

	ScenePrimitive* GetPrimitive() { return primitive.get(); }

	//this function should be called by Scene
	//area light should know it's transform,
	//but only when an area light is placed in a scene
	//it could know it's transformation
	//TODO: refactor this function
	void SetTransform(Transform::Ptr trans);
private:
	shared_ptr<ScenePrimitive> primitive;
	Transform::Ptr trans;
};