#pragma once
//for scene loader
#include "parser.h"
#include "scene/scene.h"
#include "camera/camera.h"


class SceneLoader {
public:
	static optional<tuple<Scene::Ptr,Camera::Ptr>> 
		Load(const String& path,ParamParser::Ptr parser);
};