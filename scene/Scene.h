#pragma once

#include "math/math.h"
#include "scene/Model.h"
#include "scene/Texture.h"
#include "accelerator/bvh.h"

using ModelID = int32;
using SceneObjectID = int32;


struct IntersectSurfaceInfo {
	//this normal has been normal mapped
	Vector3f     normal;
	Intersection intersection;

	const Material* material;
};


class SceneObject {
public:
	al_add_ptr_t(SceneObject);
	SceneObject(Model::Ptr& model, const Transform& transform);

	Transform& GetTransform() { return transform; }
	Model::Ptr GetModel() { return model; }

private:
	Transform transform;
	Model::Ptr model;
};

class Scene {
public:
	al_add_ptr_t(Scene);
	Scene():sceneBuildFlag(false) {}

	//after editing scene will be built
	void Build();

	uint32 GetModelCount() { return models.size(); }
	Model::Ptr   GetModel(ModelID i);
	SceneObject::Ptr GetSceneObject(SceneObjectID i);

	ModelID LoadModel(const String& path);
	SceneObjectID CreateSceneObject(Model::Ptr model,const Transform& transform);

	IntersectSurfaceInfo Intersect(const Ray& r);

	Texture::Ptr  GetTexture(uint32 texId);
private:
	bool sceneBuildFlag : 1;

	vector<String>			 modelPaths;
	vector<Model::Ptr>		 models;
	vector<SceneObject::Ptr> sceneObjects;
	
	//cleared after every build
	vector<Texture::Ptr>     texturePool;
	vector<Material>         materialPool;
	vector<Vertex>			 vertexPool;
	vector<uint32>			 indexPool;
	vector<uint32>			 primitiveMaterialIndex;
	//tree for acceleration
	BVHTree tree;
};

class SceneIntersector : BVHPrimitiveIntersector {
	friend class Scene;
public:
	SceneIntersector(const vector<uint32>* indexPool,const vector<Vertex>* vertexPool) :
		indexPool(indexPool),vertexPool(vertexPool) {}
	virtual Intersection Intersect(const Ray& r, uint32 primitiveIndex) override;

private:
	const vector<uint32>* indexPool;
	const vector<Vertex>* vertexPool;
};
