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

enum FILM_AA {
	FILM_AA_NONE,
	//AA_GAUSSIAN
};


class BitTexture;
class FloatTextture;

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
	al_add_ptr_t(BitTexture);

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
	al_add_ptr_t(FloatTexture);
	~FloatTexture();
protected:
	FloatTexture(float* data,uint32 width,
		uint32 height, uint32 dim);

	virtual Vector4f At(uint32 x, uint32 y) override;
	float* textureData;
};

using ToneMapping = function<Vector3f(const Vector3f&, float)>;
Vector3f CEToneMapping(const Vector3f& value,float lum);

class Film : public FloatTexture {
public:
	al_add_ptr_t(Film);
	Film(uint32 width, uint32 height, FILM_AA aa);

	void Write(uint32 x, uint32 y, const Vector3f& value);
	bool Save(const String& path);
	void SetAAStrategy(FILM_AA aa);
	void SetToneMappingAlgorithm(ToneMapping toneMapping);
	void SetExposure(float exp);
	
	~Film();
private:
	float*  sampleWeightData;
	FILM_AA aaStrategy;
	ToneMapping toneMapping;
	float   exposure;
};