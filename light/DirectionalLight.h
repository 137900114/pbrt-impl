#pragma once
#include "light.h"

class DirectionalLight : public Light {
public:
	DirectionalLight(Vector3f dir,const Vector3f& intensity);

	LightSample Sample(const Ray& r);

private:
	Vector3f dir;
};