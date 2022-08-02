#include "scene_parser.h"
#include "tinyxml2.h"
#include "camera/perspective.h"
#include "light/area_light.h"
#include <filesystem>
using namespace tinyxml2;
namespace fs = std::filesystem;

//TODO print error message
#define getNotNullChildElement(target,node,element)\
	target = node->FirstChildElement(element);\
	al_assert(target != nullptr,"SceneLoader::Load : fail to load scene");


AL_PRIVATE Material::Ptr ParseMaterial(ScenePrimitive::Ptr primitive,XMLElement* materialNode) {
	XMLElement* typeNode;
	LambertBSDF::Ptr bsdf;
	getNotNullChildElement(typeNode, materialNode, "bsdf");
	if (strcmp(typeNode->GetText(),"lambert")) {
		XMLElement* diffuseNode;
		getNotNullChildElement(diffuseNode, materialNode, "diffuse");
		vector<String> dif = SplitString(ConvertFromNarrowString(diffuseNode->GetText()), AL_STR(','));
		Vector3f diffuse(StringCast<float>::FromString(dif[0]), StringCast<float>::FromString(dif[1]), StringCast<float>::FromString(dif[2]));
		
		bsdf =  make_shared<LambertBSDF>(diffuse);
	}
	else {
		al_warn("SceneLoader::Load : fail to load the scene, fail to parse material");
		return nullptr;
	}

	AreaLight::Ptr light = nullptr;
	if (auto v = materialNode->FirstChildElement("area_light");v != nullptr) {
		XMLElement* intensityNode;
		getNotNullChildElement(intensityNode, materialNode, "diffuse");
		vector<String> intens = SplitString(ConvertFromNarrowString(intensityNode->GetText()), AL_STR(','));
		Vector3f intensity(StringCast<float>::FromString(intens[0]), StringCast<float>::FromString(intens[1]), StringCast<float>::FromString(intens[2]));

		light = make_shared<AreaLight>(primitive,intensity);
	}

	//TODO : support texture in the future
	return Material::Ptr(new Material(bsdf, light, 0, nullptr));
}

AL_PRIVATE Transform ParseTransform(XMLElement* objectNode) {
	Transform trans;
	XMLElement* transformElement,
		* positionElement, * rotationElement, * scaleElement;
	getNotNullChildElement(transformElement, objectNode, "transform");

	//TODO checks and error logging
	getNotNullChildElement(positionElement, transformElement, "position");
	vector<String> pos = SplitString(ConvertFromNarrowString(positionElement->GetText()), AL_STR(','));
	Vector3f position(StringCast<float>::FromString(pos[0]), StringCast<float>::FromString(pos[1]), StringCast<float>::FromString(pos[2]));
	getNotNullChildElement(rotationElement, transformElement, "rotation");
	vector<String> rot = SplitString(ConvertFromNarrowString(rotationElement->GetText()), AL_STR(','));
	Quaternion rotation(Vector3f(StringCast<float>::FromString(rot[0]), StringCast<float>::FromString(rot[1]), StringCast<float>::FromString(rot[2])),
		StringCast<float>::FromString(rot[3]));
	getNotNullChildElement(scaleElement, transformElement, "scale");
	vector<String> scl = SplitString(ConvertFromNarrowString(scaleElement->GetText()), AL_STR(','));
	Vector3f scale(StringCast<float>::FromString(scl[0]), StringCast<float>::FromString(scl[1]), StringCast<float>::FromString(scl[2]));

	trans = Transform(position, rotation, scale);
	return trans;
}

