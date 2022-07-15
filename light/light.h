#pragma once
#include "common/common.h"
#include "math/math.h"

struct LightSample{
	Vector3f dir;
	Vector3f I;
	Vector3f pos;
};

enum LIGHT_TYPE {
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_DIRECTIONAL,
	LIGHT_TYPE_AREA
};

class Light {
public:
	al_add_ptr_t(Light);
	Light(LIGHT_TYPE type,const Vector3f& intensity);

	bool IsDeltaType();
	
	Vector3f DeltaIntensity();
	Vector3f SampleIntensity();


	LIGHT_TYPE GetLightType() {}

	virtual ~Light() {}
protected:
	Vector3f   intensity;
	LIGHT_TYPE type;
};

template<typename T,typename ...Args>
Light* CreateLight(Args... args) {
	static_assert(std::is_base_of_v<Light, T>, "light type should be derived from class 'Light'");
	return al_new(T, args...);
}