#pragma once
#include "common/common.h"
#include "math/math.h"
#include <functional>
#include "scene/material.h"

enum SCENE_PRIMITIVE_TYPE {
	SCENE_PRIMITIVE_TYPE_SPHERE,
	SCENE_PRIMITIVE_TYPE_TRIANGLE,
	SCENE_PRIMITIVE_TYPE_PLANE
};

struct Vertex {
	Vertex() {}
	Vector3f position;
	Vector3f normal;
	Vector3f tangent;
	Vector2f uv;
};

/// <summary>
/// generate a random tangent
/// this function should not be used in model with normal map
/// </summary>
/// <param name="N">the normal</param>
/// <returns>generated tangent</returns>
Vector3f GenerateTangent(const Vector3f& N);


struct ScenePrimitiveInfo {
	SCENE_PRIMITIVE_TYPE type;
	Bound3f bound;
	Material::Ptr material;
	
	union Data{
		Data() {}
		struct {
			Transform trans;
			float radius;
		}sphere;
		struct {
			Vertex v0, v1, v2;
		} triangle;
		struct {
			Vector3f ul, dr, dl,n;
		} plane;
	} data;

	ScenePrimitiveInfo():intersector(nullptr),material(nullptr) {}
	ScenePrimitiveInfo(const ScenePrimitiveInfo& info) {
		Clone(info);
	}
	const ScenePrimitiveInfo& operator=(const ScenePrimitiveInfo& info) {
		Clone(info);
		return *this;
	}

	function<bool(const ScenePrimitiveInfo&,
		const Ray& r, Intersection& isect)> intersector;
private:
	void Clone(const ScenePrimitiveInfo& info);
};


/// <summary>
/// objects in scene which is discribed by math expressions
/// </summary>
class ScenePrimitive {
public:
	al_add_ptr_t(ScenePrimitive);
	virtual Bound3f GetBound(const Transform& trans) = 0;

	//TODO refactor this function.we should not return a intersection information
	//the pdf of angle
	/// <summary>
	/// Generate a random sample from seed to the intersection point p
	/// This function is useful while importance sampling the area light sources
	/// </summary>
	/// <param name="trans">the transform of this primitive</param>
	/// <param name="p">the intersection position</param>
	/// <param name="seed">random seed used to generate the sample</param>
	/// <param name="pdf">output parameter.posibility density for the sample</param>
	/// <returns></returns>
	virtual Intersection Sample(const Transform& trans,const Intersection& p,
		const Vector2f& seed,float* pdf) = 0;

	//virtual bool Intersect(const Transform& trans,const Ray& r,Intersection& isect) = 0;

	/// <summary>
	/// generate a scene primitive info structure from scene primitive class
	/// this function is useful while building the scene
	/// </summary>
	/// <param name="transform">the transform of the primitive</param>
	/// <param name="mat">the material of the primitive</param>
	/// <returns>a scene primitive info structure</returns>
	virtual ScenePrimitiveInfo GeneratePrimitiveInfo(const Transform& transform,
		Material::Ptr mat) = 0;
};

class Plane : public ScenePrimitive {
public:
	Plane(float width,float height);
	virtual Bound3f GetBound(const Transform& trans) override;
	virtual Intersection Sample(const Transform& trans,const Intersection& p,
		const Vector2f& seed,float *pdf);
	virtual ScenePrimitiveInfo GeneratePrimitiveInfo(const Transform& transform,
		Material::Ptr mat) override;
private:
	float width, height;
	//useful while calculating pdf
	float invArea;
};

class Sphere : public ScenePrimitive {
public:
	Sphere(float radius);

	virtual Bound3f GetBound(const Transform& transs) override;
	virtual Intersection Sample(const Transform& trans,const Intersection& p,
		const Vector2f& seed, float* pdf) override;

	virtual ScenePrimitiveInfo GeneratePrimitiveInfo(const Transform& transform,
		Material::Ptr mat) override;
private:
	float radius;
	float r2;
};