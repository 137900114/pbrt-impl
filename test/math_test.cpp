#include "math/math.h"
#include "gtest/gtest.h"

#define OP2(v0,op,v1,v2) EXPECT_TRUE(((v0)op(v1))==v2) << "expected value:" << v2 << " actual value:" << ((v0)op(v1));

#define F1(f,v1,v2) EXPECT_TRUE( (f(v1)==v2)) << "expected value:" << v2 << " actual value:" << f(v1) ;
#define F2(f,v0,v1,v2) EXPECT_TRUE( (f(v0,v1)==v2)) << "expected value:" << v2 << " actual value:" << f(v0,v1);
#define F3(f,v0,v1,v2,v3) EXPECT_TRUE( (f(v0,v1,v2)==v3)) << "expected value:" << v3 << " actual value:" << f(v0,v1,v2);
#define FN(f,res,...) EXPECT_TRUE(f(__VA_ARGS__)==res) << "expected value:" << res << " actual value:" << f(__VA_ARGS__);

#define F1E(f,v1,v2,e) EXPECT_TRUE( e(f(v1),v2)) << "expected value:" << v2 << " actual value:" << f(v1);
#define F2E(f,v0,v1,v2,e) EXPECT_TRUE( e(f(v0,v1),v2)) << "expected value:" << v2 << " actual value:" << f(v0,v1);
#define F3E(f,v0,v1,v2,v3,e) EXPECT_TRUE( e(f(v0,v1,v2),v3)) << "expected value:" << v3 << " actual value:" << f(v0,v1,v2);


bool operator==(const Vector4f& a, const Vector4f& b) {
	return al_fequal(a.x, b.x) && al_fequal(a.y, b.y) && al_fequal(a.z, b.z)
		&& al_fequal(a.w, b.w);
}

bool operator==(const Vector3f& a,const Vector3f& b) {
	return al_fequal(a.x, b.x) && al_fequal(a.y, b.y) && al_fequal(a.z, b.z);
}

bool operator==(const Vector2f& a, const Vector2f& b) {
	return al_fequal(a.x, b.x) && al_fequal(a.y, b.y);
}

TEST(FloatPoint, FloatPoint) {
	uint32 nan = 0x7fffffff;
	EXPECT_TRUE(al_fequal(1.f, 1.f));
	EXPECT_FALSE(al_fequal(1.f, 0.f)) << "1.f should not equal to 0.f";
	EXPECT_TRUE(Math::isNan(*(float*)(&nan)));

	//TODO test contains_nan

	Vector2f f1(infinity + infinity, 0), f2(0, infinity + infinity);
	Vector3f f3(infinity + infinity, 0, 0), f4(0, infinity + infinity, 0), f5(0, 0, infinity + infinity);
	Vector4f f6(infinity + infinity, 0, 0, 0), f7(0, infinity + infinity, 0, 0), f8(0, 0, infinity + infinity, 0), f9(0, 0, 0, infinity + infinity);
	EXPECT_TRUE(Math::contains_inf(f1));
	EXPECT_TRUE(Math::contains_inf(f2));
	EXPECT_TRUE(Math::contains_inf(f3));
	EXPECT_TRUE(Math::contains_inf(f4));
	EXPECT_TRUE(Math::contains_inf(f5));
	EXPECT_TRUE(Math::contains_inf(f6));
	EXPECT_TRUE(Math::contains_inf(f7));
	EXPECT_TRUE(Math::contains_inf(f8));
	EXPECT_TRUE(Math::contains_inf(f9));

	//angle function
	float piOver4 = Math::pi / 4.f;
	float piOver2 = Math::pi / 2.f;
	float pi3Over4 = Math::pi * 3.f / 4.f;
	float pi = Math::pi;
	float pi5Over4 = Math::pi * 5.f / 4.f;
	float pi3Over2 = Math::pi * 3.f / 2.f;
	float pi7Over4 = Math::pi * 7.f / 4.f;
	F2E(Math::angle, sinf(piOver4), cosf(piOver4), piOver4,al_fequal);
	F2E(Math::angle, sinf(piOver2), cosf(piOver2), piOver2, al_fequal);
	F2E(Math::angle, sinf(pi3Over4), cosf(pi3Over4), pi3Over4, al_fequal);
	F2E(Math::angle, sinf(pi), cosf(pi), pi, al_fequal);
	F2E(Math::angle, sinf(pi5Over4), cosf(pi5Over4), pi5Over4, al_fequal);
	F2E(Math::angle, sinf(pi3Over2), cosf(pi3Over2), pi3Over2, al_fequal);
	F2E(Math::angle, sinf(pi7Over4), cosf(pi7Over4), pi7Over4, al_fequal);
}

