#include "texture.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

Texture::~Texture() {}

Texture::Texture( uint32 width,uint32 height, uint32 dim):
	width(width),height(height),dim(dim){
	al_assert(dim == 3 || dim == 4,"Texture::Texture the color clannel of texture must be 3 or 4 rather than {0}",dim);
}

#include <filesystem>
namespace fs = std::filesystem;

AL_PRIVATE bool SupportedBySTBBitData(const fs::path& _ext) {
	std::string extName = _ext.extension().string();
	return extName == ".png" || extName == ".bmp" || extName == ".jpg" || extName == ".tga";
}

AL_PRIVATE bool SupportedBySTBFloatData(const fs::path& _ext) {
	std::string extName = _ext.extension().string();
	return extName == ".hdr";
}

AL_PRIVATE std::tuple<void*, int,int,int> LoadRawDataBySTB(const wchar_t* filepath, bool filp_vertically) {
	void* data = nullptr;
	int height, width,comp;
	stbi_set_flip_vertically_on_load(filp_vertically);
	FILE* target_file = nullptr;
	if (errno_t error = _wfopen_s(&target_file, filepath, L"rb"); error != 0) {
		al_log("Texture::Load : fail to open file {0} error {1}", ConvertToNarrowString(filepath),error);
		return std::move(std::make_tuple(nullptr, -1, -1,-1));
	}
	data = stbi_load_from_file(target_file, &width, &height, &comp, 0);
	fclose(target_file);
	return std::move(std::make_tuple(data, width, height,comp));
}

Texture::Ptr Texture::Load(const String& path) {
	fs::path p = path;
	if (!fs::exists(p)) {
		al_log("Texture::Load : file path {0} doesn't exists",ConvertToNarrowString(path));
		return nullptr;
	}
	if (SupportedBySTBBitData(p)) {
		auto [data, width, height, comp] = LoadRawDataBySTB(path.c_str(), false);
		if (!data) return nullptr;
		Texture::Ptr texture(new BitTexture((uint8*)data, (uint32)width, (uint32)height, (uint32)comp));
		return texture;
	}
	if (SupportedBySTBFloatData(p)) {
		auto [data, width, height, comp] = LoadRawDataBySTB(path.c_str(), false);
		if (!data) return nullptr;
		Texture::Ptr texture(new FloatTexture((float*)data, (uint32)width, (uint32)height, (uint32)comp));
		return texture;
	}

	al_log("Texture::Load : file {0} 's extension {1} is not supported", ConvertToNarrowString(path), p.extension().string());
	return nullptr;
}



Texture::Ptr Texture::Create(uint32 width, uint32 height, uint32 dim, TEXTURE_DATA_TYPE type){
	switch (type) {
	case TEXTURE_DATA_TYPE_8BIT:
		{
			uint32 size = width * height * dim * sizeof(uint8);
			void* data = al_malloc(size); 
			al_assert(data != nullptr, "out of memory");
			zero_memory_s(data, size);
			Texture::Ptr texture(new BitTexture((uint8*)data, width, height, dim));
			return texture;
		}
	case TEXTURE_DATA_TYPE_FLOAT:
		{
			uint32 size = width * height * dim * sizeof(float);
			void* data = al_malloc(size);
			al_assert(data != nullptr, "out of memory");
			zero_memory_s(data, size);
			Texture::Ptr texture(new FloatTexture((float*)data, width, height, dim));
			return texture;
		}
	}
	al_assert(false, "invalid texture type {}",type);
	return nullptr;
}

AL_PRIVATE Vector4f u32ToVector4(uint32 v) {
	return Vector4f(
		(float)(v & 0xff) / 256.f,
		(float)((v >> 8) & 0xff) / 256.f,
		(float)((v >> 16) & 0xff) / 256.f,
		(float)((v >> 24) & 0xff) / 256.f
	);
}

Vector4f BitTexture::At(uint32 x, uint32 y) {
	uint32 result = 0;
	uint32 offset = (x + y * width) * dim;
	result |= textureData[offset + 0];
	result |= textureData[offset + 1] << 8;
	result |= textureData[offset + 2] << 16;
	if (dim == 4) result |= textureData[offset + 3] << 24;
	return u32ToVector4(result);
}

AL_PRIVATE uint8 floatToU8(float f) {
	f = Math::clamp((f * 256.f), 0.f, 256.f - 1e-4f);
	return (uint8)f;
}

AL_PRIVATE Vector4f Interpolate(const Vector4f& v1,const Vector4f& v2,float blend) {
	return v1 * (1.f - blend) +  v2 * blend;
}

Vector4f Texture::Sample(const Vector2f& _uv, TEXTURE_SAMPLER sampler) {
	Vector2f uv = _uv;
	uv.x = Math::clamp(uv.x, 0.f, 1.f), uv.y = Math::clamp(uv.y, 0.f, 1.f);
	uv = Vector2f(uv.x * (float)width, uv.y * (float)height);
	uint32 x = (uint32)uv.x, y = (uint32)uv.y;
	switch (sampler) {
	case TEXTURE_SAMPLER_LOWER:
		{
			return At(x, y);
		}
	case TEXTURE_SAMPLER_LINEAR:
		{
			Vector4f v0 = At(x		, y		),
					 v1 = At(x + 1 , y		),
					 v2 = At(x		, y + 1	),
					 v3 = At(x + 1 , y + 1	);
			float bx = Math::frac(uv.x), by = Math::frac(uv.y);
			return Interpolate(
				Interpolate(v0, v1, bx),
				Interpolate(v2, v3, bx),
				by
			);
		}
	}
	al_assert(false , "Texture::Sample : invalid texture sampler type");
	return Vector4f();
}

