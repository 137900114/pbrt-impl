#pragma once
#include "math/math.hpp"
#include "common/common.hpp"
#include "Texture.hpp"

struct Vertex {
	Vector3f position;
	Vector3f normal;
	Vector3f tangent;
	Vector2f uv;
};


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



struct Material{
	int32 textureIndex[TEXTURE_TYPE_NUM];

	Vector3f diffuseColor;
	Vector3f specular;
	float metallic, roughness;
};

class Mesh {
	friend class Model;
public:
	const vector<Vertex>& GetVertices() { return vertices; }
	const vector<uint32>& GetIndices() { return indices; }
private:
	Mesh(const vector<Vertex>& vertices,
		const  vector<uint32>& indices);

	vector<Vertex> vertices;
	vector<uint32> indices;
};

class Model {
public:
	static ptr<Model> Load(const String& path);

	uint32 MeshCount() { return meshs.size(); }
	ptr<Mesh> GetMesh(uint32 i);
	uint32 GetMeshMaterialIndex(uint32 i);

	uint32 MaterialCount() { return materials.size(); }
	Material GetMaterial(uint32 i);

	uint32 TextureCount() { return textures.size(); }
	ptr<Texture> GetTexture(uint32 i);

private:
	Model(const vector<ptr<Mesh>>& meshs,
		const vector<uint32>& meshMaterialIndices,
		const vector<Material>& materials,
		const vector<ptr<Texture>>& textures);

	vector<ptr<Mesh>> 	 meshs;
	vector<uint32>		 meshMaterialIndices;
	vector<Material>  	 materials;
	vector<ptr<Texture>> textures;
};