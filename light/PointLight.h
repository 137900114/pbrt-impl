#pragma once
#include "light/light.h"

class PointLight : public Light {
public:
	PointLight(Vector3f position,const Vector3f& intensity);

private:
	Vector3f position;
};