#include "common/common.h"
#include "sampler/halton.h"

#include "scene/Texture.h"

#include <iostream>

RTTI_STRUCT(CameraSample)
	RTTI_VARIABLE(Vector2f, len );
	RTTI_VARIABLE(Vector2f, film);
	RTTI_VARIABLE(float   , time);
RTTI_STRUCT_END

int main(){
	al_log_initialize();

	ptr<Texture> tex = Texture::Create(100, 100, 3, TEXTURE_DATA_TYPE_8BIT);
	

	al_log_finalize();
}