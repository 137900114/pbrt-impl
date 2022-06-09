#include "Texture.hpp"
#include "stb/stb_image.hpp"

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
		al_log("Texture::Load : fail to open file {0} error {1}", filepath,error);
		return std::move(std::make_tuple(nullptr, -1, -1,-1));
	}
	data = stbi_load_from_file(target_file, &width, &height, &comp, 0);
	fclose(target_file);
	return std::move(std::make_tuple(data, width, height,comp));
}

ptr<Texture> Texture::Load(const String& path) {
	fs::path p = path;
	if (!fs::exists(p)) {
		al_log("Texture::Load : file path {0} doesn't exists",path.c_str());
		return nullptr;
	}
	if (!SupportedBySTB(p)) {
		al_log("Texture::Load : file {0} 's extension {1} is not supported",path.c_str(),p.extension().wstring().c_str());
		return nullptr;
	}

	auto [data,width,height,comp] = LoadRawDataBySTB(path.c_str(), false);
	if (!data) return nullptr;
	ptr<Texture> texture(new Texture((uint8*)data,(uint32)width,(uint32)height,(uint32)comp));
	return texture;
}

uint32 Texture::At(uint32 x, uint32 y) {
	uint32 result = 0;
	uint32 offset = (x + y * width) * dim;
	result |= textureBitMap[offset + 0];
	result |= textureBitMap[offset + 1] << 8;
	result |= textureBitMap[offset + 2] << 16;
	if (dim == 4) result |= textureBitMap[offset + 3] << 24;
	return result;
}