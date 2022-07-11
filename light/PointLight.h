#pragma once
#include "light/light.h"

class PointLight : public Light {
public:
	PointLight(Vector3f position, float intensity);

	virtual LightSample Sample(const Ray& r) override;

private:
	Vector3f position;
};