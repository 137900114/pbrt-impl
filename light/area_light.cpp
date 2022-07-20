#include "area_light.h"
#include "scene/scene_primitive.h"

AreaLight::AreaLight(ScenePrimitive::Ptr primitive, const Vector3f& intensity):
	primitive(primitive),Light(LIGHT_TYPE_AREA,intensity) {}

Vector3f AreaLight::SampleIntensity(const Intersection& isect, Vector3f* wi, float* pdf, VisiblityTester* tester) {
	
}

Vector3f AreaLight::SurfaceEmissionIntensity(const Intersection& isect, const Vector3f& w) {

}