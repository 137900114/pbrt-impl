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
	isect.adjustedPosition = isect.position + isect.normal * Math::eta * 5.f;
	isect.tangent = GenerateTangent(isect.normal);

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
	isect.adjustedPosition = isect.position + isect.normal * Math::eta * 5.f;

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

Vector3f GenerateTangent(const Vector3f& normal) {
	Vector3f t = Math::cross(normal, Vector3f::Up);
	if (Math::zero(t)) {
		t = Math::cross(normal, Vector3f::Right);
	}
	return Math::normalize(t);
}

bool PlaneIntersect(const ScenePrimitiveInfo& info, const Ray& r, Intersection& isect) {
	Vector3f& position = isect.position;
	Vector2f& uv = isect.localUv;
	float&    t = isect.t;
	
	if (!Math::ray_intersect_plane(info.data.plane.dl,info.data.plane.dr,
		info.data.plane.ul,r,&t,&uv,&position)) {
		return false;
	}
	
	isect.normal = info.data.plane.n;
	if (Math::dot(isect.normal,r.d) > 0.f) {
		isect.normal = -1 * isect.normal;
	}
	isect.tangent = GenerateTangent(isect.normal);
	isect.adjustedPosition = isect.position + isect.normal * Math::eta * 5.f;
	isect.uv = isect.localUv;
	return true;
}

Plane::Plane(float width, float height):width(width),height(height),invArea( 1.f / (width * height)) {
	al_assert(width > 0.f && height > 0.f, "width and height of a plane should be greater than 0");
}

Bound3f Plane::GetBound(const Transform& trans) {
	Vector3f p0(-width / 2.f, 0.f, height / 2.f), p1(width / 2.f, 0.f, height / 2.f),
		p2(-width / 2.f, 0.f, -height / 2.f), p3(width / 2.f, 0.f, -height / 2.f);
	p0 = Math::transform_point(trans.GetMatrix(), p0);
	p1 = Math::transform_point(trans.GetMatrix(), p1);
	p2 = Math::transform_point(trans.GetMatrix(), p2);
	p3 = Math::transform_point(trans.GetMatrix(), p3);

	Bound3f bound;
	bound = Math::bound_merge(bound, p0);
	bound = Math::bound_merge(bound, p1);
	bound = Math::bound_merge(bound, p2);
	bound = Math::bound_merge(bound, p3);
	return bound;
}

Intersection Plane::Sample(const Transform& trans, const Intersection& p,
	const Vector2f& seed, float* pdf) {
	//normal in local space
	Vector3f normal(0.f, 1.f, 0.f);
	normal = Math::transform_vector(trans.GetTransInvMatrix(), normal);

	//adjust to back face if the p is on the other side
	if (Math::dot(normal,p.position - trans.GetPosition()) < 0.f) {
		normal = -1 * normal;
	}
	Intersection isect;
	//point in local space
	Vector3f p1((seed.x - .5f) * width, 0.f, (seed.y - .5f ) * height);
	p1 = Math::transform_point(trans.GetMatrix(), p1);
	isect.position = p1;
	isect.normal = normal;
	isect.uv = Vector2f(seed.x, seed.y);
	isect.adjustedPosition = isect.normal * Math::eta * 5.f + isect.position;
	isect.tangent = GenerateTangent(isect.normal);
	isect.localUv = isect.uv;

	Vector3f dist = p1 - p.position;
	float d2 = Math::dot(dist, dist);
	//see note 14.2
	*pdf = invArea * d2 / max(abs(Math::dot(dist, isect.normal)), 1e-8f);
	return isect;
}

ScenePrimitiveInfo Plane::GeneratePrimitiveInfo(const Transform& transform,
	Material::Ptr mat) {
	ScenePrimitiveInfo info;
	info.bound = GetBound(transform);
	info.material = mat;
	info.type = SCENE_PRIMITIVE_TYPE_PLANE;
	info.intersector = PlaneIntersect;
	
	//local space
	Vector3f ul(-width / 2.f, 0, height / 2.f);
	Vector3f dl(-width / 2.f, 0,-height / 2.f);
	Vector3f dr( width / 2.f, 0,-height / 2.f);
	Vector3f n(0.f, 1.f, 0.f);

	//from local space to world space
	info.data.plane.ul = Math::transform_point(transform.GetMatrix(), ul);
	info.data.plane.dl = Math::transform_point(transform.GetMatrix(), dl);
	info.data.plane.dr = Math::transform_point(transform.GetMatrix(), dr);
	info.data.plane.n  = Math::transform_vector(transform.GetTransInvMatrix(), n);

	return info;
}