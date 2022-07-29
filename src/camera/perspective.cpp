#include "camera/perspective.h"

PerspectiveCamera* PerspectiveCamera::SetFov(float fov) {
	if (fov < 0 || fov > 179.f) {
		al_warn("PerspectiveCamera::SetFov : invalid fov value {},fov should be less than "
			"180 while greater than 0", fov);
	}
	else {
		//we use radius represent angles internally
		//half of the original fov
		this->fov = fov * Math::pi / 360.f;
		tanFov = tanf(this->fov);
	}
	return this;
}

PerspectiveCamera* PerspectiveCamera::SetLenRadius(float lr) {
	if (lr < 0.f) {
		al_warn("PerspectiveCamera::SetLenRadius : invalid len radius value {}"
			",len radius should be greater than 0", lr);
	}
	else {
		this->lenRadius = lr;
	}
	return this;
}

PerspectiveCamera* PerspectiveCamera::SetNearPlane(float near) {
	if (near < 0.f || near > far) {
		al_warn("PerspectiveCamera::SetNearPlane : invalid near plane value {}"
			",near plane value should be greater than 0 and less than far plane value {}", near, far);
	}
	else {
		this->near = near;
	}
	return this;
}

PerspectiveCamera* PerspectiveCamera::SetFarPlane(float far) {
	if (far < near) {
		al_warn("PerspectiveCamera::SetFarPlane : invalid far plane value {}"
			",far plane value should be greater than near plane value {}", far, near);
	}
	else {
		this->far = far;
	}
	return this;
}


PerspectiveCamera* PerspectiveCamera::SetFocusPlane(float focus) {
	if (focus > far || focus < near) {
		al_warn("PerspectiveCamera::SetFocusPlane : invalid focus plane value {}"
			",focus plane value should be between the near and far plane ({}~{})", focus,
			near, far);
	}
	else {
		focusPlane = focus;
	}
	return this;
}

PerspectiveCamera::PerspectiveCamera(float fov, float lenRadius,
	float near, float far,float focusPlane ,uint32 width, uint32 height,
	const Transform& trans) :Camera(width, height, trans),
	fov(Math::pi * .25f), //by default the fov is 90 degree 
	lenRadius(0.f), near(0.1f), far(1000.f), aspectRatio((float)width / (float)height),
	focusPlane((this->near + this->far) / 2.f) {

	SetLenRadius(lenRadius);
	SetNearPlane(near);
	SetFarPlane(far);
	SetFov(fov);
	SetFocusPlane(focusPlane);

	tanFov = tanf(this->fov);
}

PerspectiveCamera::PerspectiveCamera(uint32 width, uint32 height,
	const Transform& trans) :Camera(width, height, trans),
	fov(Math::pi * .25f), //by default the fov is 90 degree
	lenRadius(0.f), near(0.1f), far(1000.f),
	aspectRatio((float)width / (float)height),
	focusPlane((near + far) / 2.f)
{
	tanFov = tanf(this->fov);
}


Ray PerspectiveCamera::GenerateRay(float s_time, Vector2f s_uv, Vector2f s_len,
	uint32 x, uint32 y) {

	al_assert(x < width, "PerspectiveCamera::GenerateRay : fail to generate ray.X coordinate {} "
		"should be less than width of the film {}", x, width);
	al_assert(y < height, "PerspectiveCamera::GenerateRay : fail to generate ray.Y coordinate {} "
		"should be less than the height of the film {}", y, height);

	//add offset within a pixel for aa
	float ndcX =   ((float)x + s_uv.x) / (float)height * 2.f  -  (float)width / (float)height,
		  ndcY = - ((float)y + s_uv.y) / (float)height * 2.f  +  1.f;

	Vector3f pFilm(ndcX ,ndcY , 1.f);

	Ray rv(Vector3f(), Math::normalize(pFilm));
	if (lenRadius > 0.f) {
		float ft = focusPlane * rv.invd.z;
		Vector3f lenOffset(s_len.x * lenRadius, s_len.y * lenRadius, 0.f);
		Vector3f fPos = rv.d * ft + rv.o;
		rv = Ray(lenOffset, Math::normalize(fPos - lenOffset));
	}

	//from camera local space to world space
	rv.SetPosition(Math::transform_point(transform.GetMatrix(), rv.o));
	rv.SetDirection(Math::transform_vector(transform.GetMatrix(), rv.d));

	return rv;
}