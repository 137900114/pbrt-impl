#include "bvh.hpp"

//cpu bvh generating
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

    BVHPrimitiveInfo(BVHPrimitive primitive,uint32 primitiveIndex,uint32 code):
        primitive(primitive),primitiveIndex(primitiveIndex),motornCode(code)
    {}

    BVHPrimitiveInfo():motornCode(0),primitiveIndex(0) {}
};


//copied from pbrt
static uint32 Left3Shift(uint32 x) {
    al_assert(x <= (1 << 10),"build scene failed x:{0} should less than {1}",x,(1 << 10));
    if (x == (1 << 10)) --x;
    x = (x | (x << 16)) & 0b00000011000000000000000011111111;
    // x = ---- --98 ---- ---- ---- ---- 7654 3210
    x = (x | (x << 8)) & 0b00000011000000001111000000001111;
    // x = ---- --98 ---- ---- 7654 ---- ---- 3210
    x = (x | (x << 4)) & 0b00000011000011000011000011000011;
    // x = ---- --98 ---- 76-- --54 ---- 32-- --10
    x = (x | (x << 2)) & 0b00001001001001001001001001001001;
    // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
    return x;
}

static uint32 MortonCode(const Vector3f& v) {
    al_assert(v.x > 0, "build scene fail: v.x {0} should be greater than 0", v.x);
    al_assert(v.y > 0, "build scene fail: v.y {0} should be greater than 0", v.y);
    al_assert(v.z > 0, "build scene fail: v.z {0} should be greater than 0", v.z);
    
    return (Left3Shift((uint32)v.z) << 2) | (Left3Shift((uint32)v.y) << 1) | Left3Shift((uint32)v.x);
}

template<uint32 nBitPerPass>
static uint32 CalculateBucket(uint32 code,uint32 pass) {
    constexpr uint32 mask = (1 << nBitPerPass) - 1;
    uint32 lowBit = pass * nBitPerPass;
    return (code >> lowBit) & mask;
}


static void RadixSort(vector<BVHPrimitiveInfo>& info) {
    constexpr uint32 nBits = 30;
    constexpr uint32 nBitPerPass = 6;
    constexpr uint32 nPass = nBits / nBitPerPass;
    constexpr uint32 nBucket = 1 << nBitPerPass;

    vector<BVHPrimitiveInfo> tmpBuffer(info.size());
    uint32 nPrimitive = info.size();
    al_for(pass,0,nPass) {
        vector<BVHPrimitiveInfo>& in  = pass % 2 ? info : tmpBuffer;
        vector<BVHPrimitiveInfo>& out = pass % 2 ? tmpBuffer : info;
        
        uint32 bucketSize[nBucket] = { 0 };
        al_for(i, 0, nPrimitive) {
            uint32 bucket = CalculateBucket<nBitPerPass>(in[i].motornCode, pass);
            bucketSize[bucket]++;
        }
        uint32 bucketIndex[nBucket] = { 0 };
        al_for(i, 1, nBucket) {
            bucketIndex[i] = bucketSize[i - 1] + bucketIndex[i - 1];
        }
        al_for(i,0,nPrimitive) {
            uint32 bucket = CalculateBucket<nBitPerPass>(in[i].motornCode, pass);
            out[bucketIndex[bucket]++] = in[i];
        }
    }
    if (nPass & 1) std::swap(info, tmpBuffer);
}