stringstream& operator<<(stringstream& ss,const Vector2f& v) {
	ss << "(" << to_string(v.x) << "," << to_string(v.y) << ")";
	return ss;
}

stringstream& operator<<(stringstream& ss, const Vector3f& v) {
	ss << "(" << to_string(v.x) << "," << to_string(v.y) << "," << to_string(v.z) << ")";
	return ss;
}

stringstream& operator<<(stringstream& ss, const Vector4f& v) {
	ss << "(" << to_string(v.x) << "," << to_string(v.y) << "," << to_string(v.z) << "," << to_string(v.w) << ")";
	return ss;
}


TEST(VectorTest,VectorTest) {
	//add operator
	OP2(Vector2f(1.f, 2.f),+, Vector2f(3.f, 4.f),Vector2f(4.f, 6.f));
	OP2(Vector3f(1.f, 2.f, 3.f), + ,Vector3f(4.f, 5.f, 6.f),Vector3f(5.f, 7.f, 9.f));
	OP2(Vector4f(1.f, 2.f, 3.f, 4.f),+, Vector4f(5.f, 6.f, 7.f, 8.f), Vector4f(6.f, 8.f, 10.f, 12.f));
	OP2(Vector2f(1.f, 2.f), +, 1.f, Vector2f(2.f, 3.f));
	OP2(Vector3f(1.f, 2.f, 3.f), +, 1.f, Vector3f(2.f, 3.f, 4.f));
	OP2(Vector4f(1.f, 2.f, 3.f, 4.f), +, 1.f, Vector4f(2.f, 3.f, 4.f, 5.f));
	OP2(1.f, +, Vector2f(1.f, 2.f), Vector2f(2.f, 3.f));
	OP2(1.f, +, Vector3f(1.f, 2.f, 3.f), Vector3f(2.f, 3.f, 4.f));
	OP2(1.f, +, Vector4f(1.f, 2.f, 3.f, 4.f), Vector4f(2.f, 3.f, 4.f, 5.f));

	//subtract operator
	OP2(Vector2f(1.f, 2.f), -,Vector2f(3.f, 4.f), Vector2f(-2.f, -2.f));
	OP2(Vector3f(1.f, 2.f, 3.f) ,-, Vector3f(4.f, 5.f, 6.f), Vector3f(-3.f, -3.f, -3.f));
	OP2(Vector4f(1.f, 2.f, 3.f, 4.f) ,- ,Vector4f(5.f, 6.f, 7.f, 8.f), Vector4f(-4.f, -4.f, -4.f, -4.f));
	OP2(Vector2f(1.f, 2.f), -, 1.f, Vector2f(0.f, 1.f));
	OP2(Vector3f(1.f, 2.f, 3.f), -, 1.f, Vector3f(0.f, 1.f, 2.f));
	OP2(Vector4f(1.f, 2.f, 3.f, 4.f), -, 1.f, Vector4f(0.f, 1.f, 2.f, 3.f));
	OP2(1.f, -, Vector2f(1.f, 2.f), Vector2f(0.f, -1.f));
	OP2(1.f, -, Vector3f(1.f, 2.f, 3.f), Vector3f(0.f, -1.f, -2.f));
	OP2(1.f, -, Vector4f(1.f, 2.f, 3.f, 4.f), Vector4f(0.f, -1.f, -2.f, -3.f));

	//multiply operator
	OP2(Vector2f(1.f, 2.f) ,*, Vector2f(3.f, 4.f), Vector2f(3.f, 8.f));
	OP2(Vector3f(1.f, 2.f, 3.f) ,*, Vector3f(4.f, 5.f, 6.f), Vector3f(4.f, 10.f, 18.f));
	OP2(Vector4f(1.f, 2.f, 3.f, 4.f) ,*, Vector4f(5.f, 6.f, 7.f, 8.f), Vector4f(5.f, 12.f, 21.f, 32.f));

	//divide operator
	OP2(Vector2f(4.f, 6.f) ,/, Vector2f(2.f, 2.f), Vector2f(2.f, 3.f));
	OP2(Vector3f(4.f, 6.f, 8.f) ,/, Vector3f(2.f, 2.f, 2.f), Vector3f(2.f, 3.f, 4.f));
	OP2(Vector4f(4.f, 6.f, 8.f, 10.f) ,/, Vector4f(2.f, 2.f, 2.f, 2.f), Vector4f(2.f, 3.f, 4.f, 5.f));

	//multiply operator
	OP2(Vector2f(1.f, 2.f) ,*, 2.f, Vector2f(2.f, 4.f));
	OP2(Vector3f(1.f, 2.f, 3.f) ,*, 2.f, Vector3f(2.f, 4.f, 6.f));
	OP2(Vector4f(1.f, 2.f, 3.f, 4.f) ,*, 2.f, Vector4f(2.f, 4.f, 6.f, 8.f));

	//multiply operator
	OP2(2.f ,*, Vector2f(1.f, 2.f), Vector2f(2.f, 4.f));
	OP2(2.f ,*, Vector3f(1.f, 2.f, 3.f), Vector3f(2.f, 4.f, 6.f));
	OP2(2.f ,*, Vector4f(1.f, 2.f, 3.f, 4.f), Vector4f(2.f, 4.f, 6.f, 8.f));

	// divide operator
	OP2(Vector2f(4.f, 6.f) ,/, 2.f, Vector2f(2.f, 3.f));
	OP2(Vector3f(4.f, 6.f, 8.f) ,/, 2.f, Vector3f(2.f, 3.f, 4.f));
	OP2(Vector4f(4.f, 6.f, 8.f, 10.f) ,/, 2.f, Vector4f(2.f, 3.f, 4.f, 5.f));

	//dot function
	F2(Math::dot, Vector3f(1.f, 2.f, 3.f), Vector3f(4.f, 3.f, 2.f), 16.f);
	//normalize function
	F1(Math::normalize, Vector3f(1.f, 2.f, 3.f),   Vector3f(0.267261241912424384684f, 
		0.53452248382484876936910696175951f, 0.80178372573727315405366044263927f));
	//length function
	F1(Math::length, Vector3f(1.f, 2.f, 3.f), 3.7416573867739413855914974646331f);
	
	//cross function
	F2(Math::cross, Vector3f(1.f, 0.f, 0.f), Vector3f(0.f, 1.f, 0.f), Vector3f(0, 0, 1));
	F2(Math::cross, Vector3f(1.f, 2.f, 3.f), Vector3f(4.f, 5.f, 6.f), Vector3f(-3.f, 6.f, -3.f));

	//vmax function
	F2(Math::vmax, Vector3f(1.f, 2.f, 3.f), Vector3f(-1.f, -2.f, -3.f), Vector3f(1.f, 2.f, 3.f));
	F2(Math::vmax, Vector3f(1.f, -2.f, -3.f), Vector3f(-1.f, 2.f, 3.f), Vector3f(1.f, 2.f, 3.f));
	F2(Math::vmax, Vector3f(-1.f, 2.f, -3.f), Vector3f(1.f, -2.f, 3.f), Vector3f(1.f, 2.f, 3.f));
	F2(Math::vmax, Vector3f(-1.f, -2.f, 3.f), Vector3f(1.f, 2.f, -3.f), Vector3f(1.f, 2.f, 3.f));

	//vmin function
	F2(Math::vmin, Vector3f(1.f, 2.f, 3.f), Vector3f(-1.f, -2.f, -3.f), Vector3f(-1.f, -2.f, -3.f));
	F2(Math::vmin, Vector3f(1.f, -2.f, -3.f), Vector3f(-1.f, 2.f, 3.f), Vector3f(-1.f, -2.f, -3.f));
	F2(Math::vmin, Vector3f(-1.f, 2.f, -3.f), Vector3f(1.f, -2.f, 3.f), Vector3f(-1.f, -2.f, -3.f));
	F2(Math::vmin, Vector3f(-1.f, -2.f, 3.f), Vector3f(1.f, 2.f, -3.f), Vector3f(-1.f, -2.f, -3.f));

	//zero function
	F1(Math::zero, Vector3f(), true);
	F1(Math::zero, Vector3f(1.f, 0.f, 0.f), false);
	F1(Math::zero, Vector3f(0.f, 1.f, 0.f), false);
	F1(Math::zero, Vector3f(0.f, 0.f, 1.f), false);


	//color blend function
	F2(Math::color_blend, Vector3f(1.f, .5f, .6f), Vector3f(0.5f, .4f, .5f), Vector3f(0.5f, 0.2f, 0.3f));

	//interpolate functions
	F3(Math::interpolate2, Vector4f(1.f, 1.f, 1.f, 1.f), Vector4f(2.f, 2.f, 2.f, 2.f), .6f, Vector4f(1.6f, 1.6f, 1.6f, 1.6f));
	F3(Math::interpolate2, Vector3f(1.f, 1.f, 1.f), Vector3f(2.f, 2.f, 2.f), .6f, Vector3f(1.6f, 1.6f, 1.6f));
	F3(Math::interpolate2, Vector2f(1.f, 1.f), Vector2f(2.f, 2.f), .6f, Vector2f(1.6f, 1.6f));

	FN(Math::interpolate3, Vector2f(1.4f, 1.2f), Vector2f(1, 1), Vector2f(2, 1), Vector2f(1, 2), Vector2f(0.4, 0.2f));
	FN(Math::interpolate3, Vector3f(1.4f, 1.2f, 3.4f), Vector3f(1, 1, 3), Vector3f(2, 1, 4), Vector3f(1, 2, 3), Vector2f(0.4, 0.2));
	FN(Math::interpolate3, Vector4f(1.4f, 1.2f, 3.4f, 5.2f), Vector4f(1, 1, 3, 5), Vector4f(2, 1, 4, 5), Vector4f(1, 2, 3, 6), Vector2f(0.4, 0.2f));


	al_for(x,-1000,1000) {
		al_for(y,-1000,1000) {
			Vector3f v = Math::normalize(Vector3f(x, y, 1000 - x - y));
			F2(Math::dot, v, v, 1);
		}
	}

}


