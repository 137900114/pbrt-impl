#include "point_light.h"

PointLight::PointLight(Vector3f position,const Vector3f& intensity) :
	Light(LIGHT_TYPE_POINT, intensity),
	position(position) 
{}

Vector3f PointLight::DeltaIntensity(const Intersection& isect, Vector3f* wi, VisiblityTester* tester) {
	*tester = VisiblityTester(isect.position,position);

	Vector3f dis = Math::vsub(isect.position, position);
	float len = Math::length(dis);
	*wi = Math::vdiv(dis, len);

	return Math::vdiv(intensity, len * len);
}