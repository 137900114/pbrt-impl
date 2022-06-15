#include "Model.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <filesystem>
namespace fs = std::filesystem;

ptr<Mesh> Model::GetMesh(uint32 i) {
	al_assert(i < meshs.size(), "file-{0}:line-{1} index of mesh {2} is out of boundary (0~{3})",
		__FILE__,__LINE__,i,meshs.size());
	return meshs[i];
}

Material Model::GetMaterial(uint32 i) {
	al_assert(i < materials.size(), "file-{0}:line-{1} index of material {2} is out of boundary (0~{3})",
		__FILE__, __LINE__, i, materials.size());
	return materials[i];
}

ptr<Texture> Model::GetTexture(uint32 i) {
	al_assert(i < textures.size(), "file-{0}:line-{1} index of texture {2} is out of boundary (0~{3})",
		__FILE__, __LINE__, i, textures.size());
	return textures[i];
}

uint32 Model::GetMeshMaterialIndex(uint32 i) {
	al_assert(i < meshMaterialIndices.size(), "file-{0}:line-{1} index of mesh {2} is out of boundary (0~{3})",
		__FILE__, __LINE__, i, meshMaterialIndices.size());
	return meshMaterialIndices[i];
}


static void processAiNode(vector<ptr<Mesh>>& meshs,vector<uint32> materialIndex, aiNode* node, const aiScene* scene) {
	for (size_t i = 0; i != node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		std::vector<Vertex> vertices;
		std::vector<uint32>		  indices;

		vertices.resize(mesh->mNumVertices);
		for (size_t j = 0; j != mesh->mNumVertices; j++) {
			Vertex v;
			v.position = Vector3f(mesh->mVertices[j].x, mesh->mVertices[j].y,
				mesh->mVertices[j].z);
			v.normal = Vector3f(mesh->mNormals[j].x, mesh->mNormals[j].y,
				mesh->mNormals[j].z);
			if (mesh->mTangents == nullptr) {
				v.tangent = Vector3f(0., 1., 0.);
			}
			else {
				v.tangent = Vector3f(mesh->mTangents[j].x, mesh->mTangents[j].y,
					mesh->mTangents[j].z);
			}
			if (mesh->mTextureCoords[0] == nullptr) {
				v.uv = Vector2f(0., 0.);
			}
			else {
				v.uv = Vector2f(mesh->mTextureCoords[0][j].x
					, mesh->mTextureCoords[0][j].y);
			}
			vertices[j] = v;
		}

		for (size_t j = 0; j != mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			for (size_t k = 0; k != face.mNumIndices; k++) {
				indices.push_back(face.mIndices[k]);
			}
		}

		meshs.push_back(ptr<Mesh>(al_new(Mesh,vertices,indices)));
		materialIndex.push_back(mesh->mMaterialIndex);
	}
	for (size_t i = 0; i != node->mNumChildren; i++) {
		processAiNode(meshs,materialIndex, node->mChildren[i], scene);
	}
}