bool operator==(const Mat4x4& a,const Mat4x4& b) {
	for (int i = 0; i < 16;i++) {
		if (!al_fequal(a.raw[i], b.raw[i])) {
			return false;
		}
	}
	return true;
}

//print matrix status
stringstream& operator<<(stringstream& ss, const Mat4x4& m) {
	ss << "\n|" << to_string(m.a00) << "," << to_string(m.a01) << "," << to_string(m.a02) << "," << to_string(m.a03) << "|";
	ss << "\n|" << to_string(m.a10) << "," << to_string(m.a11) << "," << to_string(m.a12) << "," << to_string(m.a13) << "|";
	ss << "\n|" << to_string(m.a20) << "," << to_string(m.a21) << "," << to_string(m.a22) << "," << to_string(m.a23) << "|";
	ss << "\n|" << to_string(m.a30) << "," << to_string(m.a31) << "," << to_string(m.a32) << "," << to_string(m.a33) << "|";
	return ss;
}

TEST(MatrixTest,MatrixTest) {

	//multiply operations
	Mat4x4 A(1.f, 2.f, 3.f, 4.f,
		     1.f, 3.f, 5.f, 7.f,
		     1.f, 4.f, 9.f, 16.f,
		     1.f, 3.f, 9.f, 11.f);
	Mat4x4 B(3.f, 8.f, 9.f, 1.f,
		     4.f, 5.f, 7.f, 11.f,
		     0.f, 9.f, 3.f, 1.f,
		     7.f, 11.f, 1.f, 3.f);
	Mat4x4 C(0.f, 0.f, 0.f, 1.f,
		     0.f, 0.f, 1.f, 0.f,
		     0.f, 1.f, 0.f, 0.f,
		     1.f, 0.f, 0.f, 0.f);
	Vector4f d(1, 2, 3, 4);
	Vector4f Ad(30, 50, 100, 78);
	Vector4f dA(10,	32,	76,	110);

	Mat4x4 AB(39, 89, 36, 38,
		64, 145, 52, 60,
		131, 285, 80, 102,
		92, 225, 68, 76);
	Mat4x4 BA(21,69,139,223,
		27,84,199,284,
		13,42,81,122,
		22,60,112,154);
	
	OP2(A, *, B, AB);
	OP2(B, *, A, BA);
	OP2(A, *, d, Ad);
	OP2(d, *, A, dA);



	//inverse operation
	Mat4x4 InvA(3.2500f, -2.6250f, 0.2500f, 0.1250f,
		-1.2500f, 1.8750f, -0.2500f, -0.3750f,
		-0.2500f, 0.1250f, -0.2500f, 0.3750,
		0.2500, -0.3750, 0.2500, -0.1250);
	Mat4x4 InvB(0.0712, -0.0220, -0.2037, 0.1249,
		-0.0318, -0.0140, 0.1207, 0.0216,
		0.1258, 0.0087, -0.0447, -0.0589,
		-0.0915, 0.0997, 0.0476, -0.0178);
	//C-1 = C
	Mat4x4 InvC(C);
	F1(Math::inverse, A, InvA);
	F1(Math::inverse, B, InvB);
	F1(Math::inverse, C, InvC);



	//transpose operation
	Mat4x4 AT(
		1, 1, 1, 1,
		2, 3, 4, 3,
		3, 5, 9, 9,
		4, 7, 16, 11
	);
	Mat4x4 BT(
		3,4,0,7,
		8,5,9,11,
		9,7,3,1,
		1,11,1,3
	);
	//CT=C
	Mat4x4 CT(C);
	F1(Math::transpose, A, AT);
	F1(Math::transpose, B, BT);
	F1(Math::transpose, C, CT);
}

