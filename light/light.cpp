#include "light.h"


Light::Light(LIGHT_TYPE type, float intensity):
	type(type),intensity(intensity){
	al_assert(intensity >= 0.f, "Light::Light : the intensity of the light ({})"
		" should be greater than 0", intensity);
}

