#pragma once
#include "common/common.h"
#include "math/math.h"
#include "scene/texture.h"
#include "common/rtti.h"

struct SurfaceIntersection;
class AreaLight;
class Material;

//copied from assimp
enum TEXTURE_TYPE
{
	/** Dummy value.
	*
	*  No texture, but the value to be used as 'texture semantic'
	*  (#aiMaterialProperty::mSemantic) for all material properties
	*  *not* related to textures.
	*/
	TEXTURE_TYPE_NONE = 0x0,

	/** The texture is combined with the result of the diffuse
	*  lighting equation.
	*/
	TEXTURE_TYPE_DIFFUSE = 0x1,

	/** The texture is combined with the result of the specular
	*  lighting equation.
	*/
	TEXTURE_TYPE_SPECULAR = 0x2,

	/** The texture is combined with the result of the ambient
	*  lighting equation.
	*/
	TEXTURE_TYPE_AMBIENT = 0x3,

	/** The texture is added to the result of the lighting
	*  calculation. It isn't influenced by incoming light.
	*/
	TEXTURE_TYPE_EMISSIVE = 0x4,

	/** The texture is a height map.
	*
	*  By convention, higher gray-scale values stand for
	*  higher elevations from the base height.
	*/
	TEXTURE_TYPE_HEIGHT = 0x5,

	/** The texture is a (tangent space) normal-map.
	*
	*  Again, there are several conventions for tangent-space
	*  normal maps. Assimp does (intentionally) not
	*  distinguish here.
	*/
	TEXTURE_TYPE_NORMALS = 0x6,

	/** The texture defines the glossiness of the material.
	*
	*  The glossiness is in fact the exponent of the specular
	*  (phong) lighting equation. Usually there is a conversion
	*  function defined to map the linear color values in the
	*  texture to a suitable exponent. Have fun.
	*/
	TEXTURE_TYPE_SHININESS = 0x7,

	/** The texture defines per-pixel opacity.
	*
	*  Usually 'white' means opaque and 'black' means
	*  'transparency'. Or quite the opposite. Have fun.
	*/
	TEXTURE_TYPE_OPACITY = 0x8,

	/** Displacement texture
	*
	*  The exact purpose and format is application-dependent.
	*  Higher color values stand for higher vertex displacements.
	*/
	TEXTURE_TYPE_DISPLACEMENT = 0x9,

	/** Lightmap texture (aka Ambient Occlusion)
	*
	*  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
	*  covered by this material property. The texture contains a
	*  scaling value for the final color value of a pixel. Its
	*  intensity is not affected by incoming light.
	*/
	TEXTURE_TYPE_LIGHTMAP = 0xA,

	/** Reflection texture
	*
	* Contains the color of a perfect mirror reflection.
	* Rarely used, almost never for real-time applications.
	*/
	TEXTURE_TYPE_REFLECTION = 0xB,

	/** Unknown texture
	*
	*  A texture reference that does not match any of the definitions
	*  above is considered to be 'unknown'. It is still imported,
	*  but is excluded from any further postprocessing.
	*/
	TEXTURE_TYPE_UNKNOWN = 0xC,

	TEXTURE_TYPE_NUM = TEXTURE_TYPE_UNKNOWN + 1
};

class BSDF {
public:
	al_add_ptr_t(BSDF);
	virtual Vector3f Sample(Material* mat,const SurfaceIntersection& isect,
		const Vector2f& seed,const Vector3f& wo,float* pdf) = 0;
	virtual Vector3f Evaluate(const Vector3f& wo,const SurfaceIntersection& isect,
		const Vector3f& wi) = 0;
	
	bool SetParameterByName(const char* name,const char* type,const void* data);

	template<typename T>
	bool SetParameterByName(const char* name, const T& value) {
		return SetParameterByName(name, rtti::TypeTrait<T>::name, &value);
	}

	//every bsdf is should maintain reflection information of it's data
	BSDF(rtti::Struct* pStruct,void* data);

	virtual String ToString() { return AL_STR("to string for this bsdf is not supported!"); }

private:
	rtti::Struct* pStruct;
	void* data;
};


class Material {
public:
	al_add_ptr_t(Material);
	
	Material(BSDF::Ptr bsdf,shared_ptr<AreaLight> emission,
		uint32 textureCount,Texture::Ptr* texture,TEXTURE_TYPE* types);
	
	optional<shared_ptr<AreaLight>> GetEmission();
	optional<Texture::Ptr>          GetTexture(TEXTURE_TYPE type);
	
	static SurfaceIntersection Intersect(const Intersection& isect,Material::Ptr mat);
	/// <summary>
	/// importance sample a bsdf function
	/// </summary>
	/// <param name="isect">the intersection surface point</param>
	/// <param name="seed">random seed for the sample</param>
	/// <param name="wo">the view direction goes into the surface</param>
	/// <param name="pdf">the posibility density of the sample</param>
	/// <returns>the sampled direction</returns>
	Vector3f SampleBSDF(const SurfaceIntersection& isect,const Vector2f& seed,const Vector3f& wo,float* pdf);
	Vector3f EvaluateBSDF(const Vector3f& wo,const SurfaceIntersection& isect,const Vector3f& wi);

	template<typename T>
	bool SetBSDFParameterByName(const char* name, const T& value) {
		return bsdf->SetParameterByName(name, rtti::TypeTrait<T>::name, &value);
	}
private:
	BSDF::Ptr bsdf;
	shared_ptr<AreaLight> emission;
	Texture::Ptr textures[TEXTURE_TYPE_NUM];
};


struct SurfaceIntersection{
	Intersection  isect;
	Material::Ptr material;
	Vector3f	  shadingNormal;
};

class LambertBSDF : public BSDF {
public:
	virtual Vector3f Sample(Material* mat, const SurfaceIntersection& isect,
		const Vector2f& seed, const Vector3f& wo, float* pdf) override;
	virtual Vector3f Evaluate(const Vector3f& wo, const SurfaceIntersection& isect,
		const Vector3f& wi) override;
	
	LambertBSDF(const Vector3f& diffuse = Vector3f());

	virtual String ToString() override;
private:
	Vector3f diffuse;
};