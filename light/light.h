#pragma once
#include "common/common.h"
#include "math/math.h"

struct LightSample{
	Vector3f dir;
	float    I;
	Vector3f pos;
};

enum LIGHT_TYPE {
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_DIRECTIONAL,
};

class Light {
public:
	Light(LIGHT_TYPE type, float intensity);

	virtual LightSample Sample(const Ray& r) = 0;

	LIGHT_TYPE GetLightType() {}

	virtual ~Light() {}
protected:
	float intensity;
	LIGHT_TYPE type;
};

template<typename T,typename ...Args>
Light* CreateLight(Args... args) {
	static_assert(std::is_base_of_v<Light, T>, "light type should be derived from class 'Light'");
	return al_new(T, args...);
}