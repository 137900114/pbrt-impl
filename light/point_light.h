#pragma once
#include "light/light.h"

class PointLight : public Light {
public:
	al_add_ptr_t(PointLight);
	PointLight(Vector3f position,const Vector3f& intensity);

	virtual Vector3f DeltaIntensity(const Intersection& isect, Vector3f* wi, VisiblityTester* tester) override;
private:
	Vector3f position;
};