bool operator==(const Quaternion& q1,const Quaternion& q2) {
	return al_fequal(q1.val.x, q2.val.x) && al_fequal(q1.val.y, q2.val.y) &&
		al_fequal(q1.val.z, q2.val.z) && al_fequal(q1.val.w, q2.val.w);
}

bool operator==(const Quaternion& q,const Vector4f& v) {
	return q.val == v;
}


TEST(QuaternionAndTransform, QuaternionAndTransform) {
	Transform t1(Vector3f(1.f, 2.f, 3.f), Quaternion(Vector3f(0.f, 1.f, 0.f), Math::pi), Vector3f::I);
	Transform t2(Vector3f(1.f, 1.f, 4.f), Quaternion(Vector3f(0.f, 1.f, 0.f), Math::pi / 2.f), Vector3f::I);
	Mat4x4 t1Mat(
		-1, 0, 0, 1,
		0, 1, 0, 2,
		0, 0, -1, 3,
		0, 0, 0, 1),
		t2Mat(
			0, 0, 1, 1,
			0, 1, 0, 1,
			-1, 0, 0, 4,
			0, 0, 0, 1
		);

	EXPECT_TRUE(t1.GetMatrix() == t1Mat) << "expected transform matrix " << t1Mat << " get:" << t1.GetMatrix();
	EXPECT_TRUE(t2.GetMatrix() == t2Mat) << "expected transform matrix " << t2Mat << " get:" << t1.GetMatrix();
	Vector3f p(4.f, 5.f, 6.f);
	Vector3f vp1(-4.f, 5.f, -6.f), vp2(6.f, 5.f, -4.f);
	Vector3f pp1(-3, 7, -3), pp2(7, 6, 0);

	F2(Math::transform_point, t1.GetMatrix(), p, pp1);
	F2(Math::transform_vector, t1.GetMatrix(), p, vp1);
	F2(Math::transform_point, t2.GetMatrix(), p, pp2);
	F2(Math::transform_vector, t2.GetMatrix(), p, vp2);
}

