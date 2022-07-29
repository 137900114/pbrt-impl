#include "scene_primitive.h"

Sphere::Sphere(float radius):radius(radius),r2(radius * radius) {
	al_assert(radius > 0, "radius of a sphere must be greater than 0.f");
}

Bound3f Sphere::GetBound(const Transform& trans) {
	Bound3f b;
	b.lower = trans.GetPosition() - Vector3f(radius, radius, radius);
	b.upper = trans.GetPosition() + Vector3f(radius, radius, radius);
	return b;
}

Intersection Sphere::Sample(const Transform& trans,const Intersection& p,
	const Vector2f& seed, float* pdf) {
	Vector3f offset = p.position - trans.GetPosition();
	float dc = Math::length(offset);
	//TODO:currently we don't support sample inside a sphere
	al_assert(dc >= radius, "Sphere::Sample : currently we don't support sampling inside a sphere");

	float sinThetaMax = Math::clamp(radius / dc, 0.0f, .9999f);
	float cosThetaMax = sqrtf(1.f - sinThetaMax * sinThetaMax);
	float sinTheta = sinThetaMax * seed.x;
	float sinTheta2 = Math::clamp(sinTheta * sinTheta,0.0f,.9999f);
	float cosTheta = sqrtf(1 - sinTheta2);
	float dc2 = dc * dc;

	//find alpha by solving triangle
	float ds = dc * cosTheta - sqrtf(max(r2 - sinTheta2 * dc2, 0.f));
	float cosAlpha = Math::clamp((dc2 + r2 - ds * ds) / (2 * radius * dc), -.9999f, .9999f);
	float sinAlpha = sqrtf(1.f - cosAlpha * cosAlpha);

	float phi = 2 * Math::pi * seed.y;

	//calculate the sample point coordinate
	Vector3f wy = Math::normalize(offset), wx, wz;
	if (fabsf(wy.x) > fabsf(wy.z)) {
		float d = sqrtf(wy.x * wy.x + wy.y * wy.y);
		wx = Vector3f(wy.y / d, -wy.x / d, 0.f);
	}
	else {
		float d = sqrtf(wy.z * wy.z + wy.y * wy.y);
		wx = Vector3f(0.f, -wy.z / d, wy.y / d);
	}
	wz = Math::cross(wy, wx);

	//calculate normal and position
	Vector3f n(sinAlpha * cos(phi), cosAlpha, sinAlpha * sin(phi));
	
	//TODO:encapsulate this to a function
	Intersection isect;
	isect.normal = wx* n.x + wy * n.y + wz * n.z;
	isect.position = trans.GetPosition() + isect.normal * radius;
	float alpha = Math::angle(sinAlpha, cosAlpha);
	isect.uv = Vector2f(phi / 2 * Math::pi, alpha / Math::pi);
	isect.localUv = isect.uv;

	//uniform cone area
	*pdf = 1.f / (2.f * Math::pi * (1 - cosThetaMax));

	return isect;
}


bool SphereIntersect(const ScenePrimitiveInfo& info,const Ray& r, Intersection& isect) {
	al_assert(info.type == SCENE_PRIMITIVE_TYPE_SPHERE, "SphereIntersect : invalid primitive intersector");
	const Transform& trans = info.data.sphere.trans;
	Vector3f o = r.o - trans.GetPosition();
	float radius = info.data.sphere.radius;

	float b = 2 * (r.d.x * o.x + r.d.y * o.y + r.d.z * o.z);
	float c = o.x * o.x + o.y * o.y + o.z * o.z - radius * radius;

	float delta = b * b - 4 * c;
	if (delta < 0) {
		return false;
	}
	float sqrtDelta = sqrtf(delta);
	float t1 = (-b + sqrtDelta) / 2;
	float t2 = (-b - sqrtDelta) / 2;
	if (t2 < t1) std::swap(t1, t2);

	//TODO : currently we don't deal with intersection from inside
	if (t1 < 0) return false;

	isect.t = t1;
	isect.position = (r.d * t1) + r.o;
	isect.normal = Math::normalize(isect.position - trans.GetPosition());
	//push the intersection point a little forward
	isect.position = isect.position + radius * 1e-3 * isect.normal;

	float cosTheta = Math::clamp(isect.normal.y, -.9999f, .9999f);
	float sinTheta = sqrtf(1.f - cosTheta * cosTheta);

	float sinPhi = Math::clamp(isect.normal.z / sinTheta, -.9999f, .9999f);
	float cosPhi = Math::clamp(isect.normal.x / sinTheta, -.9999f, .9999f);

	float theta = Math::angle(sinTheta, cosTheta),
		phi = Math::angle(sinPhi, cosPhi);

	isect.uv = Vector2f(phi / (2 * Math::pi), theta / Math::pi);
	isect.localUv = isect.uv;
	//derive normal by theta
	isect.tangent = Vector3f(cosTheta * cosPhi, -sinTheta, cosTheta * sinPhi);

	return true;
}


ScenePrimitiveInfo Sphere::GeneratePrimitiveInfo(const Transform& transform,
	Material::Ptr mat) {
	ScenePrimitiveInfo info;
	info.bound = GetBound(transform);
	info.intersector = SphereIntersect;
	info.data.sphere.radius = radius;
	info.data.sphere.trans = transform;
	info.type = SCENE_PRIMITIVE_TYPE_SPHERE;
	info.material = mat;
	return info;
}


void ScenePrimitiveInfo::Clone(const ScenePrimitiveInfo& info) {
	material = info.material;
	intersector = info.intersector;
	bound = info.bound;
	type = info.type;
	memcpy(&data, &info.data, sizeof(data));
}