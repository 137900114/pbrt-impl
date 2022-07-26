#include "material.h"


Material::Material(BSDF::Ptr bsdf, shared_ptr<AreaLight> emission,
	uint32 textureCount, Texture::Ptr* texture, TEXTURE_TYPE* types):
	bsdf(bsdf),emission(emission) {
	for (uint32 i = 0; i < textureCount; i++) {
		al_assert(types[i] >= TEXTURE_TYPE_NUM, "Material::GetTexture : invalid texture type.out of bondary");
		textures[types[i]] = texture[i];
	}
	al_assert(bsdf != nullptr, "a material's bsdf should not be nullptr");
}

SurfaceIntersection Material::Intersect(const Intersection& isect, Material::Ptr mat) {
	SurfaceIntersection sIsect;
	sIsect.isect = isect;
	sIsect.material = mat;

	if (auto v = mat->GetTexture(TEXTURE_TYPE_NORMALS); v.has_value()) {
		Texture::Ptr normalMap = v.value();
		Vector4f localNormalSample = normalMap->Sample(isect.uv, TEXTURE_SAMPLER_LINEAR);

		Vector3f localNormal = Vector3f(localNormalSample.x, localNormalSample.y, localNormalSample.z);
		localNormal = Math::normalize((localNormal * 2.f) - Vector3f(1.f, 1.f, 1.f));

		Vector3f normal = isect.normal;
		Vector3f tangent = isect.tangent;
		Vector3f bitangent = Math::cross(tangent, normal);
		//TODO : encapsulate this to a function
		sIsect.shadingNormal = Math::normalize(tangent* localNormal.x + normal * localNormal.y + bitangent * localNormal.z);
	}
	else {
		sIsect.shadingNormal = isect.normal;
	}
	return sIsect;
}


Vector3f Material::SampleBSDF(const SurfaceIntersection& isect,const Vector2f& seed, const Vector3f& wo, float* pdf) {
	Vector3f wi = bsdf->Sample(this, isect, seed, wo, pdf);
	return wi;
}

Vector3f   Material::EvaluateBSDF(const Vector3f& wo, const SurfaceIntersection& isect, const Vector3f& wi) {
	Vector3f value = bsdf->Evaluate(wo, isect, wi);
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

optional<shared_ptr<AreaLight>> Material::GetEmission() {
	if (emission != nullptr) {
		return emission;
	}
	return {};
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

RTTI_STRUCT(Lambert)
	RTTI_VARIABLE(Vector3f, diffuse);
RTTI_STRUCT_END


//importance sampling lambert bsdf on a cosine distributed hemisphere
Vector3f LambertBSDF::Sample(Material* mat, const SurfaceIntersection& isect,
	const Vector2f& seed, const Vector3f& wo, float* pdf) {
	//generate concentric disk sample
	// r = x
	// theta = y / x * (pi / 4)
	Vector2f p = seed * 2.f - Vector2f(1.f, 1.f);
	float r, theta;
	if (std::abs(p.x) > std::abs(p.y)) {
		r = p.x;
		theta = p.y / p.x * (Math::pi / 4.f);
	}
	else {
		r = p.y;
		theta = (Math::pi / 2.f) - (Math::pi / 4.f) * p.x / p.y;
	}
	//map it to sphere
	Vector3f rv;
	rv.x = r * cosf(theta);
	rv.y = r * sinf(theta);
	rv.z = sqrtf(1.f - r * r);
	return rv;
}

Vector3f LambertBSDF::Evaluate( const Vector3f& wo, const SurfaceIntersection& isect,
	const Vector3f& wi) {
	Material::Ptr mat = isect.material;
	Vector3f diffuse = this->diffuse;
	
	if (auto v = mat->GetTexture(TEXTURE_TYPE_DIFFUSE);
		v.has_value()) {
		Texture::Ptr tex = v.value();
		Vector3f texDiffuse = tex->Sample(isect.isect.uv, TEXTURE_SAMPLER_LINEAR).XYZ();
		diffuse = Math::color_blend(diffuse, texDiffuse);
	}
	 
	return diffuse / Math::pi;
}

LambertBSDF::LambertBSDF(const Vector3f& diffuse):
	BSDF(new Lambert,&this->diffuse),diffuse(diffuse) {}


String LambertBSDF::ToString() {
	String s = String(AL_STR("Lambert BSDF : diffuse (")) + AL_TO_STR(diffuse.x) + 
		AL_STR(",") + AL_TO_STR(diffuse.y) + AL_STR(",") +  AL_TO_STR(diffuse.z) + 
		AL_STR(")");
	return s;
}