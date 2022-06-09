#pragma once


#include "scene/Model.hpp"
#include "scene/Texture.hpp"


class SceneObject {
public:
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
	ptr<Model>   GetModel(uint32 i);
	ptr<SceneObject> GetSceneObject(uint32 i);

	int32 LoadModel(const String& path);
	int32 CreateSceneObject(ptr<Model> model,const Transform& transform);
private:
	vector<String>			 modelPaths;
	vector<ptr<Model>>		 models;
	vector<ptr<SceneObject>> sceneObjects;
	
	//cleared after every build
	vector<ptr<Texture>>     texturePool;
	vector<Material>         materialPool;
	vector<Vertex>			 vertexPool;
	vector<uint32>			 vertexMaterialIndex;
	vector<uint32>			 indexPool;
	//tree for acceleration
};