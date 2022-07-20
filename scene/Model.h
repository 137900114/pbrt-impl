#pragma once
#include "math/math.h"
#include "common/common.h"
#include "texture.h"
#include "material.h"
#include "scene_primitive.h"


class AreaLight;


class Mesh {
	friend class Model;
public:
	al_add_ptr_t(Mesh);

	Mesh(const vector<Vertex>& vertices,
		const  vector<uint32>& indices);

	const vector<Vertex>& GetVertices() { return vertices; }
	const vector<uint32>& GetIndices() { return indices; }
private:
	vector<Vertex> vertices;
	vector<uint32> indices;
};

class Model {
public:
	al_add_ptr_t(Model);

	Model(const vector<Mesh::Ptr>& meshs,
		const vector<uint32>& meshMaterialIndices,
		const vector<Material::Ptr>& materials,
		const vector<Texture::Ptr>& textures,
		const vector<shared_ptr<AreaLight>>& lights);

	static Ptr Load(const String& path);

	uint32 MeshCount() { return meshs.size(); }
	Mesh::Ptr GetMesh(uint32 i);
	uint32 GetMeshMaterialIndex(uint32 i);

	uint32 MaterialCount() { return materials.size(); }
	Material::Ptr GetMaterial(uint32 i);

	uint32 TextureCount() { return textures.size(); }
	Texture::Ptr GetTexture(uint32 i);

	vector<ScenePrimitiveInfo> GenerateScenePrimitiveInfos(
		const Transform& trans);
private:
	vector<Mesh::Ptr> 	 meshs;
	vector<uint32>		 meshMaterialIndices;
	vector<Material::Ptr>  	 materials;
	vector<Texture::Ptr> textures;
	vector<shared_ptr<AreaLight>> emissionLights;
};