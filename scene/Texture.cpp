#include "Texture.h"
#include "stb/stb_image.h"

Texture::~Texture() {}

Texture::Texture( uint32 width,uint32 height, uint32 dim):
	width(width),height(height),dim(dim){
	al_assert(dim == 3 || dim == 4,"Texture::Texture the color clannel of texture must be 3 or 4 rather than {0}",dim);
}

#include <filesystem>
namespace fs = std::filesystem;

static bool SupportedBySTBBitData(const fs::path& _ext) {
	std::wstring extName = _ext.extension().wstring();
	return extName == L".png" || extName == L".bmp" || extName == L".jpg" || extName == L".tga";
}

static bool SupportedBySTBFloatData(const fs::path& _ext) {
	std::wstring extName = _ext.extension().wstring();
	return extName == L".hdr";
}

static std::tuple<void*, int,int,int> LoadRawDataBySTB(const wchar_t* filepath, bool filp_vertically) {
	void* data = nullptr;
	int height, width,comp;
	stbi_set_flip_vertically_on_load(filp_vertically);
	FILE* target_file = nullptr;
	if (errno_t error = _wfopen_s(&target_file, filepath, L"rb"); error != 0) {
		al_log("Texture::Load : fail to open file {0} error {1}", WideString2String(filepath),error);
		return std::move(std::make_tuple(nullptr, -1, -1,-1));
	}
	data = stbi_load_from_file(target_file, &width, &height, &comp, 0);
	fclose(target_file);
	return std::move(std::make_tuple(data, width, height,comp));
}

ptr<Texture> Texture::Load(const String& path) {
	fs::path p = path;
	if (!fs::exists(p)) {
		al_log("Texture::Load : file path {0} doesn't exists",WideString2String(path));
		return nullptr;
	}
	if (SupportedBySTBBitData(p)) {
		auto [data, width, height, comp] = LoadRawDataBySTB(path.c_str(), false);
		if (!data) return nullptr;
		ptr<Texture> texture(new BitTexture((uint8*)data, (uint32)width, (uint32)height, (uint32)comp));
		return texture;
	}
	if (SupportedBySTBFloatData(p)) {
		auto [data, width, height, comp] = LoadRawDataBySTB(path.c_str(), false);
		if (!data) return nullptr;
		ptr<Texture> texture(new FloatTexture((float*)data, (uint32)width, (uint32)height, (uint32)comp));
		return texture;
	}

	al_log("Texture::Load : file {0} 's extension {1} is not supported", WideString2String(path), p.extension().string());
	return nullptr;
}



ptr<Texture> Texture::Create(uint32 width, uint32 height, uint32 dim, TEXTURE_DATA_TYPE type){
	switch (type) {
	case TEXTURE_DATA_TYPE_8BIT:
		{
			uint32 size = width * height * dim * sizeof(uint8);
			void* data = al_malloc(size); 
			al_assert(data != nullptr, "out of memory");
			zero_memory_s(data, size);
			ptr<Texture> texture(new BitTexture((uint8*)data, width, height, dim));
			return texture;
		}
	case TEXTURE_DATA_TYPE_FLOAT:
		{
			uint32 size = width * height * dim * sizeof(float);
			void* data = al_malloc(size);
			al_assert(data != nullptr, "out of memory");
			zero_memory_s(data, size);
			ptr<Texture> texture(new FloatTexture((float*)data, width, height, dim));
			return texture;
		}
	}
	al_assert(false, "invalid texture type {}",type);
	return nullptr;
}

static Vector4f u32ToVector4(uint32 v) {
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

static uint8 floatToU8(float f) {
	f = Math::clamp((f * 256.f), 0.f, 256.f - 1e-4f);
	return (uint8)f;
}

static Vector4f Interpolate(const Vector4f& v1,const Vector4f& v2,float blend) {
	return Math::vadd(Math::vmul(v1, 1.f - blend), Math::vmul(v2, blend));
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

void BitTexture::Write(uint32 x, uint32 y, const Vector4f& value) {
	al_assert(x < width&& y < height, "BitTexture::Write : index out of bondary ({},{}).Texture's size ({},{})",
		x, y, width, height);
	uint32 offset = (x + y * width) * dim;

	textureData[offset + 0] = floatToU8(value.x);
	textureData[offset + 1] = floatToU8(value.y);
	textureData[offset + 2] = floatToU8(value.z);
	if (dim == 4)textureData[offset + 3] = floatToU8(value.w);
}

void FloatTexture::Write(uint32 x, uint32 y, const Vector4f& value) {
	al_assert(x < width&& y < height, "BitTexture::Write : index out of bondary ({},{}).Texture's size ({},{})",
		x, y, width, height);
	uint32 offset = (x + y * width) * dim;

	textureData[offset + 0] = value.x;
	textureData[offset + 1] = value.y;
	textureData[offset + 2] = value.z;
	if (dim == 4)textureData[offset + 3] = value.w;
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