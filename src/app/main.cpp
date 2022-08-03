#include "core.h"
#include "parser.h"
#include "scene_parser.h"
#include <filesystem>
#include <iostream>

constexpr uint32 width = 1000;
constexpr uint32 height = 1000;

void help(ParamParser*, ParameterTable* table,uint32 count) {
	printf("options : (* stands for options required)\n");
	al_for(i,0,count) {
		printf("%-10s\t%-30s\t\n                values:", table[i].key, table[i].discribtion);
		al_for(j,0,table[i].name_count) {
			printf("%s ", table[i].names[j]);
		}
		printf("\n");
	}
	exit(0);
}

int main(int argc,const char** argvs){
	al_log_initialize();

	ParameterTable paramTable[] = {
		{"config","load configuration from previously dumped file,"
				  "will be overwrited by command line parameters",nullptr,nullptr,true,3,{"-C","-c","--config"}},
		{"dump", "dump the configurations to a file",nullptr,nullptr,true,3,{"-D","-d","--dump"}},
		{"help","lists avaiable options",nullptr,help,false,3,{"-H","-h","--help"}},
		{"scene","* input scene file",nullptr,nullptr,true,3,{"-S","-s","--scene"}},
		{"output","* output picture file",nullptr,nullptr,true,3,{"-O","-o","-output"}},
		{"debug_break","set debug points on a set of pixels e.g.: -b 114,514;191,810"
						"debug point will be set on (114,514) and (191,810)",
						nullptr,nullptr,true,3,{"-B","-b","-break"}},
		{"thread_num","set num of threads for rendering(by default is 4)",
						"4",nullptr,true,3,{"-NT","-nt","--nthread"}},
		{"spp","set sample-per-pixel(by default is 64)","64",nullptr,true,1,{"--spp"}},
		{"width","the width of the output picture(1000 by default)","1000",nullptr,true,3,{"-W","-w","--width"}},
		{"height","the height of the output picture(1000 by default)","1000",nullptr,true,3,{"-H","-h","--height"}}
	};
	
	ParamParser::Ptr parser(new ParamParser(argc - 1, argvs + 1, al_countof(paramTable), paramTable));
	if (auto v = parser->Get<String>(AL_STR("config"));v.has_value()) {
		parser->Load(v.value());
	}

	auto v = SceneLoader::Load(parser->Require<String>(AL_STR("scene")),parser);
	al_assert(v.has_value(),"fail to load scene");
	auto [scene, camera] = v.value();

	if (auto v = parser->Get<String>(AL_STR("dump")); v.has_value()) {
		parser->Dump(v.value());
	}

	PathIntegrator::Ptr integrator(new PathIntegrator);
	integrator->AttachCamera(camera);
	integrator->AttachScene(scene);
#ifdef DEBUG
	if (auto v = parser->Get<String>(AL_STR("debug_break")); v.has_value()) {
		vector<String> pts = SplitString(v.value(),AL_STR(';'));
		al_for(i,0,pts.size()) {
			vector<String> pt = SplitString(pts[i], AL_STR(','));
			uint32 x = StringCast<uint32>::FromString(pt[0]),
				y = StringCast<uint32>::FromString(pt[1]);
			integrator->DebugBreakAtPixel(x, y);
		}
	}
#endif
	integrator->SetDirectLightEsitimateStrategy(DIRECT_LIGHT_ESITIMATE_STRATEGY_MIS);
	integrator->SetSamplePerPixel(StringCast<uint32>::FromString(parser->Require<String>(AL_STR("spp"))));
	integrator->SetThreadNumber(StringCast<uint32>::FromString(parser->Require<String>(AL_STR("thread_num"))));
	integrator->SetMaxDepth(5);

	String output = parser->Require<String>(AL_STR("output"));
	al_assert(filesystem::exists(output),"invalid output file");

	integrator->Render();

	camera->SaveFilm(output);
	
	/*Scene::Ptr scene(new Scene);
	
	const uint32 processor_count = std::thread::hardware_concurrency();

	ConstantEnvironmentLight::Ptr env(new ConstantEnvironmentLight(Vector3f::I * .1f));
	

	Plane::Ptr  plane(new Plane(40.f, 40.f));
	BSDF::Ptr bsdf(new LambertBSDF(Vector3f(6, 102, 255) / 255.f));
	Material::Ptr diffuse_mat2(new Material(bsdf, nullptr, 0, nullptr));
	Transform trans3(Vector3f(0.f, -10.f, 13.f), Quaternion(), Vector3f::I);
	scene->CreateSceneObject(plane, diffuse_mat2, trans3);

	Plane::Ptr  plane2(new Plane(10.f, 10.f));
	Transform trans4(Vector3f(-10.f, -5.f, 13.f), Quaternion(Vector3f(0.f, 0.f, 1.f), Math::pi / 2.f), Vector3f::I);
	BSDF::Ptr bsdf2(new LambertBSDF(Vector3f(0.8, 1., 1.)));
	AreaLight::Ptr light(new AreaLight(plane2, Vector3f::I * 3.f));
	Material::Ptr emission_mat(new Material(bsdf2, light, 0, nullptr));
	scene->CreateSceneObject(plane2, emission_mat, trans4);

	Plane::Ptr  plane3(new Plane(30.f, 10.f));
	BSDF::Ptr bsdf3(new LambertBSDF(Vector3f(0.5, 1, 0)));
	Material::Ptr diffuse_mat(new Material(bsdf3, nullptr, 0, nullptr));
	Transform trans5(Vector3f(8.f, -7.5f, 13.f), Quaternion(Vector3f(0.f, 0.f, 1.f), Math::pi / 2.f), Vector3f::I);
	scene->CreateSceneObject(plane3, diffuse_mat, trans5);

	Sphere::Ptr sphere(new Sphere(1.0f));
	AreaLight::Ptr light2(new AreaLight(sphere, Vector3f(1.0f, 0.f, 1.f) * 10.f));
	BSDF::Ptr bsdf4(new LambertBSDF(Vector3f(0.8, 1., 1.)));
	Material::Ptr emission_mat2(new Material(bsdf4, light2, 0, nullptr));
	Transform trans1(Vector3f(3.f, -9.f, 13.f), Quaternion(Vector3f(1., 0., 0.), Math::pi / 2.f), Vector3f::I);
	scene->CreateSceneObject(sphere, emission_mat2, trans1);

	ModelID model;
	if (auto v = scene->LoadModel(AL_STR("C:\\Users\\谈造楠\\codes\\pbrt-impl\\assets\\model\\model.pmx")); v.has_value()) {
		//if (auto v = scene->LoadModel(AL_STR("C:\\Users\\谈造楠\\codes\\pbrt-impl\\assets\\monkey.obj")); v.has_value()) {
		model = v.value();
	}
	else {
		al_warn("fail to load model");
		return 0;
	}
	Transform trans2(Vector3f(0.f, -10.f, 13.f), Quaternion(), Vector3f::I * .5f);
	scene->CreateSceneObject(model, trans2);
	//scene->AddLightSource(env);

	scene->Build();

	Transform camTrans(Vector3f(), Quaternion(), Vector3f::I);
	PerspectiveCamera::Ptr camera(new PerspectiveCamera(width, height, camTrans));

	//NormalIntegrator::Ptr integrator(new NormalIntegrator);
	PathIntegrator::Ptr integrator(new PathIntegrator);
	integrator->SetSamplePerPixel(128);
	integrator->SetThreadNumber(16);
	integrator->SetMaxDepth(5);
	integrator->AttachCamera(camera);
	integrator->AttachScene(scene);
	integrator->SetDirectLightEsitimateStrategy(DIRECT_LIGHT_ESITIMATE_STRATEGY_MIS);

	integrator->Render();

	camera->SaveFilm(AL_STR("miku_illumance4.png"));*/

	al_log_finalize();
}