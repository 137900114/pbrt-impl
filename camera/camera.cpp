#include "camera/camera.h"

Camera::Camera(uint32 width, uint32 height, const Transform& trans):
	width(width),height(height),transform(trans),antiAlise(AA_NONE) {
	al_assert(width != 0 , "Camera::Camera : width of the camera's film should not be 0");
	al_assert(height != 0, "Camera::Camera : height of the camera's film should not be 0");

	film = std::dynamic_pointer_cast<BitTexture>(
			Texture::Create(width, height, 3, TEXTURE_DATA_TYPE_8BIT)
		);
}


void Camera::WriteFilm(Vector3f color, uint32 x, uint32 y) {
	switch (antiAlise) {
	case AA_NONE: 
		film->Write(x, y, Vector4f(color.x, color.y, color.z, 0.f));
		break;
	}
}

Transform& Camera::GetTransform() {
	return transform;
}

Camera* Camera::DisableAntiAlise() {
	antiAlise = AA_NONE;
	return this;
}

