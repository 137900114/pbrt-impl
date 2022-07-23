#pragma once

#include "math/math.h"
#include "scene/model.h"
#include "scene/texture.h"
#include "accelerator/bvh.h"
#include "light/light.h"
#include "scene_primitive.h"

using ModelID = uint32;
using SceneObjectID = uint32;
using LightID = uint32;

class SceneObject {
	friend class Scene;
public:
	al_add_ptr_t(SceneObject);
	SceneObject(Model::Ptr model, const Transform& transform);
	SceneObject(ScenePrimitive::Ptr primitive, Material::Ptr mat,
		const Transform& transform);

	Transform::Ptr GetTransform() { return transform; }
	Model::Ptr GetModel() { return model; }
	
	ScenePrimitive::Ptr GetPrimitive() { return primitive; }
	Material::Ptr GetMaterial() { return material; }
private:
	Transform::Ptr transform;
	Model::Ptr model;

	ScenePrimitive::Ptr primitive;
	Material::Ptr material;
};


class Scene {
public:
	al_add_ptr_t(Scene);
	Scene():sceneBuildFlag(false) {}

	//after editing scene will be built
	void Build();

	uint32				GetModelCount() { return models.size(); }
	Model::Ptr			GetModel(ModelID i);
	SceneObject::Ptr	GetSceneObject(SceneObjectID i);

	optional<ModelID>	LoadModel(const String& path);
	SceneObjectID		CreateSceneObject(Model::Ptr model,const Transform& transform);
	SceneObjectID		CreateSceneObject(ScenePrimitive::Ptr primitive, Material::Ptr mat,
		const Transform& transform);

	LightID				AddLightSource(Light::Ptr light);

	LightID				GetLightSourceCount() { return lightSources.size(); }
	Light::Ptr			GetLightSource(LightID id);

	/// <summary>
	/// used for constructing tracing path
	/// </summary>
	/// <param name="r"></param>
	/// <param name="info"></param>
	/// <returns></returns>
	bool			    Intersect(const Ray& r, SurfaceIntersection& info);
	/// <summary>
	/// used for visibility test
	/// </summary>
	/// <param name="r"></param>
	/// <param name="isect"></param>
	/// <returns></returns>
	bool				Intersect(const Ray& r);
private:
	bool sceneBuildFlag : 1;

	vector<Light::Ptr>		 lightSources;
	vector<String>			 modelPaths;
	vector<Model::Ptr>		 models;
	vector<SceneObject::Ptr> sceneObjects;
	
	//cleared after every build
	vector<ScenePrimitiveInfo> scenePrimtives;

	//tree for acceleration
	BVHTree tree;
};


class SceneIntersector : public BVHPrimitiveIntersector {
	friend class Scene;
public:
	SceneIntersector(const vector<ScenePrimitiveInfo>& primitives):scenePrimitives(primitives) {}
	virtual bool Intersect(const Ray& r, uint32 primitiveIndex,Intersection& isect) override;
private:
	const vector<ScenePrimitiveInfo>& scenePrimitives;
};