TEST(BoundIntersectionTest, IntersectionTest) {
	float max_x = 1.f, min_x = -1.f, max_y = 1.f, min_y = -1.f, max_z = 3.f, min_z = 2.f;
	float eta = 0.f;
	float ox = 0.f, oy = 0.f;

	Bound3f bound(Vector3f(min_x, min_y, min_z), Vector3f(max_x, max_y, max_z));
	uint32 cx = 1000, cy = 1000;
	Vector3f o;
	//intersection success test
	al_for(x, 0, cx) {
		al_for(y,0,cy) {
			float px = (max_x - min_x - eta) * ((float)x / (float)cx - .5f) + ox;
			float py = (max_y - min_y - eta) * ((float)y / (float)cy - .5f) + oy;
			Vector3f p(px, py, min_z);
			Ray r(o, Math::normalize(p - o));
			ASSERT_TRUE(Math::ray_intersect(bound, r)) << "ray must hit the bound fail at :" << x << "," << y;
		}
	}


	max_x = 1.f + eta, min_x = -1.f - eta, max_y = 1.f + eta, min_y = -1.f - eta;
	//intersection fail test
	al_for(x,0,cx) {
		al_for(y,0,cy) {
			float py = (max_y - min_y) * ((float)y / (float)cy - .5f) * 2.f + oy,px;
			if (py > max_y || py < min_y) {
				px = (max_x - min_x) * ((float)x / (float)cx - .5f) * 2.f + ox;
			}
			else if(x < cx / 2) {
				px = (max_x - min_x) * ((float)x / (float)cx - .5f) + min_x - eta;
			}
			else {
				px = (max_x - min_x) * ((float)x / (float)cx + .5f) + max_x + eta;
			}
			Vector3f p(px, py, min_z);
			Ray r(o, Math::normalize(p - o));
			EXPECT_FALSE(Math::ray_intersect(bound, r)) << "ray must not hit the bound fail at :" << x << "," << y;
		}
	}
}

