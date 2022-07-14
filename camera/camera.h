#pragma once
#include "common/common.h"
#include "scene/Texture.h"
#include "common/rtti.h"

RTTI_STRUCT(CameraSample)
	RTTI_VARIABLE(float, time);
	RTTI_VARIABLE(Vector2f, uv);
	RTTI_VARIABLE(Vector2f, len);
RTTI_STRUCT_END


//TODO add anit aliase
class Camera {
public:
	al_add_ptr_t(Camera);
	Camera(uint32 width, uint32 height,const Transform& trans);
	
	BitTexture::Ptr GetFilm() { return film; }
	virtual Ray     GenerateRay(float s_time,Vector2f s_uv,Vector2f s_len,
		uint32 x, uint32 y) = 0;

	//guassian filter
	//Camera* GaussAntiAlise(uint32 radius);
	Camera* DisableAntiAlise();

	void WriteFilm(Vector3f color,uint32 x,uint32 y);
	Transform& GetTransform();

	virtual ~Camera() {}
protected:
	BitTexture::Ptr film;
	uint32 width, height;
	Transform transform;

	enum AA {
		AA_NONE,
		//AA_GAUSSIAN
	} antiAlise;
	//uint32 gaussRadius;
	//vector<float> guassKernel;
};
