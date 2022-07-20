#pragma once
#include "light.h"

class DirectionalLight : public Light {
public:
	al_add_ptr_t(DirectionalLight);

	DirectionalLight(Vector3f dir,const Vector3f& intensity);

	virtual Vector3f DeltaIntensity(const Intersection& isect, Vector3f* wi, 
		VisiblityTester* tester) override;
private:
	Vector3f dir;
};