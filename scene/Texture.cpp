#include "Texture.h"
#include "stb/stb_image.h"

Texture::~Texture() {
	if (textureBitMap) {
		al_mfree(textureBitMap);
		textureBitMap = nullptr;
	}
}

Texture::Texture(uint8* bitMap, uint32 width,uint32 height, uint32 dim):
	textureBitMap(bitMap),width(width),height(height),dim(dim){
	al_assert(dim == 3 || dim == 4,"Texture::Texture the color clannel of texture must be 3 or 4 rather than {0}",dim);
}

#include <filesystem>
namespace fs = std::filesystem;

static bool SupportedBySTB(const fs::path& _ext) {
	std::wstring extName = _ext.extension().wstring();
	return extName == L".png" || extName == L".bmp" || extName == L".jpg" || extName == L".tga";
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
	if (!SupportedBySTB(p)) {
		al_log("Texture::Load : file {0} 's extension {1} is not supported",WideString2String(path),p.extension().string());
		return nullptr;
	}

	auto [data,width,height,comp] = LoadRawDataBySTB(path.c_str(), false);
	if (!data) return nullptr;
	ptr<Texture> texture(new Texture((uint8*)data,(uint32)width,(uint32)height,(uint32)comp));
	return texture;
}

uint32 Texture::At(uint32 x, uint32 y) const {
	uint32 result = 0;
	uint32 offset = (x + y * width) * dim;
	result |= textureBitMap[offset + 0];
	result |= textureBitMap[offset + 1] << 8;
	result |= textureBitMap[offset + 2] << 16;
	if (dim == 4) result |= textureBitMap[offset + 3] << 24;
	return result;
}

static Vector4f u32ToVector4(uint32 v) {
	return Vector4f(
		(float)(v		  & 0xff) / 256.f,
		(float)((v >> 8)  & 0xff) / 256.f,
		(float)((v >> 16) & 0xff) / 256.f,
		(float)((v >> 24) & 0xff) / 256.f
	);
}

static Vector4f Interpolate(const Vector4f& v1,const Vector4f& v2,float blend) {
	return Math::vadd(Math::vmul(v1, 1.f - blend), Math::vmul(v2, blend));
}

Vector4f Texture::Sample(Vector2f uv, TEXTURE_SAMPLER sampler) {
	uv.x = Math::clamp(uv.x, 0.f, 1.f), uv.y = Math::clamp(uv.y, 0.f, 1.f);
	uv = Vector2f(uv.x * (float)width, uv.y * (float)height);
	uint32 x = (uint32)uv.x, y = (uint32)uv.y;
	switch (sampler) {
	case TEXTURE_SAMPLER_LOWER:
		{
			return u32ToVector4(At(x, y));
		}
	case TEXTURE_SAMPLER_LINEAR:
		{
			Vector4f v0 = u32ToVector4(At(x		, y		)),
					 v1 = u32ToVector4(At(x + 1 , y		)),
					 v2 = u32ToVector4(At(x		, y + 1	)),
					 v3 = u32ToVector4(At(x + 1 , y + 1	));
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