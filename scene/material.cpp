#include "material.h"


Material::Material(BSDF::Ptr bsdf, shared_ptr<AreaLight> emission,
	uint32 textureCount, Texture::Ptr* texture, TEXTURE_TYPE* types):
	bsdf(bsdf),emission(emission) {
	for (uint32 i = 0; i < textureCount; i++) {
		al_assert(types[i] >= TEXTURE_TYPE_NUM, "Material::GetTexture : invalid texture type.out of bondary");
		textures[types[i]] = texture[i];
	}
	al_assert(bsdf == nullptr, "a material's bsdf should not be nullptr");
}

SurfaceIntersection Material::Intersect(const Intersection& isect, Material::Ptr mat) {
	SurfaceIntersection sIsect;
	sIsect.isect = isect;
	sIsect.material = mat;

	if (auto v = mat->GetTexture(TEXTURE_TYPE_NORMALS); v.has_value()) {
		Texture::Ptr normalMap = v.value();
		Vector4f localNormalSample = normalMap->Sample(isect.uv, TEXTURE_SAMPLER_LINEAR);

		Vector3f localNormal = Vector3f(localNormalSample.x, localNormalSample.y, localNormalSample.z);
		localNormal = Math::normalize(Math::vsub(Math::vmul(localNormal, 2.f), Vector3f(1.f, 1.f, 1.f)));

		Vector3f normal = isect.normal;
		Vector3f tangent = isect.tangent;
		Vector3f bitangent = Math::cross(tangent, normal);

		sIsect.shadingNormal = Math::vadd(
			Math::vadd(
				Math::vmul(tangent, localNormal.x),
				Math::vmul(bitangent, localNormal.y)
			),
			Math::vmul(normal, localNormal.z)
		);
		sIsect.shadingNormal = Math::normalize(sIsect.shadingNormal);
	}
	else {
		sIsect.shadingNormal = isect.normal;
	}
	return sIsect;
}


Vector3f Material::SampleBSDF(const SurfaceIntersection& isect, const Vector3f& wo, float* pdf) {
	Vector3f wi = bsdf->Sample(this, isect, wo, pdf);
	return wi;
}

Vector3f   Material::EvaluateBSDF(const Vector3f& wo, const SurfaceIntersection& isect, const Vector3f& wi) {
	Vector3f value = bsdf->Evaluate(this, wo, isect, wi);
	return value;
}

optional<Texture::Ptr>  Material::GetTexture(TEXTURE_TYPE type) {
	al_assert(type >= TEXTURE_TYPE_NUM, "Material::GetTexture : invalid texture type.out of bondary");
	uint32 idx = (uint32)type;
	Texture::Ptr res = textures[idx];
	if (res == nullptr) {
		return {};
	}
	return res;
}

bool BSDF::SetParameterByName(const char* name,const char* type,const void* data) {
	const rtti::VariableInfo* info = pStruct->GetVariable(name);
	if (info == nullptr || strcmp(type,info->typeName)) {
		return false;
	}
	void* dst = info->Get(this->data);
	copy_memory_s(dst, data, pStruct->GetSize() * sizeof(float));
	return true;
}


BSDF::BSDF(rtti::Struct* pStruct, void* data):pStruct(pStruct),
	data(data) {
	al_assert(pStruct != nullptr, "BSDF::BSDF : the struct of bsdf's data should not be nullptr");
	al_assert(data != nullptr, "BSDF::BSDF : the bsdf's data should not be nullptr");
}
