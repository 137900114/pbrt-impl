#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector3f dir,const Vector3f& intensity):
	Light(LIGHT_TYPE_DIRECTIONAL,intensity),dir(dir){}

LightSample DirectionalLight::Sample(const Ray& r) {
	LightSample rv;

	rv.dir = r.d;
	rv.I = intensity;
	rv.pos = Vector3f(infinity, infinity, infinity);

	return rv;
}