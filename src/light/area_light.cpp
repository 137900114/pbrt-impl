#include "area_light.h"
#include "scene/scene_primitive.h"

AreaLight::AreaLight(ScenePrimitive::Ptr primitive, const Vector3f& intensity):
	primitive(primitive),Light(LIGHT_TYPE_AREA,intensity) {}

void AreaLight::SetTransform(Transform::Ptr trans) {
	al_assert(trans != nullptr,"AreaLight::SetTransform : transform should not be nullptr");
	this->trans = trans;
}

Vector3f AreaLight::SampleIntensity(const Intersection& isect, const Vector2f& seed, Vector3f* wi, float* pdf, VisiblityTester* tester) {
	Intersection sample = primitive->Sample(*trans, isect, seed, pdf);
	*tester = VisiblityTester(sample.position, isect.position);
	*wi = Math::normalize(isect.position - sample.position);

	return intensity;
}

//We don't need to multiply cos(theta) / r^2 to this,because this function 
//is sampled from directional angle.There is no need to apply differential variable transformation.
Vector3f AreaLight::SurfaceEmissionIntensity(const Intersection& isect, const Vector3f& w) {
	float cosTheta = -Math::dot(isect.normal, w);
	return cosTheta > 0 ? intensity : Vector3f();
}