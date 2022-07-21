#pragma once
#include "common/common.h"
#include <cmath>

#define al_fequal(f1,f2) abs(f1 - f2) < 1e5

#define infinity std::numeric_limits<float>::max()
#define infinity_i std::numeric_limits<uint32>::max()

namespace Math {
    inline bool isNan(float f)
    {
        union { float f; int i; } u;
        u.f = f;
        return (u.i & 0x7fffffff) > 0x7f800000;
    }

    inline constexpr float pi = 3.1415926;
}

struct Vector2f {
    union {
        struct {
            float x, y;
        };
        float a[2];
    };
    Vector2f(){
        zero_memory(this);
    }
    Vector2f(const Vector2f& v) {
        copy_memory(this, &v);
    }
    Vector2f(const float* f) {
        x = f[0], y = f[1];
    }

    Vector2f(float x,float y):x(x),y(y) {}
};



struct Vector3f {
    union{
        struct {
            float x,y,z;
        };
        float a[3];
    };
    Vector3f(){
        zero_memory(this);
    }
    Vector3f(const Vector3f& vec) {
        copy_memory(this,&vec);
    }
    Vector3f(const float* f) {
        x = f[0], y = f[1], z = f[2];
    }

    Vector3f(float x, float y, float z):x(x),y(y),z(z) {}

    const Vector3f& operator=(const Vector3f& vec) {
        copy_memory(this, &vec);
        return *this;
    }
};

struct Vector4f {
    union {
        struct {
            float x, y, z, w;
        };
        float a[4];
    };
    Vector4f() {
        zero_memory(this);
    }
    Vector4f(const Vector4f& vec) {
        copy_memory(this, &vec);
    }
    Vector4f(const float* f) {
        x = f[0], y = f[1], z = f[2], w = f[3];
    }

    Vector4f(float x, float y, float z,float w) :x(x), y(y), z(z),w(w){}

    const Vector4f& operator=(const Vector4f& vec) {
        copy_memory(this, &vec);
        return *this;
    }

    Vector3f XYZ() { return Vector3f(x, y, z); }
};


struct Quaternion {
    Vector4f val;

    Quaternion(const Quaternion& o):val(o.val) {}
    Quaternion(const Vector3f& axis, float angle);
    Quaternion():val(0.f,0.f,0.f,1.f) {}

    const Quaternion& operator=(const Quaternion& q) {
        val = q.val;
        return *this;
    }
};

struct Ray {
    Vector3f o, d;
    Vector3f invd;
    Ray():d(1,0,0),invd(1,infinity,infinity) {}
    Ray(const Vector3f& o, const Vector3f& d) :o(o),d(d){
        invd.x = 1.f / d.x, invd.y = 1.f / d.y, invd.z = 1.f / d.z;
    }
    const Ray& operator=(const Ray& r) {
        o = r.o, d = r.d,invd = r.invd;
        return *this;
    }
    void SetDirection(const Vector3f& d) {
        this->d = d;
        invd.x = 1.f / d.x, invd.y = 1.f / d.y, invd.z = 1.f / d.z;
    }
    void SetPosition(const Vector3f& o) {
        this->o = o;
    }
};



struct Mat4x4{
    union{
        struct {
            float a00, a01, a02, a03,
                  a10, a11, a12, a13,
                  a20, a21, a22, a23,
                  a30, a31, a32, a33;
        };
        float a[4][4];
        float raw[16];
    };
    Mat4x4() {
        zero_memory(this);
    }
    Mat4x4(const Mat4x4& mat) {
        copy_memory(this, &mat);
    }

    Mat4x4(float a00,float a01,float a02,float a03,
           float a10,float a11,float a12,float a13,
           float a20,float a21,float a22,float a23,
           float a30,float a31,float a32,float a33) {
        a[0][0] = a00, a[0][1] = a01, a[0][2] = a02, a[0][3] = a03;
        a[1][0] = a10, a[1][1] = a11, a[1][2] = a12, a[1][3] = a13;
        a[2][0] = a20, a[2][1] = a21, a[2][2] = a22, a[2][3] = a23;
        a[3][0] = a30, a[3][1] = a31, a[3][2] = a32, a[3][3] = a33;
    }

    const Mat4x4& operator=(const Mat4x4& mat) {
        copy_memory(this, &mat);
        return *this;
    }

    Vector4f row(uint32 i) const {
        return Vector4f(a[i][0], a[i][1], a[i][2], a[i][3]);
    }

    Vector4f column(uint32 i) const {
        return Vector4f(a[0][i], a[1][i], a[2][i], a[3][i]);
    }
};


struct Bound3f {
    union {
        struct {
            Vector3f lower, upper;
        };
        Vector3f bound[2];
    };
    Bound3f() {
        constexpr float fmin = std::numeric_limits<float>::lowest();
        constexpr float fmax = std::numeric_limits<float>::max();
        lower = Vector3f(fmax, fmax, fmax);
        upper = Vector3f(fmin, fmin, fmin);
    }

    Bound3f(Vector3f lower, Vector3f upper):lower(lower),
    upper(upper){}

    Bound3f(const Bound3f& b) {
        lower = b.lower, upper = b.upper;
    }

    const Bound3f& operator=(const Bound3f& b) {
        lower = b.lower, upper = b.upper;
        return *this;
    }
};

struct Transform {
    al_add_ptr_t(Transform);
    Transform() :quat(Vector3f(0., 1., 0.), 0.) { RecomputeMatrix(); }
    Transform(const Vector3f& position, const Quaternion& quat, const Vector3f& scale) :
        position(position), quat(quat), scale(scale) {
        RecomputeMatrix();
    }
    Transform(const Transform& other):position(other.position),
        quat(other.quat),scale(other.scale),world(other.world),
        transInvWorld(other.transInvWorld){}
    
