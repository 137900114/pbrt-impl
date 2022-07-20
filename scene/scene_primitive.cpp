#include "scene_primitive.h"

Sphere::Sphere(float radius):radius(radius),r2(radius * radius) {
	al_assert(radius > 0, "radius of a sphere must be greater than 0.f");
}

Bound3f Sphere::GetBound(const Transform& trans) {
	Bound3f b;
	b.lower = Math::vadd(trans.GetPosition(), Vector3f(-radius, -radius, -radius));
	b.upper = Math::vadd(trans.GetPosition(), Vector3f(radius, radius, radius));
	return b;
}

Intersection Sphere::Sample(const Transform& trans,const Intersection& p,
	const Vector2f& seed, float* pdf) {
	Vector3f offset = Math::vsub(p.position, trans.GetPosition());
	float dc = Math::length(offset);
	//TODO:currently we don't support sample inside a sphere
	al_assert(dc >= radius, "Sphere::Sample : currently we don't support sampling inside a sphere");

	float sinThetaMax = radius / dc;
	float sinTheta = sinThetaMax * seed.x;
	float sinTheta2 = sinTheta * sinTheta;
	float cosTheta = sqrtf(1 - sinTheta2);
	float dc2 = dc * dc;

	//find alpha by solving triangle
	float ds = dc * cosTheta - sqrtf(r2 - sinTheta2 * dc2);
	float cosAlpha = (dc2 + r2 - ds * ds) / (2 * radius * dc);
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
	
	Intersection isect;
	isect.normal =
		Math::vadd(
			  Math::vmul(wx, n.x),
			Math::vadd(
				  Math::vmul(wy, n.y),
				Math::vmul(wz, n.z)
			)
		);
	isect.position = Math::vadd(trans.GetPosition(), Math::vmul(isect.normal, radius));
	float alpha = Math::angle(sinAlpha, cosAlpha);
	isect.uv = Vector2f(phi / 2 * Math::pi, alpha / Math::pi);
	isect.localUv = isect.uv;

	return isect;
}


bool SphereIntersect(const ScenePrimitiveInfo& info,const Ray& r, Intersection& isect) {
	al_assert(info.type != SCENE_PRIMITIVE_TYPE_SPHERE, "SphereIntersect : invalid primitive intersector");
	const Transform& trans = info.data.sphere.trans;
	Vector3f o = Math::vsub(trans.GetPosition(), r.o);
	float radius = info.data.sphere.radius;

	float a = r.d.x * r.d.x + r.d.y * r.d.y + r.d.z * r.d.z;
	float b = 2 * (r.d.x * o.x + r.d.y * o.y + r.d.z * o.z);
	float c = o.x * o.x + o.y * o.y + o.z * o.z - radius * radius;

	float delta = b * b - 4 * a * c;
	if (delta < 0) {
		return false;
	}
	float sqrtDelta = sqrtf(delta);
	float t1 = (-b + sqrtDelta) / (2 * a);
	float t2 = (-b - sqrtDelta) / (2 * a);
	if (t2 < t1) std::swap(t1, t2);

	//TODO : currently we don't deal with intersection from inside
	if (t1 < 0) return false;

	isect.t = t1;
	isect.position = Math::vadd(Math::vmul(r.d, t1), r.o);
	isect.normal = Math::normalize(Math::vsub(isect.position, trans.GetPosition()));

	float cosTheta = Math::clamp(isect.normal.y, -1.f, 1.f);
	float sinTheta = sqrtf(1.f - cosTheta * cosTheta);

	float sinPhi = Math::clamp(isect.normal.z / sinTheta, -1.f, 1.f);
	float cosPhi = Math::clamp(isect.normal.x / sinTheta, -1.f, 1.f);

	float theta = Math::angle(sinTheta, cosTheta),
		phi = Math::angle(sinPhi, cosPhi);

	isect.uv = Vector2f(phi / 2 * Math::pi, theta / Math::pi);
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