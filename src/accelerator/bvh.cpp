#include "bvh.h"


//copied from pbrt
AL_PRIVATE uint32 Left3Shift(uint32 x) {
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

AL_PRIVATE uint32 MortonCode(const Vector3f& v) {
    al_assert(v.x >= 0, "build scene fail: v.x {0} should be greater than 0", v.x);
    al_assert(v.y >= 0, "build scene fail: v.y {0} should be greater than 0", v.y);
    al_assert(v.z >= 0, "build scene fail: v.z {0} should be greater than 0", v.z);
    
    return (Left3Shift((uint32)v.z) << 2) | (Left3Shift((uint32)v.y) << 1) | Left3Shift((uint32)v.x);
}

template<uint32 nBitPerPass>
AL_PRIVATE uint32 CalculateBucket(uint32 code,uint32 pass) {
    constexpr uint32 mask = (1 << nBitPerPass) - 1;
    uint32 lowBit = pass * nBitPerPass;
    return (code >> lowBit) & mask;
}


AL_PRIVATE void RadixSort(vector<BVHPrimitiveInfo>& info) {
    constexpr uint32 nBits = 30;
    constexpr uint32 nBitPerPass = 6;
    constexpr uint32 nPass = nBits / nBitPerPass;
    constexpr uint32 nBucket = 1 << nBitPerPass;

    vector<BVHPrimitiveInfo> tmpBuffer(info.size());
    uint32 nPrimitive = info.size();
    al_for(pass,0,nPass) {
        vector<BVHPrimitiveInfo>& out  = pass % 2 ? info : tmpBuffer;
        vector<BVHPrimitiveInfo>& in = pass % 2 ? tmpBuffer : info;
        
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



AL_PRIVATE uint32 BuildNode(vector<BVHLeafNode>& buildNodes,
    uint32 pStart,uint32 pEnd,vector<BVHPrimitiveInfo>& primitives,int32 bitIndex) {
    //initialize a tree node
    if (bitIndex < 0 || pEnd - pStart <= 1) {
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
            uint32 mid = (searchEnd - searchStart) / 2 + searchStart;
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

uint32 treeDepth(uint32 i, vector<BVHLeafNode>& leafNodes) {
    if (leafNodes[i].isLeaf) {
        return 1;
    }
    else {
        return max(treeDepth(leafNodes[i].lchild, leafNodes),
            treeDepth(leafNodes[i].rchild, leafNodes)) + 1;
    }
}

//generate lbvh
//sort primitives by morton code
//generate hireachy 
//fit bounding-boxes
void BVHTree::Build(const vector<BVHPrimitive>& _primitives,BVHPrimitiveIntersector* intersector) {
    Clear();

    al_assert(intersector != nullptr, "BVHTree::Build : the intersector should not be nullptr");

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
        Vector3f offset = _primitives[i].centorid - maxExtent.lower;
        float mx = offset.x / (maxExtent.upper.x - maxExtent.lower.x);
        float my = offset.y / (maxExtent.upper.y - maxExtent.lower.y);
        float mz = offset.z / (maxExtent.upper.z - maxExtent.lower.z);
        uint32 mortonCode = MortonCode(Vector3f(mx, my, mz) * (float)mortonScale);
        primitives.push_back(BVHPrimitiveInfo(_primitives[i], i, mortonCode));
    }

    RadixSort(primitives);

    constexpr int32 firstBitIndex = 29;
    BuildNode(buildNodes, 0, primitives.size(),
        primitives, firstBitIndex);

    this->intersector = intersector;

    al_log("the depth of bvh is {}", treeDepth(0,buildNodes));
}

//stakless tranverse
bool BVHTree::Intersect(const Ray& r,BVHIntersectInfo& info) {
    constexpr uint32 maxiumDepth = 512;
    uint32 toVisit[maxiumDepth] = { 0 },toVisitCount = 1;
    info.intersection.t = infinity;

    bool intersected = false;

    //delete
    uint32 count = 0;
    //delete end
    while (toVisitCount != 0 && toVisitCount < maxiumDepth) {
        BVHLeafNode& node = leafNodes[toVisit[--toVisitCount]];
    
        //delete
        count++;
        //delete end
        if (Math::ray_intersect(node.bound, r)) {
            if (node.isLeaf) {
                BVHPrimitiveInfo& primInfo = primitiveInfo[node.primitiveIndex];
                al_for(i,0,node.primitiveCount) {
                    if (Math::ray_intersect(primInfo.primitive.aabb, r)) {
                        Intersection isect;
                        bool nodeIsect = intersector->Intersect(r, primInfo.primitiveIndex + i, isect);
                        intersected |= nodeIsect;
                        if (nodeIsect && isect.t < info.intersection.t) {
                            info.primitiveIndex = primInfo.primitiveIndex;
                            info.intersection = isect;
                        }
                    }
                    
                }
            }
            else {
                if (r.d.a[node.axis] > 0) {
                    toVisit[toVisitCount++] = node.rchild;
                    toVisit[toVisitCount++] = node.lchild;
                }
                else {
                    toVisit[toVisitCount++] = node.lchild;
                    toVisit[toVisitCount++] = node.rchild;
                }
            }
        }
    }
    //delete
    if(count > 100)
        //al_log("bvh depth {}",count);
    if (count > 2000) {
        __debugbreak();
    }
    //delete end

    return intersected;
}


void BVHTree::Clear() {
    leafNodes.clear();
    primitiveInfo.clear();
    al_delete(intersector);
}