static ptr<Model> LoadByAssimp(const String& pathName) {
	Assimp::Importer imp;

	FILE* f;
	if (errno_t error = _wfopen_s(&f, pathName.c_str(), L"rb"); error != 0) {
		al_log("Model::Load : fail to open file {0} , reason {1}", WideString2String(pathName), error);
		return nullptr;
	}
	fseek(f, 0, SEEK_END);
	uint32 fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	void* tmpBuffer = al_malloc(fsize);
	fread(tmpBuffer, fsize, 1, f);

	fclose(f);

	const aiScene* scene = imp.ReadFileFromMemory(tmpBuffer,
		fsize,
		aiProcess_GenNormals |
		aiProcess_Triangulate |
		aiProcess_CalcTangentSpace
	);


	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		al_log("Model::Load : fail to load model file {0} reason {1}",WideString2String(pathName), 
			string(imp.GetErrorString())
		);
		return nullptr;
	}

	vector<ptr<Mesh>> 	 meshs;
	vector<uint32>		 meshMaterialIndices;
	vector<Material>  	 materials;
	vector<ptr<Texture>> textures;

	unordered_map<String, uint32>  texturePathMap;

	String dirName = ConvertToString(fs::path(pathName).parent_path().wstring());
	
	for (size_t i = 0; i != scene->mNumMaterials; i++) {

		aiMaterial* mat = scene->mMaterials[i];
		Material material;

		auto loadTexture = [&](aiTextureType type)-> int32 {
			if (mat->GetTextureCount(type) != 0) {
				aiString str;
				mat->GetTexture(type, 0, &str);
				std::wstring texPath = ConvertToString(str.C_Str());
				//if the texture has been loaded
				if (auto iter = texturePathMap.find(texPath);iter != texturePathMap.end()) {
					return iter->second;
				}
				//if it's not a absolute path
				if (fs::path(texPath).is_relative()) {
					texPath = dirName + texPath;
				}
				ptr<Texture> tex = Texture::Load(texPath);
				if (tex == nullptr) {
					al_log("Model::Load : fail to load texture {0}",WideString2String(texPath));
					return -1;
				}
				else {
					int32 index = (int)textures.size();
					texturePathMap[texPath] = index;
					textures.push_back(tex);
					return index;
				}
			}
			else {
				return -1;
			}
		};
		al_for(i, 0, TEXTURE_TYPE_NUM) {
			material.textureIndex[i] = loadTexture((aiTextureType)i);
		}

		for (size_t i = 0; i != mat->mNumProperties; i++) {
			auto prop = mat->mProperties[i];

			float* data = reinterpret_cast<float*>(prop->mData);
			if (strstr(prop->mKey.C_Str(), "diffuse")) {
				material.diffuseColor = Vector3f(data[0],data[1],data[2]);
			}
			else if (strstr(prop->mKey.C_Str(),"specular")) {
				material.specular = Vector3f(data[0], data[1], data[2]);
			}
			else if (strstr(prop->mKey.C_Str(),"roughness")) {
				material.roughness = *data;
			}
			else if (strstr(prop->mKey.C_Str(),"metallic")) {
				material.metallic = *data;
			}
		}

		materials.push_back(material);
	}

	processAiNode(meshs,meshMaterialIndices, scene->mRootNode, scene);

	ptr<Model> model(al_new(Model, meshs, meshMaterialIndices,
		materials, textures));

	return model;
}

static bool supportedByAssimp(const wchar_t* extName) {
	static std::unordered_set<String> extNames = {
		L".dae",L".xml",L".blend",L".bvh",L".3ds",L".ase",
		L".glFT",L".ply",L".dxf",L".ifc",L".nff",L".smd",
		L".vta",L".mdl",L".md2",L".md3",L".pk3",L".mdc",L".md5anim",
		L".md5camera",L".x",L".q3o",L".q3s",L".raw",L".ac",L".stl",
		L".dxf",L".irrmesh",L".irr",L".off",L".ter",L".mdl",L".hmp",
		L".mesh.xml",L".skeleton.xml",L".material",L".ms3d",L".lwo",
		L".lws",L".lxo",L".csm",L".cob",L".scn",L".fbx"
	};

	if (extNames.find(extName) != extNames.end()) {
		return true;
	}
	return false;
}


ptr<Model> Model::Load(const String& path) {

	fs::path p = path;
	String extName = ConvertToString(p.extension().wstring());
	
	if (supportedByAssimp(extName.c_str())) {
		return LoadByAssimp(path);
	}
	else {
		al_log("Model::Load : file {0} 's extension is not supported", WideString2String(path));
	}
	return nullptr;
}

Mesh::Mesh(const vector<Vertex>& vertices,
	const  vector<uint32>& indices):vertices(vertices),indices(indices){

}

Model::Model(const vector<ptr<Mesh>>& meshs,
	const vector<uint32>& meshMaterialIndices,
	const vector<Material>& materials,
	const vector<ptr<Texture>>& textures):
	meshs(meshs),meshMaterialIndices(meshMaterialIndices),
	materials(materials),textures(textures){ }