#include "light.h"
#include "scene/scene.h"

bool VisiblityTester::Visible(shared_ptr<Scene> scene) {
	Ray r;
	r.o = p0;
	r.d = dir;
	SurfaceIntersection isect;
	bool intersected = scene->Intersect(r,isect);
	return !intersected || isect.isect.t > distance;
}

Light::Light(LIGHT_TYPE type,const Vector3f& intensity):
	type(type),intensity(intensity){
	al_assert(intensity.x >= 0.f && intensity.y >= 0.f 
		&& intensity.z >= 0.f, "Light::Light : the intensity of the light ({},{},{})"
		" should be greater than 0", intensity.x,intensity.y,intensity.z);
}

bool Light::IsDeltaType() {
	return type == LIGHT_TYPE_DIRECTIONAL ||
		type == LIGHT_TYPE_POINT;
}

bool Light::IsInfinity() {
	return type == LIGHT_TYPE_INFINITY;
}

bool Light::IsArea() {
	return type == LIGHT_TYPE_AREA;
}

Vector3f Light::DeltaIntensity(const Intersection& isect, Vector3f* wi, VisiblityTester* tester) {
	return Vector3f();
}

Vector3f Light::SampleIntensity(const Intersection& isect, const Vector2f& seed, Vector3f* wi, float* pdf, VisiblityTester* tester) {
	return Vector3f();
}

Vector3f Light::SurfaceEmissionIntensity(const Intersection& isect, const Vector3f& w) {
	return Vector3f();
}

Vector3f Light::InfiniteIntensity(const Ray& r) {
	return Vector3f();
}

float Light::SamplePdf(const Intersection& isect, const Vector3f& wi) {
	return 0.f;
}

VisiblityTester::VisiblityTester(const VisiblityTester& vt) :p0(vt.p0),
dir(vt.dir), distance(vt.distance) {
	al_assert(al_fequal(Math::length(dir), 1.f), "direction should be normalized");
}

VisiblityTester::VisiblityTester(const Vector3f& p0, const Vector3f& p1) :
	p0(p0), dir(Math::normalize(p1 - p0)) {
	distance = Math::length(p0 - p1);
}

VisiblityTester::VisiblityTester(const Vector3f& p0, const Vector3f& dir, float distance) :
	p0(p0), dir(dir), distance(distance) {
}

const VisiblityTester& VisiblityTester::operator=(const VisiblityTester& other) {
	p0 = other.p0, dir = other.dir;
	distance = other.distance;
	al_assert(al_fequal(Math::length(dir), 1.f), "direction should be normalized");
	return *this;
}