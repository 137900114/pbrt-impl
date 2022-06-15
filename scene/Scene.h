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
	SceneObject(ptr<Model>& model, const Transform& transform);

	Transform& GetTransform() { return transform; }
	ptr<Model> GetModel() { return model; }

private:
	Transform transform;
	ptr<Model> model;
};

class Scene {
public:
	Scene() {}

	//after editing scene will be built
	void Build();

	uint32 GetModelCount() { return models.size(); }
	ptr<Model>   GetModel(ModelID i);
	ptr<SceneObject> GetSceneObject(SceneObjectID i);

	ModelID LoadModel(const String& path);
	SceneObjectID CreateSceneObject(ptr<Model> model,const Transform& transform);

	IntersectSurfaceInfo Intersect(const Ray& r);

	ptr<Texture>  GetTexture(uint32 texId);
private:
	vector<String>			 modelPaths;
	vector<ptr<Model>>		 models;
	vector<ptr<SceneObject>> sceneObjects;
	
	//cleared after every build
	vector<ptr<Texture>>     texturePool;
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
