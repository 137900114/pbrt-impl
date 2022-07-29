#include "gtest/gtest.h"
#include "accelerator/bvh.h"
#include <random>
#include <bitset>

#define OP2(v0,op,v1,v2) EXPECT_TRUE(((v0)op(v1))==v2) << "expected value:" << v2 << " actual value:" << ((v0)op(v1));

#define F1(f,v1,v2) EXPECT_TRUE( (f(v1)==v2)) << "expected value:" << v2 << " actual value:" << f(v1) ;
#define F2(f,v0,v1,v2) EXPECT_TRUE( (f(v0,v1)==v2)) << "expected value:" << v2 << " actual value:" << f(v0,v1);
#define F3(f,v0,v1,v2,v3) EXPECT_TRUE( (f(v0,v1,v2)==v3)) << "expected value:" << v3 << " actual value:" << f(v0,v1,v2);
#define FN(f,res,...) EXPECT_TRUE(f(__VA_ARGS__)==res) << "expected value:" << res << " actual value:" << f(__VA_ARGS__);

#define F1E(f,v1,v2,e) EXPECT_TRUE( e(f(v1),v2)) << "expected value:" << v2 << " actual value:" << f(v1);
#define F2E(f,v0,v1,v2,e) EXPECT_TRUE( e(f(v0,v1),v2)) << "expected value:" << v2 << " actual value:" << f(v0,v1);
#define F3E(f,v0,v1,v2,v3,e) EXPECT_TRUE( e(f(v0,v1,v2),v3)) << "expected value:" << v3 << " actual value:" << f(v0,v1,v2);

void RadixSort(vector<BVHPrimitiveInfo>& info);

BVHPrimitiveInfo P(uint32 i) {
	BVHPrimitiveInfo p;
	p.motornCode = i;
	return p;
}

bool Varify(const vector<BVHPrimitiveInfo>& v) {
	al_for(i,0,v.size() - 1) {
		if (v[i].motornCode > v[i].motornCode) {
			return false;
		}
	}
	return true;
}

bool Varify(const vector<BVHPrimitiveInfo>& v,const vector<BVHPrimitiveInfo>& j) {
	al_for(i,0,v.size() - 1) {
		if (v[i].motornCode != j[i].motornCode) {
			return false;
		}
	}
	return true;
}

struct BVHMsg {
	vector<BVHPrimitiveInfo>& info;
	int32 s, e;
	int32 offset = 3;
	BVHMsg(vector<BVHPrimitiveInfo>& info):info(info) {
		int32 j = -1;
		al_for(i, 0, info.size() - 1) {
			if (info[i].motornCode > info[i].motornCode) {
				j = i;
				break;
			}
		}
		s = 0, e = 0;
		if (j >= 0) {
			s = std::max(j - offset, 0);
			e = std::min(j + offset, (int32)info.size());
		}
	}

	BVHMsg(vector<BVHPrimitiveInfo>& v,const vector<BVHPrimitiveInfo>& m) :info(info) {
		int32 j = -1;
		al_for(i, 0, info.size() - 1) {
			if (m[i].motornCode != v[i].motornCode) {
				j = i;
				break;
			}
		}
		s = 0, e = 0;
		if (j >= 0) {
			s = std::max(j - offset, 0);
			e = std::min(j + offset, (int32)info.size());
		}
	}
};


stringstream& operator<<(stringstream& ss,const BVHMsg& msg) {
	ss << "{";
	if (msg.s != 0) {
		ss << "...";
	}
	al_for(i,msg.s,msg.e) {
		ss << to_string(msg.info[i].motornCode);
		if (i != msg.e) {
			ss << ",";
		}
	}
	if (msg.e != msg.info.size()) {
		ss << "...";
	}
	ss << "}";
	return ss;
}

TEST(Radix, SortTest) {
	uint32 upper = 1 << 30 - 1;

	//manually create numbers
	vector<BVHPrimitiveInfo> prims,ans,prims2,ans2;
	prims.push_back(P(3)),ans.push_back(P(0));
	prims.push_back(P(4)),ans.push_back(P(2));
	prims.push_back(P(2)),ans.push_back(P(3));
	prims.push_back(P(0)),ans.push_back(P(4));

	prims2.push_back(P(upper));
	prims2.push_back(P(upper - 8));
	prims2.push_back(P(upper / 2));
	prims2.push_back(P(upper - 19));
	prims2.push_back(P(upper / 3));
	prims2.push_back(P(upper - 9));


	RadixSort(prims);
	RadixSort(prims2);
	ASSERT_TRUE(Varify(prims,ans)) << "invalid sort result at" << BVHMsg(prims,ans);
	ASSERT_TRUE(Varify(prims2)) << "invalid sort result at" << BVHMsg(prims2);


	//randomly generated 1000 cases
	al_for(i, 0, 1000) {
		std::uniform_int_distribution<uint32> distribution(0, upper);
		uint32 size = 100;
		std::default_random_engine seed_generator(time(NULL));
		uint32 seed = distribution(seed_generator);
		std::default_random_engine generator(seed);
		

		vector<BVHPrimitiveInfo> tCase;
		al_for(i,0,size) {
			tCase.push_back(P(distribution(generator)));
		}
		RadixSort(tCase);
		EXPECT_TRUE(Varify(tCase)) << "random test case fails at " << BVHMsg(tCase) << "at seed " << seed;
	}
}

uint32 MortonCode(const Vector3f& v);
uint32 Left3Shift(uint32 x);
TEST(MortonCodeGenerate,MortonCodeGenerate) {
	uint32 u = 0b1111111111,r = 0b00001001001001001001001001001001;
	EXPECT_TRUE(Left3Shift(u) == r) << "expected value " << bitset<32>(r) << " actual get" << bitset<32>(Left3Shift(u));
	
	F1(MortonCode, Vector3f(1.f, 0.f, 0.f), 1);
	F1(MortonCode, Vector3f(0.f, 1.f, 0.f), 2);
	F1(MortonCode, Vector3f(0.f, 0.f, 1.f), 4);
	F1(MortonCode, Vector3f(4.f, 1.f, 1.f), 70);
}



int main() {
	testing::InitGoogleTest();
	RUN_ALL_TESTS();
}