    const Transform& operator=(const Transform& other) {
        position = other.position;
        quat = other.quat;
        scale = other.scale;
        world = other.world;
        transInvWorld = other.transInvWorld;
        return *this;
    }

    void SetPosition(const Vector3f& position);
    void SetRotation(const Quaternion& quat);
    void SetScale(const Vector3f& scale);

    const Vector3f& GetPosition() const { return position; }
    const Quaternion& GetRotation() const { return quat; }
    const Vector3f& GetScale() const { return scale; }
    const Mat4x4& GetMatrix() const {return world;}
    const Mat4x4& GetTransInvMatrix() const { return transInvWorld; }

private:
    void RecomputeMatrix();

    Vector3f position;
    Quaternion quat;
    Vector3f scale;

    Mat4x4   world;
    Mat4x4   transInvWorld;
};

struct Intersection {
    float t;
    Vector2f uv;
    Vector2f localUv;
    Vector3f position;
    Vector3f normal;
    Vector3f tangent;
};


//TODO rewrite using concept in c++20
namespace Math {
    template<typename T>
    T clamp(T a,T lower,T upper) {
        return std::max(std::min(a, upper), lower);
    }

    float frac(float v);

    //from sin and cos to 0~2pi radius
    float angle(float sinValue,float cosValue);

    Vector3f vmax(const Vector3f& a,const Vector3f& b);

    Vector3f vmin(const Vector3f& a,const Vector3f& b);

    float length(const Vector3f& vec);

    Vector3f normalize(const Vector3f& vec);

    float dot(const Vector3f& lhs,const Vector3f& rhs);

    Vector3f cross(const Vector3f& a,const Vector3f& b);

    Vector4f vsub(const Vector4f& a, const Vector4f& b);

    Vector4f vadd(const Vector4f& a, const Vector4f& b);

    Vector4f vmul(const Vector4f& a, float f);

    Vector4f vdiv(const Vector4f& a, float f);

    Vector3f vsub(const Vector3f& a, const Vector3f& b);

    Vector3f vadd(const Vector3f& a, const Vector3f& b);

    Vector3f vmul(const Vector3f& a, float f);

    Vector3f vdiv(const Vector3f& a, float f);

    Vector2f vsub(const Vector2f& a, const Vector2f& b);

    Vector2f vadd(const Vector2f& a, const Vector2f& b);

    Vector2f vmul(const Vector2f& a, float f);

    Vector2f vdiv(const Vector2f& a, float f);

    Mat4x4 multiply(const Mat4x4& a,const Mat4x4& b);

    Vector3f transform_point(const Mat4x4& m,const Vector3f& pt);

    Vector3f transform_vector(const Mat4x4& m,const Vector3f& vc);

    Bound3f bound_merge(const Bound3f& a,const Bound3f& b);

    Bound3f bound_merge(const Bound3f& a,const Vector3f& v);

    Vector3f bound_centorid(const Bound3f& b);

    Mat4x4 transpose(const Mat4x4& m);

    Mat4x4 inverse(const Mat4x4& m);

    Mat4x4 perspective(float aspectRatio,float fov,float near,float far);

    Mat4x4 mat_position(const Vector3f& pos);

    Mat4x4 mat_rotation(const Quaternion& quat);

    Mat4x4 mat_scale(const Vector3f& scale);

    Mat4x4 mat_transform(const Vector3f& pos,const Quaternion& quat,const Vector3f& scale);

    //v0 * (1 - b) + v1 * b
    Vector2f interpolate2(const Vector2f& v0, const Vector2f& v1, float b);

    //v0 * (1 - b) + v1 * b
    Vector3f interpolate2(const Vector3f& v0, const Vector3f& v1, float b);
    
    //v0 * (1 - b) + v1 * b
    Vector4f interpolate2(const Vector4f& v0, const Vector4f& v1, float b);

    //v0 * (1 - u -  v) + v1 * u + v2 * v
    Vector2f interpolate3(const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Vector2f& uv);

    //v0 * (1 - u -  v) + v1 * u + v2 * v
    Vector3f interpolate3(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector2f& uv);

    //v0 * (1 - u -  v) + v1 * u + v2 * v
    Vector4f interpolate3(const Vector4f& v0, const Vector4f& v1, const Vector4f& v2, const Vector2f& uv);

    bool   ray_intersect(const Bound3f& bound,const Ray& r);
    
    //intersect with triangle
    bool   ray_intersect(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Ray& r,
        param_out float* t, param_out Vector2f* uv, param_out Vector3f* position);

    Vector3f color_blend(const Vector3f& c1, const Vector3f& c2);

    bool contains_nan(const Vector2f& v);
    bool contains_nan(const Vector3f& v);
    bool contains_nan(const Vector4f& v);
    
    bool contains_inf(const Vector2f& v);
    bool contains_inf(const Vector3f& v);
    bool contains_inf(const Vector4f& v);
};

//format vector values
#include "spdlog/formatter.h"

template<> struct fmt::formatter<Vector2f> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(Vector2f v, FormatContext& ctx) {
        string res = fmt::format("({},{})", v.x, v.y);
        return formatter<string>::format(res, ctx);
    }
};

template<> struct fmt::formatter<Vector3f> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(Vector3f v, FormatContext& ctx) {
        string res = fmt::format("({},{},{})", v.x, v.y, v.z);
        return formatter<string>::format(res, ctx);
    }
};

template<> struct fmt::formatter<Vector4f> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(Vector4f v, FormatContext& ctx) {
        string res = fmt::format("({},{},{},{})", v.x, v.y, v.z, v.w);
        return formatter<string>::format(res, ctx);
    }
};