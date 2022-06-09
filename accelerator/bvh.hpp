#pragma once
#include "common/common.hpp"
#include "math/math.hpp"

struct BVHPrimitive{
    Bound3f aabb;
    Vector3f centorid;

    BVHPrimitive(const Bound3f& aabb):aabb(aabb) {
        centorid = Math::bound_centorid(aabb);
    }

    BVHPrimitive() {}
};

struct BVHLeafNode;

struct BVHPrimitiveInfo {
    BVHPrimitive primitive;
    uint32       primitiveIndex;
    uint32       motornCode;

    BVHPrimitiveInfo(BVHPrimitive primitive, uint32 primitiveIndex, uint32 code) :
        primitive(primitive), primitiveIndex(primitiveIndex), motornCode(code)
    {}

    BVHPrimitiveInfo() :motornCode(0), primitiveIndex(0) {}
};

struct BVHIntersectInfo {
    uint32 primitiveIndex;
    Intersection intersection;
    BVHIntersectInfo(const Intersection& inter,uint32 primitiveIndex) :
        primitiveIndex(primitiveIndex),intersection(inter) {}
    BVHIntersectInfo() :primitiveIndex(std::numeric_limits<uint32>::max()) { intersection.intersected = false; }

    bool Intersected() {
        return intersection.intersected;
    }
};

class BVHPrimitiveIntersector {
public:
    virtual Intersection Intersect(const Ray& r,uint32 primitiveIndex) = 0;
};

//use LBVH method by default
class BVHTree {
public:
    BVHTree(){}

    void Build(const vector<BVHPrimitive>& primitives,ptr<BVHPrimitiveIntersector> intersector);
    void Clear();
    BVHIntersectInfo Intersect(const Ray& p);


private:
    vector<BVHLeafNode>             leafNodes;
    vector<BVHPrimitiveInfo>        primitiveInfo;
    ptr<BVHPrimitiveIntersector>    intersector;
};