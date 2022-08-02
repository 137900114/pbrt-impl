#include "model.h"

#include "mmd/Pmx.h"
#include "assimp/importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <filesystem>
#include "light/area_light.h"
namespace fs = std::filesystem;

Mesh::Ptr Model::GetMesh(uint32 i) {
	al_assert(i < meshs.size(), "file-{0}:line-{1} index of mesh {2} is out of boundary (0~{3})",
		__FILE__,__LINE__,i,meshs.size());
	return meshs[i];
}

Material::Ptr Model::GetMaterial(uint32 i) {
	al_assert(i < materials.size(), "file-{0}:line-{1} index of material {2} is out of boundary (0~{3})",
		__FILE__, __LINE__, i, materials.size());
	return materials[i];
}

Texture::Ptr Model::GetTexture(uint32 i) {
	al_assert(i < textures.size(), "file-{0}:line-{1} index of texture {2} is out of boundary (0~{3})",
		__FILE__, __LINE__, i, textures.size());
	return textures[i];
}

uint32 Model::GetMeshMaterialIndex(uint32 i) {
	al_assert(i < meshMaterialIndices.size(), "file-{0}:line-{1} index of mesh {2} is out of boundary (0~{3})",
		__FILE__, __LINE__, i, meshMaterialIndices.size());
	return meshMaterialIndices[i];
}


AL_PRIVATE void processAiNode(vector<Mesh::Ptr>& meshs,vector<uint32>& materialIndex, aiNode* node, const aiScene* scene) {
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

		meshs.push_back(Mesh::Ptr(al_new(Mesh,vertices,indices)));
		materialIndex.push_back(mesh->mMaterialIndex);
	}
	for (size_t i = 0; i != node->mNumChildren; i++) {
		processAiNode(meshs,materialIndex, node->mChildren[i], scene);
	}
}

AL_PRIVATE Model::Ptr LoadByAssimp(const String& pathName) {
	Assimp::Importer imp;

	FILE* f;
	if (auto v = OpenFile(pathName, AL_STR("rb"));v.has_value()) {
		f = v.value();
	}
	else {
		al_log("Model::Load : fail to load model from {}",ConvertToNarrowString(pathName));
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
		al_log("Model::Load : fail to load model file {0} reason {1}",ConvertToNarrowString(pathName), 
			string(imp.GetErrorString())
		);
		return nullptr;
	}

	vector<Mesh::Ptr> 	 meshs;
	vector<uint32>		 meshMaterialIndices;
	vector<Material::Ptr>  	 materials;
	vector<Texture::Ptr> textures;

	unordered_map<String, uint32>  texturePathMap;

	fs::path dirName = fs::path(pathName).parent_path();
	
	for (size_t i = 0; i != scene->mNumMaterials; i++) {

		aiMaterial* mat = scene->mMaterials[i];
		BSDF::Ptr  bsdf(new LambertBSDF(Vector3f(1.f, 1.f, 1.f)));

		vector<MaterialTextureTable> table;
		auto loadTexture = [&](aiTextureType type)-> optional<uint32> {
			if (mat->GetTextureCount(type) != 0) {
				aiString str;
				mat->GetTexture(type, 0, &str);
				String texPath = ConvertFromNarrowString(str.C_Str());
				//if the texture has been loaded
				if (auto iter = texturePathMap.find(texPath);iter != texturePathMap.end()) {
					return iter->second;
				}
				//if it's not a absolute path
				if (fs::path(texPath).is_relative()) {
					texPath = dirName / texPath;
				}
				Texture::Ptr tex = Texture::Load(texPath);
				if (tex == nullptr) {
					al_log("Model::Load : fail to load texture {0}",ConvertToNarrowString(texPath));
					return {};
				}
				else {
					int32 index = (int)textures.size();
					texturePathMap[texPath] = index;
					textures.push_back(tex);
					return index;
				}
			}
			else {
				return {};
			}
		};
		al_for(i, 0, TEXTURE_TYPE_NUM) {
			auto v = loadTexture((aiTextureType)i);
			if (v.has_value()) {
				table.push_back(MaterialTextureTable{(TEXTURE_TYPE)i,
					textures[v.value()]});
			}
		}

		for (size_t i = 0; i != mat->mNumProperties; i++) {
			auto prop = mat->mProperties[i];

			float* data = reinterpret_cast<float*>(prop->mData);
			if (strstr(prop->mKey.C_Str(), "diffuse")) {
				bsdf->SetParameterByName("diffuse", Vector3f(data));
			}
			else if (strstr(prop->mKey.C_Str(),"specular")) {
				bsdf->SetParameterByName("specular",Vector3f(data));
			}
			else if (strstr(prop->mKey.C_Str(),"roughness")) {
				bsdf->SetParameterByName("roughness", *(float*)data);
			}
			else if (strstr(prop->mKey.C_Str(),"metallic")) {
				bsdf->SetParameterByName("metallic", *(float*)data);
			}
		}

		Material::Ptr material(new Material(bsdf, nullptr,
			table.size(), table.data()));

		materials.push_back(material);
	}

	processAiNode(meshs,meshMaterialIndices, scene->mRootNode, scene);

	Model::Ptr model(al_new(Model, meshs, meshMaterialIndices,
		materials, textures, {}));

	return model;
}

