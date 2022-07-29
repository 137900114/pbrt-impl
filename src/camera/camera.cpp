#include "camera/camera.h"

Camera::Camera(uint32 width, uint32 height, const Transform& trans):
	width(width),height(height),transform(trans),
	film(new Film(width,height,FILM_AA_NONE)) {
	al_assert(width != 0 , "Camera::Camera : width of the camera's film should not be 0");
	al_assert(height != 0, "Camera::Camera : height of the camera's film should not be 0");
}


void Camera::WriteFilm(Vector3f color, uint32 x, uint32 y) {
	film->Write(x, y, color);
}

Transform& Camera::GetTransform() {
	return transform;
}

void Camera::SetAAStrategy(FILM_AA aa) {
	film->SetAAStrategy(aa);
}

void Camera::SetToneMappingAlgorithm(ToneMapping toneMapping) {
	film->SetToneMappingAlgorithm(toneMapping);
}

void Camera::SetExposure(float exp) {
	film->SetExposure(exp);
}

bool Camera::SaveFilm(const String& path) {
	return film->Save(path);
}
