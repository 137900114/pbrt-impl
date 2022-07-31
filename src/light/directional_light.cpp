#include "directional_light.h"

DirectionalLight::DirectionalLight(Vector3f dir,const Vector3f& intensity):
	Light(LIGHT_TYPE_DIRECTIONAL,intensity),dir(dir){}

constexpr float sceneBoundary = 1e5f;

Vector3f DirectionalLight::DeltaIntensity(const Intersection& isect, Vector3f* wi,
	VisiblityTester* tester) {
	*tester = VisiblityTester(isect.adjustedPosition,  dir * -1, infinity);

	*wi = dir;
	return intensity;
}