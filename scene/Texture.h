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
	using Ptr = std::shared_ptr<Texture>;


	static Ptr Load(const String& path);
	static Ptr Create(uint32 width,uint32 height,uint32 dim, TEXTURE_DATA_TYPE type);
	
	uint32 GetWidth() const { return width; }
	uint32 GetHeight() const { return height; }
	uint32 GetDimension() const { return dim; }

	Vector4f Sample(const Vector2f& uv, TEXTURE_SAMPLER sampler);
	virtual void Write(uint32 x, uint32 y,const Vector4f& value) = 0;
	virtual Vector4f At(uint32 x, uint32 y) = 0;

	virtual ~Texture();
protected:
	Texture(uint32 width,
		uint32 height,uint32 dim);

	uint32 width, height,dim;

};

class BitTexture : public Texture {
	friend Ptr Texture::Load(const String& path);
	friend Ptr Texture::Create(uint32 width, uint32 height, uint32 dim, TEXTURE_DATA_TYPE type);
public:
	virtual void Write(uint32 x, uint32 y, const Vector4f& value) override;

	bool Save(const String& path);

	~BitTexture();
protected:
	BitTexture(uint8* data, uint32 width,
		uint32 height, uint32 dim);

	virtual Vector4f At(uint32 x,uint32 y) override;
	uint8* textureData;
};


class FloatTexture : public Texture {
	friend Ptr Texture::Load(const String& path);
	friend Ptr Texture::Create(uint32 width, uint32 height, uint32 dim, TEXTURE_DATA_TYPE type);
public:
	virtual void Write(uint32 x, uint32 y, const Vector4f& value) override;
	~FloatTexture();
protected:
	FloatTexture(float* data,uint32 width,
		uint32 height, uint32 dim);

	virtual Vector4f At(uint32 x, uint32 y) override;
	float* textureData;
};