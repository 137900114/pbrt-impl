#include "PointLight.h"

PointLight::PointLight(Vector3f position,const Vector3f& intensity) :
	Light(LIGHT_TYPE_POINT, intensity) {

}
