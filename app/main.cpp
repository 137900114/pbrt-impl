#include "common/common.h"
#include "sampler/halton.h"

#include <iostream>

RTTI_STRUCT(CameraSample)
	rtti::Variable<Vector2f> len = "len";
	rtti::Variable<Vector2f> pfilm = "film";
	rtti::Variable<float>	 time = "time";
};

int main(){
	al_log_initialize();



	al_log_finalize();
}