optional<tuple<Scene::Ptr, Camera::Ptr>>
	SceneLoader::Load(const String& path, ParamParser::Ptr parser){
	FILE* f = nullptr;
	if (auto v = OpenFile(path,AL_STR("w"));v.has_value() ) {
		f = v.value();
	}
	else {
		al_warn("SceneLoader::Load : fail to load the scene");
		return {};
	}

	XMLDocument doc;
	if (auto v = doc.LoadFile(f);v != XML_SUCCESS) {
		static const char* xmlErrorStr[] = {
			"XML_SUCCESS",
			"XML_NO_ATTRIBUTE",
			"XML_WRONG_ATTRIBUTE_TYPE",
			"XML_ERROR_FILE_NOT_FOUND",
			"XML_ERROR_FILE_COULD_NOT_BE_OPENED",
			"XML_ERROR_FILE_READ_ERROR",
			"XML_ERROR_PARSING_ELEMENT",
			"XML_ERROR_PARSING_ATTRIBUTE",
			"XML_ERROR_PARSING_TEXT",
			"XML_ERROR_PARSING_CDATA",
			"XML_ERROR_PARSING_COMMENT",
			"XML_ERROR_PARSING_DECLARATION",
			"XML_ERROR_PARSING_UNKNOWN",
			"XML_ERROR_EMPTY_DOCUMENT",
			"XML_ERROR_MISMATCHED_ELEMENT",
			"XML_ERROR_PARSING",
			"XML_CAN_NOT_CONVERT_TEXT",
			"XML_NO_TEXT_NODE",
			"XML_ELEMENT_DEPTH_EXCEEDED"
		};
		al_log("SceneLoader::Load : fail to load scene from {} reason : {}", ConvertToNarrowString(path),
			string(xmlErrorStr[v]));
	}

	Scene::Ptr scene(new Scene);
	unordered_map<String, ModelID> modelTable;

	String parentPath = fs::path(path).parent_path();
	XMLElement* sceneNode = doc.FirstChildElement("scene");
	XMLElement* modelNode = sceneNode->FirstChildElement("model");
	while (modelNode != nullptr) {
		String modelPath = ConvertFromNarrowString(modelNode->GetText());
		String modelName;
		if (auto name = modelNode->Attribute("name"); name != nullptr) {
			modelName = ConvertFromNarrowString(name);
		}
		else {
			al_warn("SceneLoader::Load : fail to load the scene, all models in scene should have a name");
			return {};
		}

		if (fs::path(modelPath).is_relative()) {
			modelPath = parentPath + AL_STR("/") + modelPath;
		}
		if (auto v = scene->LoadModel(modelPath); v.has_value()) {
			modelTable[modelName] = v.value();
		}
		else {
			al_warn("SceneLoader::Load : fail to load the scene, fail to load model {}", ConvertToNarrowString(modelPath));
			return {};
		}

		modelNode = modelNode->NextSiblingElement("model");
	}

	//TODO support textures
	XMLElement* objectNode = sceneNode->FirstChildElement("object");
	while (objectNode != nullptr) {
		Transform trans = ParseTransform(objectNode);
		if (auto v = objectNode->FirstChildElement("model"); v != nullptr) {
			String name = ConvertFromNarrowString(v->GetText());
			if (auto v = modelTable.find(name); v != modelTable.end()) {
				scene->CreateSceneObject(v->second, trans);
			}
			else {
				al_log("SceneLoader::Load : fail to load scene invalid model name {}", ConvertToNarrowString(name));
				return {};
			}
		}
		else{
			ScenePrimitive::Ptr prim;
			if (auto sphereNode = objectNode->FirstChildElement("sphere"); sphereNode != nullptr) {
				XMLElement* radius;
				getNotNullChildElement(radius, sphereNode, "radius");
				prim = make_shared<Sphere>(StringCast<float>::FromString(ConvertFromNarrowString(radius->GetText())));
			}
			else if (auto planeNode = objectNode->FirstChildElement("plane"); planeNode != nullptr) {
				XMLElement* width, * height;
				getNotNullChildElement(width, planeNode, "width");
				getNotNullChildElement(height, planeNode, "height");
				prim = make_shared<Plane>(
					StringCast<float>::FromString(ConvertFromNarrowString(width->GetText())),
					StringCast<float>::FromString(ConvertFromNarrowString(height->GetText())));
			}
			else {
				al_warn("SceneLoader::Load : fail to load scene invalid scene object");
				return {};
			}

			XMLElement* materialNode;
			getNotNullChildElement(materialNode, objectNode, "material");
			Material::Ptr mat = ParseMaterial(prim, materialNode);

			scene->CreateSceneObject(prim, mat, trans);
		}
	}

	uint32 width = 1000,height = 1000;
	if (auto v = parser->Get<uint32>(AL_STR("width")); v.has_value()) {
		width = v.value();
		parser->Set(AL_STR("width"), width);
	}
	if (auto v = parser->Get<uint32>(AL_STR("height"));v.has_value()) {
		height = v.value();
		parser->Set(AL_STR("height"), height);
	}

	PerspectiveCamera::Ptr camera;
	XMLElement* cameraNode = sceneNode->FirstChildElement("camera");
	if (cameraNode != nullptr) {
		Transform trans = ParseTransform(cameraNode);
		camera = make_shared<PerspectiveCamera>(width, height, trans);
	}
	else {
		al_warn("SceneLoader::Load : a scene must contain a camera");
		return {};
	}

	scene->Build();
	return { make_tuple(scene,camera)};
}