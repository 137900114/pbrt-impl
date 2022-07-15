#pragma once
#include "light.h"

class DirectionalLight : public Light {
public:
	DirectionalLight(Vector3f dir,const Vector3f& intensity);

	virtual Vector3f DeltaIntensity(const Intersection& isect, Vector3f* wi, 
		VisiblityTester* tester) override;
private:
	Vector3f dir;
};