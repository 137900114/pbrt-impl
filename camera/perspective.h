#pragma once
#include "camera.h"

class PerspectiveCamera : public Camera {
public:
	al_add_ptr_t(PerspectiveCamera);

	PerspectiveCamera(float fov, float lenRadius,
		float near, float far, float focusPlane, uint32 width, uint32 height,
		const Transform& trans);
	PerspectiveCamera(uint32 width, uint32 height,
		const Transform& trans);

	virtual Ray     GenerateRay(float s_time, Vector2f s_uv, Vector2f s_len,
		uint32 x, uint32 y) override;

	PerspectiveCamera* SetFov(float fov);
	PerspectiveCamera* SetLenRadius(float lr);
	PerspectiveCamera* SetNearPlane(float near);
	PerspectiveCamera* SetFarPlane(float far);
	PerspectiveCamera* SetFocusPlane(float focus);
private:
	float fov, lenRadius, near, far, aspectRatio,focusPlane;
	float tanFov;
};