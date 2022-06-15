#pragma once
#include "common/common.h"
#include "math/math.h"

enum TEXTURE_SAMPLER {
	TEXTURE_SAMPLER_LOWER,
	TEXTURE_SAMPLER_LINEAR
};


//dimension should be 3 or 4
class Texture{
public:
	static ptr<Texture> Load(const String& path);
	
	uint32 GetWidth() const { return width; }
	uint32 GetHeight() const { return height; }
	uint32 GetDimension() const { return dim; }

	uint32 At(uint32 x, uint32 y) const;
	Vector4f Sample(Vector2f uv, TEXTURE_SAMPLER sampler);

	~Texture();
private:
	Texture(uint8* bitMap,uint32 width,
		uint32 height,uint32 dim);

	uint8* textureBitMap;
	uint32 width, height,dim;

};