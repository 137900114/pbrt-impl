#pragma once
#include "common/common.h"
#include "math/math.h"

struct LightSample{
	Vector3f dir;
	Vector3f I;
	Vector3f pos;
};

enum LIGHT_TYPE {
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_DIRECTIONAL,
	LIGHT_TYPE_AREA,
	//LIGHT_TYPE_INFINITY
};

class Scene;

struct VisiblityTester {
	bool Visible(shared_ptr<Scene> scene);
	
	VisiblityTester():distance(0) {}
	
	VisiblityTester(const VisiblityTester& vt):p0(vt.p0),
	dir(vt.dir),distance(vt.distance) {}
	
	VisiblityTester(const Vector3f& p0, const Vector3f& p1):
		p0(p0), dir(Math::normalize(Math::vsub(p1,p0))) {
		distance = Math::length(Math::vsub(p0, p1));
	}
	
	VisiblityTester(const Vector3f& p0,const Vector3f& dir,float distance):
		p0(p0),dir(dir),distance(distance) {}
	
	const VisiblityTester& operator=(const VisiblityTester& other) {
		p0 = other.p0, dir = other.dir;
		distance = other.distance;
		return *this;
	}
private:
	Vector3f p0, dir;
	float distance;
};


class Light {
public:
	al_add_ptr_t(Light);
	Light(LIGHT_TYPE type,const Vector3f& intensity);

	bool IsDeltaType();
	//bool IsInfinity();
	
	/// <summary>
	/// intensity from a delta light source(point light,directional light)
	/// for other types of light sources,this function will return 0
	/// </summary>
	/// <param name="isect">the instersection point of surface which samples the light source</param>
	/// <param name="wi">output parameter,the direction from the sample point to light source</param>
	/// <param name="tester">output parameter,a visiblity tester which checks wether any objects blocks in the sample direction</param>
	/// <returns>the sampled intensity</returns>
	virtual Vector3f DeltaIntensity(const Intersection& isect,Vector3f* wi,VisiblityTester* tester);

	/// <summary>
	/// intensity from a area light source
	/// for other types of light sources,this function will return 0
	/// </summary>
	/// <param name="isect">the instersection point of surface which samples the light source</param>
	/// <param name="seed">the random seed of the sample generated by sampler</param>
	/// <param name="wi">output parameter,the direction from the sample point to light source</param>
	/// <param name="pdf">output parameter,probability density of the sample.for importance sampling</param>
	/// <param name="tester">output parameter,a visiblity tester which checks wether any objects blocks in the sample direction</param>
	/// <returns></returns>
	virtual Vector3f SampleIntensity(const Intersection& isect,const Vector2f& seed
		,Vector3f* wi,float* pdf,VisiblityTester* tester);

	/// <summary>
	/// get emission intensity when a ray hits the surface of area light
	/// for other types of light sources,this function will return 0
	/// </summary>
	/// <param name="isect">the intersection position</param>
	/// <param name="w">the direction of the ray</param>
	/// <returns></returns>
	virtual Vector3f SurfaceEmissionIntensity(const Intersection& isect,const Vector3f& w);

	LIGHT_TYPE GetLightType() {}

	virtual ~Light() {}
protected:
	Vector3f   intensity;
	LIGHT_TYPE type;
};

template<typename T,typename ...Args>
Light* CreateLight(Args... args) {
	static_assert(std::is_base_of_v<Light, T>, "light type should be derived from class 'Light'");
	return al_new(T, args...);
}