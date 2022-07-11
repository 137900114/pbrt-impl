#include "PointLight.h"

PointLight::PointLight(Vector3f position, float intensity) :
	Light(LIGHT_TYPE_POINT, intensity) {

}

LightSample PointLight::Sample(const Ray& r) {
	LightSample rv;

	Vector3f distant = Math::vsub(position, r.o);

	rv.dir = Math::normalize(distant);
	rv.pos = position;
	rv.I =   intensity / Math::length(distant);

	return rv;
}