AL_PRIVATE bool supportedByAssimp(const wchar_t* extName) {
	static std::unordered_set<String> extNames = {
		L".obj",L".dae",L".xml",L".blend",L".bvh",L".3ds",L".ase",
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

AL_PRIVATE Model::Ptr LoadMMD(const String& pathName) {
	FILE* f;
	if (auto v = OpenFile(pathName, AL_STR("rb"));v.has_value()) {
		f = v.value();
	}
	else {
		al_log("Model::Load : fail to open file {0}", ConvertToNarrowString(pathName));
		return nullptr;
	}
	std::filebuf fb(f);

	std::istream is(&fb);
	pmx::PmxModel x;
	x.Read(&is);
	//TODO : generate tangent data from normals reference http://www.thetenthplanet.de/archives/1180
	
	vector<Mesh::Ptr> 	 meshs;
	vector<uint32>		 meshMaterialIndices;
	vector<Material::Ptr>  	 materials;
	vector<Texture::Ptr> textures;
	fs::path dirName = fs::path(pathName).parent_path();
	//load textures,TODO : currently we ignore the toon textures
	al_for(i, 0, x.texture_count) {
		fs::path path = dirName / x.textures[i];
		Texture::Ptr tex = Texture::Load(ConvertFromWideString(path.wstring()));
		//TODO : rearrange the texture's index rather than throw a assert
		al_assert(tex != nullptr, "Model::Load : fail to load {} texture for model", ConvertToNarrowString(path));
		textures.push_back(tex);
	}

	//mmd models are material driven rendering
	uint32 indexOffset = 0;
	al_for(i, 0, x.material_count) {
		pmx::PmxMaterial& mat = x.materials[i];
		if (mat.index_count <= 0) continue;

		//TODO toon bsdfs
		BSDF::Ptr bsdf(new LambertBSDF());

		//we only need the first 3 components
		bsdf->SetParameterByName("diffuse", Vector3f(mat.diffuse));
		
		vector<Vertex> vertices;
		vector<uint32> indices;
		uint32 vmin = x.indices[indexOffset], vmax = x.indices[indexOffset];
		al_for(i, 0, mat.index_count) {
			vmin = std::min((uint32)x.indices[i + indexOffset], vmin);
			vmax = std::max((uint32)x.indices[i + indexOffset], vmax);
		}
		vertices.resize(vmax - vmin + 1), indices.resize(mat.index_count);
		al_for(i, 0, vertices.size()) {
			Vertex& vert = vertices[i];
			pmx::PmxVertex& pmxVert = x.vertices[vmin + i];
			vert.position = pmxVert.positon;
			vert.normal = pmxVert.normal;
			vert.uv = pmxVert.uv;
		}
		al_for(i, 0, mat.index_count) {
			indices[i] = x.indices[indexOffset + i] - vmin;
		}
		indexOffset += mat.index_count;

		Mesh::Ptr mesh(new Mesh(vertices, indices));
		MaterialTextureTable texTable[] =
		{
			{TEXTURE_TYPE_DIFFUSE,textures[mat.diffuse_texture_index]}
		};
		Material::Ptr material(new Material(bsdf, nullptr, al_countof(texTable), texTable));
		materials.push_back(material);
		meshs.push_back(mesh);
		meshMaterialIndices.push_back(i);
	}

	Model::Ptr model(new Model(meshs, meshMaterialIndices,
		materials, textures, {}));
	return model;
}

Model::Ptr Model::Load(const String& path) {

	fs::path p = path;
	String extName = ConvertFromWideString(p.extension().wstring());
	
	if (supportedByAssimp(extName.c_str())) {
		return LoadByAssimp(path);
	}if (extName == AL_STR(".pmx") || extName == AL_STR(".pmd")) {
		return LoadMMD(path);
	}
	else {
		al_log("Model::Load : file {0} 's extension is not supported", ConvertToNarrowString(path));
	}
	return nullptr;
}

Mesh::Mesh(const vector<Vertex>& vertices,
	const  vector<uint32>& indices):vertices(vertices),indices(indices){}

Model::Model(const vector<Mesh::Ptr>& meshs,
	const vector<uint32>& meshMaterialIndices,
	const vector<Material::Ptr>& materials,
	const vector<Texture::Ptr>& textures,
	const vector<AreaLight::Ptr>& lights):

	meshs(meshs),meshMaterialIndices(meshMaterialIndices),
	materials(materials),textures(textures){ }



bool TriangleIntersect(const ScenePrimitiveInfo& info, const Ray& r, Intersection& isect) {
	//v0,v1,v2 are under world coordinate so we don't need do any transform
	Vertex v0 = info.data.triangle.v0, v1 = info.data.triangle.v1, v2 = info.data.triangle.v2;

	if (!Math::ray_intersect_triangle(v0.position,v1.position,v2.position,r,
		&isect.t,&isect.localUv,&isect.position)) {
		return false;
	}

	isect.normal = Math::normalize(Math::interpolate3(v0.normal, v1.normal, v2.normal, isect.localUv));
	//no vertex contains tangent information
	if (!Math::zero(v0.tangent)) {
		Vector3f interplatedTangent = Math::normalize(Math::interpolate3(v0.tangent, v1.tangent, v2.tangent, isect.localUv));
		Vector3f bitagent = Math::cross(isect.normal, interplatedTangent);
		//make sure the tangent is vertical to normal
		isect.tangent = Math::cross(bitagent, isect.normal);
	}
	//generate a tangent for the triangle
	else {
		isect.tangent = GenerateTangent(isect.normal);
	}
	isect.uv = Math::interpolate3(v0.uv, v1.uv, v2.uv, isect.localUv);
	return true;
}

Vertex TransformVertex(Vertex v,const Transform& trans) {
	Vertex rv = v;
	rv.position = Math::transform_point(trans.GetMatrix(), v.position);
	//multiply a inverse transpose matrix to normals and tangents
	rv.normal = Math::transform_vector(trans.GetTransInvMatrix(), v.normal);
	rv.tangent = Math::transform_vector(trans.GetTransInvMatrix(), v.tangent);
	return rv;
}

vector<ScenePrimitiveInfo> Model::GenerateScenePrimitiveInfos(
	const Transform& trans) {
	vector<ScenePrimitiveInfo> primitiveInfos;
	al_for(i,0,meshs.size()) {
		Mesh::Ptr mesh = meshs[i];
		for (uint32 j = 0; j < mesh->GetIndices().size();j += 3) {
			//delete end
			ScenePrimitiveInfo info;
			uint32 i0 = mesh->GetIndices()[j];
			uint32 i1 = mesh->GetIndices()[j + 1];
			uint32 i2 = mesh->GetIndices()[j + 2];

			info.data.triangle.v0 = TransformVertex(mesh->GetVertices()[i0], trans);
			info.data.triangle.v1 = TransformVertex(mesh->GetVertices()[i1], trans);
			info.data.triangle.v2 = TransformVertex(mesh->GetVertices()[i2], trans);
			
			Bound3f bound;
			bound = Math::bound_merge(bound, info.data.triangle.v0.position);
			bound = Math::bound_merge(bound, info.data.triangle.v1.position);
			bound = Math::bound_merge(bound, info.data.triangle.v2.position);

			info.bound = bound;
			info.intersector = TriangleIntersect;
			info.type = SCENE_PRIMITIVE_TYPE_TRIANGLE;
			info.material = materials[meshMaterialIndices[i]];
			primitiveInfos.push_back(info);
		}
	}
	return primitiveInfos;
}