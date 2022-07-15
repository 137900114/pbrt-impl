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


	al_log_finalize();
}