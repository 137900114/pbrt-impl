#pragma once
#include "common/common.hpp"

//dimension should be 3 or 4
class Texture{
public:
	static ptr<Texture> Load(const String& path);
	
	uint32 GetWidth() { return width; }
	uint32 GetHeight() { return height; }
	uint32 GetDimension() { return dim; }

	uint32 At(uint32 x, uint32 y);

	~Texture();
private:
	Texture(uint8* bitMap,uint32 width,
		uint32 height,uint32 dim);

	uint8* textureBitMap;
	uint32 width, height,dim;

};