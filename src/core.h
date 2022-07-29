//include everything
#pragma once

#include "accelerator/bvh.h"

#include "camera/camera.h"
#include "camera/perspective.h"

#include "common/common.h"
#include "common/rtti.h"

#include "sampler/sampler.h"
#include "sampler/halton.h"
#include "sampler/rng.h"

#include "integrator/integrator.h"
#include "integrator/sampled_integrator.h"
#include "integrator/path.h"

#include "light/point_light.h"
#include "light/area_light.h"
#include "light/environment_light.h"
#include "light/directional_light.h"

#include "math/math.h"

#include "scene/material.h"
#include "scene/model.h"
#include "scene/scene_primitive.h"
#include "scene/texture.h"

