#include "math.hpp"

namespace Math {

    Vector3f vmax(const Vector3f& a,const Vector3f& b) {
        return Vector3f(a.x > b.x ? a.x : b.x,
                        a.y > b.y ? a.y : b.y,
                        a.z > b.z ? a.z : b.z);
    }

    Vector3f vmin(const Vector3f& a,const Vector3f& b) {
        return Vector3f(a.x < b.x ? a.x : b.x,
                        a.y < b.y ? a.y : b.y,
                        a.z < b.z ? a.z : b.z);
    }


    float length(const Vector3f& vec) {
        //TODO: can be optimized
        return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    }

    Vector3f normalize(const Vector3f& vec) {
        float len = length(vec);
        return Vector3f(vec.x / len, vec.y / len, vec.z / len);
    }

    float dot(const Vector3f& lhs,const Vector3f& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    Vector3f cross(const Vector3f& a,const Vector3f& b) {
        return Vector3f(a.y * b.z - a.z * b.y,
                        a.z * b.x - a.x * b.z,
                        a.x * b.y - a.y * b.x);
    }

    Mat4x4 multiply(const Mat4x4& a,const Mat4x4& b) {
        float r[4][4];
        r[0][0] = a.a[0][0] * b.a[0][0] + a.a[0][1] * b.a[1][0] + a.a[0][2] * b.a[2][0] + a.a[0][3] * b.a[3][0];
        r[0][1] = a.a[0][0] * b.a[0][1] + a.a[0][1] * b.a[1][1] + a.a[0][2] * b.a[2][1] + a.a[0][3] * b.a[3][1];
        r[0][2] = a.a[0][0] * b.a[0][2] + a.a[0][1] * b.a[1][2] + a.a[0][2] * b.a[2][2] + a.a[0][3] * b.a[3][2];
        r[0][3] = a.a[0][0] * b.a[0][3] + a.a[0][1] * b.a[1][3] + a.a[0][2] * b.a[2][3] + a.a[0][3] * b.a[3][3];


        r[1][0] = a.a[1][0] * b.a[0][0] + a.a[1][1] * b.a[1][0] + a.a[1][2] * b.a[2][0] + a.a[1][3] * b.a[3][0];
        r[1][1] = a.a[1][0] * b.a[0][1] + a.a[1][1] * b.a[1][1] + a.a[1][2] * b.a[2][1] + a.a[1][3] * b.a[3][1];
        r[1][2] = a.a[1][0] * b.a[0][2] + a.a[1][1] * b.a[1][2] + a.a[1][2] * b.a[2][2] + a.a[1][3] * b.a[3][2];
        r[1][3] = a.a[1][0] * b.a[0][3] + a.a[1][1] * b.a[1][3] + a.a[1][2] * b.a[2][3] + a.a[1][3] * b.a[3][3];

        r[2][0] = a.a[2][0] * b.a[0][0] + a.a[2][1] * b.a[1][0] + a.a[2][2] * b.a[2][0] + a.a[2][3] * b.a[3][0];
        r[2][1] = a.a[2][0] * b.a[0][1] + a.a[2][1] * b.a[1][1] + a.a[2][2] * b.a[2][1] + a.a[2][3] * b.a[3][1];
        r[2][2] = a.a[2][0] * b.a[0][2] + a.a[2][1] * b.a[1][2] + a.a[2][2] * b.a[2][2] + a.a[2][3] * b.a[3][2];
        r[2][3] = a.a[2][0] * b.a[0][3] + a.a[2][1] * b.a[1][3] + a.a[2][2] * b.a[2][3] + a.a[2][3] * b.a[3][3];

        r[3][0] = a.a[3][0] * b.a[0][0] + a.a[3][1] * b.a[1][0] + a.a[3][2] * b.a[2][0] + a.a[3][3] * b.a[3][0];
        r[3][1] = a.a[3][0] * b.a[0][1] + a.a[3][1] * b.a[1][1] + a.a[3][2] * b.a[2][1] + a.a[3][3] * b.a[3][1];
        r[3][2] = a.a[3][0] * b.a[0][2] + a.a[3][1] * b.a[1][2] + a.a[3][2] * b.a[2][2] + a.a[3][3] * b.a[3][2];
        r[3][3] = a.a[3][0] * b.a[0][3] + a.a[3][1] * b.a[1][3] + a.a[3][2] * b.a[2][3] + a.a[3][3] * b.a[3][3];

        return Mat4x4(r[0][0], r[0][1], r[0][2], r[0][3],
                      r[1][0], r[1][1], r[1][2], r[1][3], 
                      r[2][0], r[2][1], r[2][2], r[2][3], 
                      r[3][0], r[3][1], r[3][2], r[3][3]);
    }

    Vector3f transform_point(const Mat4x4& m,const Vector3f& pt) {
        constexpr float p = 1.0f;
        float r[3];
        r[0] = m.a[0][0] * pt.x + m.a[0][1] * pt.y + m.a[0][2] * pt.z + m.a[0][3] * p;
        r[1] = m.a[1][0] * pt.x + m.a[1][1] * pt.y + m.a[1][2] * pt.z + m.a[1][3] * p;
        r[2] = m.a[2][0] * pt.x + m.a[2][1] * pt.y + m.a[2][2] * pt.z + m.a[2][3] * p;
        return Vector3f(r[0], r[1], r[2]);
    }

    Vector3f transform_vector(const Mat4x4& m,const Vector3f& vc) {
        constexpr float p = 0.0f;
        float r[3];
        r[0] = m.a[0][0] * vc.x + m.a[0][1] * vc.y + m.a[0][2] * vc.z + m.a[0][3] * p;
        r[1] = m.a[1][0] * vc.x + m.a[1][1] * vc.y + m.a[1][2] * vc.z + m.a[1][3] * p;
        r[2] = m.a[2][0] * vc.x + m.a[2][1] * vc.y + m.a[2][2] * vc.z + m.a[2][3] * p;
        return Vector3f(r[0], r[1], r[2]);
    }

    Bound3f bound_merge(const Bound3f& a,const Bound3f& b) {
        
        return Bound3f(vmin(a.lower, b.lower), vmax(a.upper, b.upper));
    }

    Bound3f bound_merge(const Bound3f& a,const Vector3f& v) {
        return Bound3f(vmin(a.lower, v), vmax(a.upper, v));
    }

    Mat4x4 transpose(const Mat4x4& m) {
        return Mat4x4(m.a00, m.a10, m.a20, m.a30,
                      m.a01, m.a11, m.a21, m.a31, 
                      m.a02, m.a12, m.a22, m.a32, 
                      m.a03, m.a13, m.a23, m.a33);
    }

    //code from https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse
    Mat4x4 inverse(const Mat4x4& _m) {
        Mat4x4 mat;
        Mat4x4 m = _m;

        for (unsigned column = 0; column < 4; ++column) {
            // Swap row in case our pivot point is not working
            if (m.a[column][column] == 0) {
                unsigned big = column;
                for (unsigned row = 0; row < 4; ++row)
                    if (fabs(m.a[row][column]) > fabs(m.a[big][column])) big = row;
                // Print this is a singular matrix, return identity ?
                al_assert(big != column, "Math::inverse fail to inverse the matrix the matrix should not be singular");
                // Swap rows
                for (unsigned j = 0; j < 4; ++j) {
                    std::swap(m.a[column][j], m.a[big][j]);
                    std::swap(mat.a[column][j], mat.a[big][j]);
                }
            }
            // Set each row in the column to 0  
            for (unsigned row = 0; row < 4; ++row) {
                if (row != column) {
                    float coeff = m.a[row][column] / m.a[column][column];
                    if (coeff != 0) {
                        for (unsigned j = 0; j < 4; ++j) {
                            m.a[row][j] -= coeff * m.a[column][j];
                            mat.a[row][j] -= coeff * mat.a[column][j];
                        }
                        // Set the element to 0 for safety
                        m.a[row][column] = 0;
                    }
                }
            }
        }
        // Set each element of the diagonal to 1
        for (unsigned row = 0; row < 4; ++row) {
            for (unsigned column = 0; column < 4; ++column) {
                mat.a[row][column] /= m.a[row][row];
            }
        }

        return mat;
    }

    Mat4x4 projection(float aspectRatio, float fov, float near, float far) {
        float ta = tanf(fov * .5f);
        return Mat4x4(
            1.f / (aspectRatio * ta), 0.0f      , 0.0f                      , 0.0f,
            0.0f                    , 1.f / ta  , 0.0f                      , 0.0f,
            0.0f                    , 0.0f      , far / (far - near)        , 1.0f,
            0.0f                    , 0.0f      , near * far / (near - far) , 0.0f
        );
    }

    Mat4x4 mat_position(const Vector3f& pos) {
        return Mat4x4(
            1.0f, 0.0f, 0.0f, pos.x,
            0.0f, 1.0f, 0.0f, pos.y,
            0.0f, 0.0f, 1.0f, pos.z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Mat4x4 mat_rotation(const Quaternion& quat) {
        float x = quat.val.x, y = quat.val.y, z = quat.val.z,
            w = quat.val.w;
        float x2 = x * x, y2 = y * y, z2 = z * z;
        float xy = x * y, xz = x * z, yz = y * z;
        float wx = x * w, wy = w * y, wz = w * z;
        return Mat4x4(
            1 - 2 * (y2 + z2)   , 2 * (xy - wz)     , 2 * (xz + wy)     , 1,
            2 * (xy + wz)       , 1 - 2 * (x2 + z2) , 2 * (yz - wx)     , 1,
            2 * (xz - wy)       , 2 * (yz + wx)     , 1 - 2 * (x2 + y2) , 1,
            0                   , 0                 , 0                 , 0
        );
    }

    Mat4x4 mat_scale(const Vector3f& scale) {
        return Mat4x4(
            scale.x , 0         , 0         , 1,
            0       , scale.y   , 0         , 1,
            0       , 0         , scale.z   , 1,
            0       , 0         ,0          , 1
        );
    }

    Mat4x4 mat_transform(const Vector3f& pos, const Quaternion& quat, const Vector3f& scale) {
        Mat4x4 res = mat_rotation(quat);
        res.a00 *= scale.x, res.a01 *= scale.x, res.a02 *= scale.x,res.a03 = pos.x;
        res.a10 *= scale.y, res.a11 *= scale.y, res.a12 *= scale.y,res.a13 = pos.y;
        res.a20 *= scale.z, res.a21 *= scale.z, res.a22 *= scale.z,res.a23 = pos.z;
        return res;
    }

    Vector3f vsub(const Vector3f& a, const Vector3f& b) {
        return Vector3f(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    Vector3f vadd(const Vector3f& a, const Vector3f& b) {
        return Vector3f(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    Vector3f vmul(const Vector3f& a, float f) {
        return Vector3f(a.x * f, a.y * f, a.z * f);
    }

    Vector3f vdiv(const Vector3f& a, float f) {
        al_assert(al_fequal(f, 0), "divide zero error");
        return Vector3f(a.x / f, a.y / f, a.z / f);
    }

    Vector3f bound_centorid(const Bound3f& b) {
        float x = (b.upper.x + b.lower.x) * .5f;
        float y = (b.upper.y + b.lower.y) * .5f;
        float z = (b.upper.z + b.lower.z) * .5f;
        return Vector3f(x, y, z);
    }


    bool   ray_intersect(const Bound3f& bound, const Ray& r) {
        Vector3f o = Math::vadd(r.o, Math::vmul(r.d, t));
        
        uint32 sign[3];
        sign[0] = r.d.x > 0, sign[1] = r.d.y > 0, sign[2] = r.d.z > 0;
        float invx = 1.f / r.d.x, invy = 1.f / r.d.y, invz = 1.f / r.d.z;

        float txmin = (bound.bound[sign[0]].x - o.x) * invx;
        float txmax = (bound.bound[1 - sign[0]].x - o.x) * invx;
        float tymin = (bound.bound[sign[1]].y - o.y) * invy;
        float tymax = (bound.bound[1 - sign[1]].y - o.y) * invy;
        float tzmin = (bound.bound[sign[2]].z - o.z) * invz;
        float tzmax = (bound.bound[1 - sign[2]].z - o.z) * invz;

        if ((txmin > tymax) || (tymin > txmax)) return false;
        float tmax = std::fmax(txmax, tymax), tmin = std::fmin(txmin, tymin);

        if ((tmax < tzmin) || (tzmax < tmin)) return false;
        
        return true;
    }

    bool   ray_intersect(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Ray& r,
        param_out float* t, param_out Vector2f* uv, param_out Vector3f* position) {

    }
};