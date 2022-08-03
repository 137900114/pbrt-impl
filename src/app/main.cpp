#include "core.h"
#include "parser.h"
#include "scene_parser.h"
#include <filesystem>
#include <iostream>

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
				  "will be overwrited by command line parameters",nullptr,nullptr,false,true,3,{"-C","-c","--config"}},
		{"dump", "dump the configurations to a file",nullptr,nullptr,false,true,3,{"-D","-d","--dump"}},
		{"help","lists avaiable options",nullptr,help,false,false,3,{"-H","-h","--help"}},
		{"scene","* input scene file",nullptr,nullptr,true,true,3,{"-S","-s","--scene"}},
		{"output","* output picture file",nullptr,nullptr,true,true,3,{"-O","-o","-output"}},
		{"debug_break","set debug points on a set of pixels e.g.: -b 114,514;191,810"
						"debug point will be set on (114,514) and (191,810)",
						nullptr,nullptr,true,true,3,{"-B","-b","-break"}},
		{"thread_num","set num of threads for rendering(by default is 4)",
						"4",nullptr,true,true,3,{"-NT","-nt","--nthread"}},
		{"spp","set sample-per-pixel(by default is 64)","64",nullptr,true,true,1,{"--spp"}},
		{"width","the width of the output picture(1000 by default)","1000",nullptr,true,true,3,{"-W","-w","--width"}},
		{"height","the height of the output picture(1000 by default)","1000",nullptr,true,true,3,{"-H","-h","--height"}}
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

	integrator->Render();

	camera->SaveFilm(output);

	al_log_finalize();
}