Vector4f FloatTexture::At(uint32 x, uint32 y) {
	Vector4f result;
	uint32 offset = (x + y * width) * dim;
	result.x = textureData[offset + 0];
	result.y = textureData[offset + 1];
	result.z = textureData[offset + 2];
	if (dim == 4) result.w = textureData[offset + 3];
	return result;
}

FloatTexture::FloatTexture(float* data, uint32 width,
	uint32 height, uint32 dim):Texture(width,height,dim),
	textureData(data){
	al_assert(data != nullptr, "FloatTexture::FloatTexture : data should not be nullptr");
}

BitTexture::BitTexture(uint8* data, uint32 width,
	uint32 height, uint32 dim) : Texture(width, height, dim),
	textureData(data){
	al_assert(data != nullptr, "BitTexture::BitTexture : data should not be nullptr");
}

FloatTexture::~FloatTexture() {
	al_mfree(textureData);
}

BitTexture::~BitTexture() {
	al_mfree(textureData);
}

struct TextureWriteFuncContext {
	FILE* pFile;
};

AL_PRIVATE void TextureWriteFunc(void* context,void* data,int size) {
	FILE* pFile = ((TextureWriteFuncContext*)context)->pFile;
	fwrite(data, size, 1, pFile);
}

template<typename T>
T* ZeroMalloc(uint32 width,uint32 height,uint32 dim) {
	uint32 size = width * height * dim * sizeof(T);
	void* data = al_malloc(size);
	al_assert(data != nullptr, "out of memory");
	zero_memory_s(data, size);
	return (T*)data;
}

bool Film::Save(const String& _path) {
	FILE* targetFile = nullptr;
	
	//get extension
	fs::path path(_path);
	String extName = ConvertFromNarrowString(path.extension().string());

	if (errno_t error = _wfopen_s(&targetFile, _path.c_str(), L"wb"); error != 0) {
		al_log("Texture::Load : fail to open file {0} error {1}", ConvertToNarrowString(_path), error);
		return false;
	}
	TextureWriteFuncContext ctx;
	ctx.pFile = targetFile;

	uint8* writeData = ZeroMalloc<uint8>(width, height, 3);
	al_for(x,0,width) {
		al_for(y,0,height) {
			uint32 offset = (x + y * width) * 3;
			Vector3f v(textureData[offset], textureData[offset + 1], textureData[offset + 2]);
			if (sampleWeightData[x + y * width] != 0)
				v = v / sampleWeightData[x + y * width];
			v = Math::vmin(Vector3f(255.9f, 255.9f, 255.9f), toneMapping(v, exposure) * 256.f);

			writeData[offset] = (uint8)v.x;
			writeData[offset + 1] = (uint8)v.y;
			writeData[offset + 2] = (uint8)v.z;
		}
	}

	int rv = 0;
	if (extName == AL_STR(".png")) {
		rv = stbi_write_png_to_func(TextureWriteFunc, &ctx, width, height, dim, writeData, 0);
	}
	else if (extName == AL_STR(".jpg")) {
		rv = stbi_write_jpg_to_func(TextureWriteFunc, &ctx, width, height, dim, writeData, 0);
	}
	else if (extName == AL_STR(".bmp")) {
		//default quality
		rv = stbi_write_bmp_to_func(TextureWriteFunc, &ctx, width, height, dim, writeData);
	}else if(extName == AL_STR(".tga")){
		rv = stbi_write_tga_to_func(TextureWriteFunc, &ctx, width, height, dim, writeData);
	}
	else {
		rv = 0;
		al_warn("fail to save texture {} invalid extension name {}",ConvertToNarrowString(_path),
			ConvertToNarrowString(extName));
	}

	return rv != 0;
}

void Film::SetAAStrategy(FILM_AA aa) {
	aaStrategy = aa;
	//TODO implement other strategies
}


Film::Film(uint32 width, uint32 height, FILM_AA aa)
:FloatTexture(ZeroMalloc<float>(width,height,3), width, height, 3),
 aaStrategy(aa) {
	sampleWeightData = ZeroMalloc<float>(width, height, 1);
	exposure = 1.f;
	toneMapping = CEToneMapping;
}

void Film::Write(uint32 x, uint32 y, const Vector3f& value) {
	al_assert(x < width&& y < height, "Film::Write : index out of bondary ({},{}).Texture's size ({},{})",
		x, y, width, height);
	
	//TODO implement other strategies
	uint32 offset = (x + y * width) * 3;
	textureData[offset] += value.x;
	textureData[offset + 1] += value.y;
	textureData[offset + 2] += value.z;
	
	offset = x + y * width;
	sampleWeightData[offset] += 1;
}


void Film::SetToneMappingAlgorithm(ToneMapping toneMapping) {
	this->toneMapping = toneMapping;
}

Film::~Film() {
	al_mfree(sampleWeightData);
}

Vector3f CEToneMapping(const Vector3f& value, float lum) {
	return Vector3f::I - Vector3f(
		expf(- lum * value.x),
		expf(- lum * value.y),
		expf(- lum * value.z)
	);
}

Vector3f ClampToneMapping(const Vector3f& value, float lum) {
	return Math::clamp(value, Vector3f(0.f, 0.f, 0.f), Vector3f(1.0f, 1.0f, 1.0f));
}

void Film::SetExposure(float exp) {
	if (exp > 0.f) {
		this->exposure = exp;
	}
	else {
		al_warn("Film::SetExposure : exposure should not be less than 0");
	}
}