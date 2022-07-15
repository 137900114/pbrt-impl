#include "light.h"


Light::Light(LIGHT_TYPE type,const Vector3f& intensity):
	type(type),intensity(intensity){
	al_assert(intensity.x >= 0.f && intensity.y > 0.f 
		&& intensity.z >= 0.f, "Light::Light : the intensity of the light ({},{},{})"
		" should be greater than 0", intensity.x,intensity.y,intensity.z);
}

