#include "light.h"
#include "scene/scene.h"

bool VisiblityTester::Visible(shared_ptr<Scene> scene) {
	Ray r;
	r.o = p0;
	r.d = dir;
	Intersection isect = scene->Intersect(r).intersection;
	return !isect.intersected || isect.t > distance;
}

Light::Light(LIGHT_TYPE type,const Vector3f& intensity):
	type(type),intensity(intensity){
	al_assert(intensity.x >= 0.f && intensity.y > 0.f 
		&& intensity.z >= 0.f, "Light::Light : the intensity of the light ({},{},{})"
		" should be greater than 0", intensity.x,intensity.y,intensity.z);
}

bool Light::IsDeltaType() {
	return type == LIGHT_TYPE_DIRECTIONAL ||
		type == LIGHT_TYPE_POINT;
}

Vector3f Light::DeltaIntensity(const Intersection& isect, Vector3f* wi, VisiblityTester* tester) {
	return Vector3f();
}

Vector3f Light::SampleIntensity(const Intersection& isect, Vector3f* wi, float* pdf, VisiblityTester* tester) {
	return Vector3f();
}

Vector3f Light::SurfaceEmissionIntensity(const Intersection& isect, const Vector3f& w) {
	return Vector3f();
}