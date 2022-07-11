#pragma once
#include "common/common.h"
#include "math/math.h"

enum TEXTURE_SAMPLER {
	TEXTURE_SAMPLER_LOWER,
	TEXTURE_SAMPLER_LINEAR
};

enum TEXTURE_DATA_TYPE {
	TEXTURE_DATA_TYPE_8BIT,
	TEXTURE_DATA_TYPE_FLOAT
};


//dimension should be 3 or 4
class Texture{
public:
	static ptr<Texture> Load(const String& path);
	static ptr<Texture> Create(uint32 width,uint32 height,uint32 dim, TEXTURE_DATA_TYPE type);
	
	uint32 GetWidth() const { return width; }
	uint32 GetHeight() const { return height; }
	uint32 GetDimension() const { return dim; }

	Vector4f Sample(const Vector2f& uv, TEXTURE_SAMPLER sampler);
	virtual void Write(uint32 x, uint32 y,const Vector4f& value) = 0;

	virtual ~Texture();
protected:
	Texture(uint32 width,
		uint32 height,uint32 dim);

	virtual Vector4f At(uint32 x, uint32 y) = 0;
	uint32 width, height,dim;

};

class BitTexture : public Texture {
	friend ptr<Texture> Texture::Load(const String& path);
	friend ptr<Texture> Texture::Create(uint32 width, uint32 height, uint32 dim, TEXTURE_DATA_TYPE type);
public:
	virtual void Write(uint32 x, uint32 y, const Vector4f& value) override;
	~BitTexture();
protected:
	BitTexture(uint8* data, uint32 width,
		uint32 height, uint32 dim);

	virtual Vector4f At(uint32 x,uint32 y) override;
	uint8* textureData;
};


class FloatTexture : public Texture {
	friend ptr<Texture> Texture::Load(const String& path);
	friend ptr<Texture> Texture::Create(uint32 width, uint32 height, uint32 dim, TEXTURE_DATA_TYPE type);
public:
	virtual void Write(uint32 x, uint32 y, const Vector4f& value) override;
	~FloatTexture();
protected:
	FloatTexture(float* data,uint32 width,
		uint32 height, uint32 dim);

	virtual Vector4f At(uint32 x, uint32 y) override;
	float* textureData;
};