#include "scene/scene_primitive.h"

bool SphereIntersect(const ScenePrimitiveInfo& info, const Ray& r, Intersection& isect);

TEST(SphereIntersectionTest, IntersectionTest) {
	float max_x = 1.f, min_x = -1.f, max_y = 1.f, min_y = -1.f, z = 2.f;

	ScenePrimitiveInfo info;
	info.type = SCENE_PRIMITIVE_TYPE_SPHERE;
	info.intersector = SphereIntersect;
	info.material = make_shared<Material>(make_shared<LambertBSDF>(), nullptr, 0, nullptr, nullptr);
	info.data.sphere.radius = 1;
	info.data.sphere.trans = Transform(Vector3f(0, 0, z), Quaternion(), Vector3f::I);

	uint32 cx = 1000, cy = 1000;
	al_for(x, 0, cx) {
		al_for(y, 0, cy) {
			float px = (max_x - min_x) * ((float)x / (float)cx - .5f);
			float py = (max_y - min_y) * ((float)y / (float)cy - .5f);
			Vector3f p(px, py, z);
			Ray r(Vector3f(), Math::normalize(p));
			Intersection isect;
			float cost = Math::dot(r.d, Vector3f(0, 0, z));
			if (cost * cost < 3.f) {
				EXPECT_FALSE(SphereIntersect(info,r, isect)) << x << "," << y;
			}
			else {
				EXPECT_TRUE(SphereIntersect(info, r, isect)) << x << "," << y;
			}
		}
	}
	
	info.data.sphere.trans = Transform(Vector3f(0, sinf(Math::pi / 3.f) * z, cosf(Math::pi / 3.f) * z), Quaternion(), Vector3f::I);
	al_for(x, 0, cx) {
		al_for(y, 0, cy) {
			float px = (max_x - min_x) * ((float)x / (float)cx - .5f);
			float py = (max_y - min_y) * ((float)y / (float)cy - .5f);
			Vector3f p(px, py, z);
			Ray r(Vector3f(), Math::normalize(p));
			Intersection isect;
			float cost = Math::dot(r.d, Vector3f(0, sinf(Math::pi / 3.f) * z, cosf(Math::pi / 3.f) * z));
			if (cost * cost < 3.f) {
				EXPECT_FALSE(SphereIntersect(info, r, isect)) << x << "," << y;
			}
			else {
				EXPECT_TRUE(SphereIntersect(info, r, isect)) << x << "," << y;
			}
		}
	}
}


