#pragma once
#include "common/common.h"
#include "math/math.h"

struct BVHPrimitive{
    Bound3f aabb;
    Vector3f centorid;

    BVHPrimitive(const Bound3f& aabb):aabb(aabb) {
        centorid = Math::bound_centorid(aabb);
    }

    BVHPrimitive() {}
};

struct BVHLeafNode {
    int axis;
    bool isLeaf;
    uint32 lchild, rchild;
    Bound3f bound;
    uint32 primitiveIndex, primitiveCount;
};

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
    bool intersected;
    BVHIntersectInfo(const Intersection& inter,uint32 primitiveIndex) :
        primitiveIndex(primitiveIndex),intersection(inter) {}
    BVHIntersectInfo() :primitiveIndex(std::numeric_limits<uint32>::max()) { intersected = false; }

    bool Intersected() {
        return intersected;
    }
};

class BVHPrimitiveIntersector {
public:
    virtual bool Intersect(const Ray& r,uint32 primitiveIndex,Intersection& isect) = 0;
};

//use LBVH method by default
class BVHTree {
public:
    BVHTree():intersector(nullptr){}

    void Build(const vector<BVHPrimitive>& primitives,BVHPrimitiveIntersector* intersector);
    void Clear();
    bool Intersect(const Ray& p,BVHIntersectInfo& isect);


private:
    vector<BVHLeafNode>             leafNodes;
    vector<BVHPrimitiveInfo>        primitiveInfo;
    BVHPrimitiveIntersector*    intersector;
};