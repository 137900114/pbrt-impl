#include "environment_light.h"

ConstantEnvironmentLight::ConstantEnvironmentLight(const Vector3f & intensity):
	Light(LIGHT_TYPE_INFINITY,intensity) {
}

//uniform sample on a hemisphere
Vector3f ConstantEnvironmentLight::SampleIntensity(const Intersection& isect, const Vector2f& seed
	, Vector3f* wi, float* pdf, VisiblityTester* tester) {
	float sqrtOneMinusZeta1 = Math::safeSqrtOneMinusSq(seed.x);
	
	//see note 13.2
	float z = seed.x;
	float y = sinf(seed.y * 2.f * Math::pi) * sqrtOneMinusZeta1;
	float x = cosf(seed.x * 2.f * Math::pi) * sqrtOneMinusZeta1;

	Vector3f bitangent = Math::cross(isect.normal, isect.tangent);
	
	//TODO:encapsulate this to a function
	*wi = isect.normal * y + isect.tangent * x + bitangent * z;
	*pdf = 1.f / (2.f * Math::pi);
	return intensity;
}

float SamplePdf(const Intersection& isect, const Vector3f& wi) {
	return 1.f / (2.f * Math::pi);
}

Vector3f ConstantEnvironmentLight::InfiniteIntensity(const Ray& r) {
	return intensity;
}