TEST(TriangleIntersectionTest, IntersectionTest) {
	float max_x = 1.f, min_x = -1.f, max_y = 1.f, min_y = -1.f, z = 2.f;

	Vector3f v0, v1, v2;
	v0 = Vector3f(max_x, max_y, z);
	v1 = Vector3f(min_x, max_y, z);
	v2 = Vector3f(min_x, min_y, z);
	float t;
	Vector2f uv;
	Vector3f position;

	Intersection isect;
	EXPECT_FALSE(Math::ray_intersect(v0, v1, v2, Ray(Vector3f(), Vector3f::Right), &t, &uv, &position));
	EXPECT_FALSE(Math::ray_intersect(v0, v1, v2, Ray(Vector3f(), Vector3f::Up), &t, &uv, &position));
	uint32 cx = 1000, cy = 1000;
	al_for(x,0,cx) {
		al_for(y,0,cy) {
			float px = (max_x - min_x) * ((float)x / (float)cx - .5f) * 4.f;
			float py = (max_y - min_y) * ((float)y / (float)cy - .5f) * 4.f;
			Vector3f p(px, py, z);
			Vector2f expectUv = Vector2f((max_y - py) / (max_y - min_y),(px - min_x) / (max_x - min_x));
			Ray r(Vector3f(), Math::normalize(p));
			if (al_fequal(px, min_x) || al_fequal(py, max_y)) break;
			if (x > y || px < min_x || px > max_x || py < min_y || py > max_y) {
				EXPECT_FALSE(Math::ray_intersect(v1, v2, v0, r, &t, &uv, &position)) << "triangle intersection fails at " << x << "," << y << "," << px << "," << py;
			}
			else if(x < y) {
				EXPECT_TRUE(Math::ray_intersect(v1, v2, v0, r, &t, &uv, &position)) << "triangle intersection fails at " << x << "," << y << "," << px << "," << py;
				EXPECT_TRUE(position == p) << "triangle intersection fails at " << x << "," << y << ","  << " intersection position should be " << p  << " rather than " << position;
				EXPECT_TRUE(uv == expectUv) << "triangle intersection fails at " << x << "," << y << "," << " uv at intersection point should be " << expectUv << " rather than " << uv;
			}
		}
	}



}

int main() {
	testing::InitGoogleTest();
	RUN_ALL_TESTS();
}