static uint32 BuildNode(vector<BVHLeafNode>& buildNodes,
    uint32 pStart,uint32 pEnd,vector<BVHPrimitiveInfo>& primitives,uint32 bitIndex) {
    //initialize a tree node
    if (bitIndex < 0) {
        buildNodes.push_back(BVHLeafNode());
        uint32 currNodeIndex = buildNodes.size() - 1;

        buildNodes[currNodeIndex].lchild = buildNodes[currNodeIndex].rchild = 0;
        buildNodes[currNodeIndex].primitiveCount = pEnd - pStart;
        buildNodes[currNodeIndex].primitiveIndex = pStart;
        buildNodes[currNodeIndex].isLeaf = true;
        Bound3f bound;
        al_for(i,pStart,pEnd) {
            bound = Math::bound_merge(bound, primitives[i].primitive.aabb);
        }
        buildNodes[currNodeIndex].bound = bound;

        return currNodeIndex;
    }
    //partition the space
    else {
        //on the same level
        uint32 highestBitMask = 1 << bitIndex;
        if ((primitives[pStart].motornCode & highestBitMask)
            == (primitives[pEnd - 1].motornCode  & highestBitMask)) {
            return BuildNode(buildNodes, pStart, pEnd, primitives, bitIndex - 1);
        }

        buildNodes.push_back(BVHLeafNode());
        uint32 currNodeIndex = buildNodes.size() - 1;

        uint32 searchStart = pStart, searchEnd = pEnd;
        while (searchStart  + 1 < searchEnd) {
            uint32 mid = (searchStart - searchEnd) / 2 + searchStart;
            if ((primitives[pStart].motornCode & highestBitMask)
                == (primitives[mid].motornCode & highestBitMask)) {
                searchStart = mid;
            }
            else {
                searchEnd = mid;
            }
        }
        uint32 pPartition = searchEnd;

        uint32 lchild = BuildNode(buildNodes, pStart, pPartition,
            primitives, bitIndex - 1);
        uint32 rchild = BuildNode(buildNodes, pPartition, pEnd,
            primitives, bitIndex - 1);
        
        buildNodes[currNodeIndex].axis = bitIndex % 3;
        buildNodes[currNodeIndex].lchild = lchild;
        buildNodes[currNodeIndex].rchild = rchild;
        buildNodes[currNodeIndex].bound = Math::bound_merge(buildNodes[rchild].bound,
            buildNodes[lchild].bound);
        buildNodes[currNodeIndex].isLeaf = false;

        return currNodeIndex;
    }
}



//generate lbvh
//sort primitives by morton code
//generate hireachy 
//fit bounding-boxes
void BVHTree::Build(const vector<BVHPrimitive>& _primitives,ptr<BVHPrimitiveIntersector> intersector) {
    Clear();

    al_assert(intersector == nullptr, "BVHTree::Build : the intersector should not be nullptr");

    vector<BVHPrimitiveInfo>& primitives = primitiveInfo;
    vector<BVHLeafNode>&      buildNodes = leafNodes;
    uint32 primitiveCount = _primitives.size(),
        expectedNodeCount = primitiveCount * 2;
    
    primitives.reserve(primitiveCount);
    
    //the biggest extension of the scene
    Bound3f  maxExtent;
    //the center point of the scene
    Vector3f centorid;
    al_for(i, 0, primitiveCount) {
        maxExtent = Math::bound_merge(maxExtent, _primitives[i].aabb);
    }
    centorid = Math::bound_centorid(maxExtent);


    al_for(i, 0, primitiveCount) {
        constexpr uint32 mortonScale = 1 << 10;
        Vector3f offset = Math::vsub(_primitives[i].centorid, centorid);
        float mx = offset.x / (maxExtent.upper.x - maxExtent.lower.x);
        float my = offset.y / (maxExtent.upper.y - maxExtent.lower.y);
        float mz = offset.z / (maxExtent.upper.z - maxExtent.lower.z);
        uint32 mortonCode = MortonCode(Math::vmul(Vector3f(mx, my, mz), (float)mortonScale));
        primitives.push_back(BVHPrimitiveInfo(_primitives[i], i, mortonCode));
    }

    RadixSort(primitives);

    constexpr int32 firstBitIndex = 29;
    BuildNode(buildNodes, 0, buildNodes.size(),
        primitives, firstBitIndex);
}

//stakless tranverse
BVHIntersectInfo BVHTree::Intersect(const Ray& r) {
    constexpr uint32 maxiumDepth = 64;
    uint32 toVisit[maxiumDepth] = { 0 },toVisitCount = 1;

    BVHIntersectInfo info;

    while (toVisitCount != 0) {
        BVHLeafNode& node = leafNodes[--toVisitCount];
        if (Math::ray_intersect(node.bound,p)) {
            if (node.isLeaf) {
                al_for(i,0,node.primitiveCount) {
                    BVHPrimitiveInfo& primInfo = primitiveInfo[node.primitiveIndex + i];
                    intersector->Intersect(r, primInfo.primitiveIndex);
                    
                }
            }
            else {

            }
        }
    }

    return info;
}


void BVHTree::Clear() {
    leafNodes.clear();
    primitiveInfo.clear();
    intersector = nullptr;
}