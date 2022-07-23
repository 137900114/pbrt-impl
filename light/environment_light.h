#pragma once
#include "light/light.h"

class ConstantEnvironmentLight : public Light {
public:

	ConstantEnvironmentLight(const Vector3f& intensity);

	virtual Vector3f SampleIntensity(const Intersection& isect, const Vector2f& seed
		, Vector3f* wi, float* pdf, VisiblityTester* tester) override;
	virtual Vector3f InfiniteIntensity(const Ray& r) override;
};