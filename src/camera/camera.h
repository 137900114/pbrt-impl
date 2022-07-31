#pragma once
#include "common/common.h"
#include "scene/texture.h"
#include "common/rtti.h"

RTTI_STRUCT(CameraSample)
	RTTI_VARIABLE(float, time);
	RTTI_VARIABLE(Vector2f, uv);
	RTTI_VARIABLE(Vector2f, len);
RTTI_STRUCT_END


class Camera {
public:
	al_add_ptr_t(Camera);
	Camera(uint32 width, uint32 height,const Transform& trans);
	
	virtual Ray     GenerateRay(float s_time,Vector2f s_uv,Vector2f s_len,
		uint32 x, uint32 y) = 0;

	void WriteFilm(Vector3f color,uint32 x,uint32 y);
	Transform& GetTransform();

	void SetAAStrategy(FILM_AA aa);
	void SetToneMappingAlgorithm(ToneMapping toneMapping);
	void SetExposure(float exp);
	bool SaveFilm(const String& path);

	uint32 GetWidth() { return width; }
	uint32 GetHeight() { return height; }

	virtual ~Camera() {}
protected:
	Film::Ptr film;
	uint32 width, height;
	Transform transform;
};
