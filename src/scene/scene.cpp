#include "scene.h"
#include "math/math.h"
#include "light/area_light.h"

//TODO : support primitive insection
bool SceneIntersector::Intersect(const Ray& r, uint32 primitiveIndex,Intersection& isect) {
	const ScenePrimitiveInfo& primitive = scenePrimitives[primitiveIndex];
	if (primitive.intersector(primitive,r,isect)) {
		//push the position a little forward
		isect.adjustedPosition = isect.position + isect.normal * .0005f;
		return true;
	}
	return false;
}


void Scene::Build() {
	if (sceneBuildFlag) return;
	scenePrimtives.clear();
	tree.Clear();
	finiteLightSources.clear();

	//TODO currently we assume the scene objects have different model and textures
	vector<BVHPrimitive> bounds;
	al_for(i,0,sceneObjects.size()) {
		SceneObject::Ptr obj = sceneObjects[i];
		ScenePrimitive::Ptr primitive = obj->GetPrimitive();
		if (primitive != nullptr) {
			ScenePrimitiveInfo info = primitive->GeneratePrimitiveInfo(*obj->GetTransform(),
				obj->GetMaterial());
			bounds.push_back(BVHPrimitive(info.bound));
			scenePrimtives.push_back(info);
			
		}
		else {
			Model::Ptr model = obj->GetModel();
			vector<ScenePrimitiveInfo> infos = model->GenerateScenePrimitiveInfos(*obj->GetTransform());
			al_for(i,0,infos.size()) {
				bounds.push_back(BVHPrimitive(infos[i].bound));
			}
			scenePrimtives.insert(scenePrimtives.end(),infos.begin(), infos.end());
		}
	}

	al_for(i, 0, lightSources.size()) {
		if (!lightSources[i]->IsInfinity()) {
			finiteLightSources.push_back(lightSources[i]);
		}
	}

	SceneIntersector* intersector = al_new(SceneIntersector,scenePrimtives);
	sceneBuildFlag = true;
	tree.Build(bounds, intersector);
}

Model::Ptr   Scene::GetModel(ModelID i) {
	al_assert(i < models.size() && i >= 0, "Scene::GetModel index {0} is out of bondary {1}",i,models.size());
	return models[i];
}

//user may adjust scene through scene object so set build flag to false
SceneObject::Ptr Scene::GetSceneObject(SceneObjectID i) {
	al_assert(i < sceneObjects.size() && i >= 0, "Scene::GetSceneObject index {0} is out of bondary {1}", i, models.size());

	sceneBuildFlag = false;
	return sceneObjects[i];
}

optional<ModelID> Scene::LoadModel(const String& path) {
	sceneBuildFlag = false;
	if (auto iter = std::find(modelPaths.begin(), modelPaths.end(), path);iter != modelPaths.end()) {
		al_log("model {0} is found at {1}", ConvertToNarrowString(path), iter - modelPaths.begin());
		return iter - modelPaths.begin();
	}
	Model::Ptr model = Model::Load(path);
	modelPaths.push_back(path);
	if (model != nullptr) {
		models.push_back(model);
		return (uint32)(models.size() - 1);
	}
	else {
		modelPaths.pop_back();
	}
	return {};
}

SceneObjectID Scene::CreateSceneObject(ModelID _model, const Transform& transform) {
	Model::Ptr model = GetModel(_model);
	SceneObject::Ptr sobj(al_new(SceneObject, model, transform));
	sceneObjects.push_back(sobj);
	//add a new object to the scene.set to build flag to false
	sceneBuildFlag = false;
	//currently models doesn't support light sources
	return (SceneObjectID)sceneObjects.size() - 1;
}

SceneObjectID Scene::CreateSceneObject(ScenePrimitive::Ptr primitive, Material::Ptr mat,
	const Transform& transform) {
	SceneObject::Ptr sobj(new SceneObject(primitive, mat, transform));
	sceneObjects.push_back(sobj);
	auto areaLight = mat->GetEmission();
	if (areaLight.has_value()) {
		AreaLight::Ptr lightSource = areaLight.value();
		//TODO : Werid function call.May refactor it later
		lightSource->SetTransform(sobj->GetTransform());
		lightSources.push_back(lightSource);
	}

	sceneBuildFlag = false;
	return (SceneObjectID)sceneObjects.size() - 1;
}


SceneObject::SceneObject(Model::Ptr model, const Transform& transform): model(model){
	al_assert(model != nullptr, "SceneObject::SceneObject : model of a scene object should not be nullptr");
	this->transform = Transform::Ptr(new Transform(transform));
}

SceneObject::SceneObject(ScenePrimitive::Ptr primitive, Material::Ptr mat,
	const Transform& transform): material(mat),primitive(primitive) {
	al_assert(primitive != nullptr, "SceneObject::SceneObject : scene primitive of a scene object should not be nullptr");
	al_assert(mat != nullptr,"SceneObject::SceneObject : material of a scene object should not be nullptr");
	this->transform = Transform::Ptr(new Transform(transform));
}

Light::Ptr Scene::GetLightSource(LightID id) {
	al_assert(id < lightSources.size(), "Scene::GetLightSource : light source id out of bondary");
	return lightSources[id];
}

LightID Scene::AddLightSource(Light::Ptr light) {
	al_for(i, 0, lightSources.size()) {
		//don't add repeated light source
		if (lightSources[i] == light) {
			return i;
		}
	}
	lightSources.push_back(light);
	return lightSources.size() - 1;
}

bool Scene::Intersect(const Ray& r,SurfaceIntersection& info) {
	al_assert(sceneBuildFlag, "Scene::Intersect : scene must be built before any intersection test");

	BVHIntersectInfo bvhInfo;
	bool intersected = tree.Intersect(r,bvhInfo);
	
	if (intersected) {
		ScenePrimitiveInfo& primitive = scenePrimtives[bvhInfo.primitiveIndex];
		info = primitive.material->Intersect(bvhInfo.intersection, primitive.material);
	}
	
	return intersected;
}

bool Scene::Intersect(const Ray& r) {
	al_assert(sceneBuildFlag,"Scene::Intersect : scene must be built before any intersection test");
	BVHIntersectInfo bvhInfo;
	bool intersected = tree.Intersect(r, bvhInfo);

	return intersected;
}

Light::Ptr Scene::PickOneFiniteLightSources(float seed) {
	if (finiteLightSources.size() == 0) return nullptr;
	uint32 i = min((uint32)(seed * finiteLightSources.size()), 
		(uint32)finiteLightSources.size() - 1);
	return finiteLightSources[i];
}