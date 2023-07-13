#include "SM64DS_2.h"

void Vec3_Interp(Vector3& vF, const Vector3& v1, const Vector3& v2, Fix12i t)
{
	Vec3_Sub(vF, v2, v1);
	Vec3_MulScalarInPlace(vF, t);
	Vec3_Add(vF, vF, v1);
}

void Vec3_InterpCubic(Vector3& vF, const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, Fix12i t)
{
	vF.x = (v1.x * 3 - v0.x) + v2.x * -3 + v3.x;
	vF.y = (v1.y * 3 - v0.y) + v2.y * -3 + v3.y;
	vF.z = (v1.z * 3 - v0.z) + v2.z * -3 + v3.z;
	vF *= t;
	
	vF.x = vF.x + v0.x * 3 + v1.x * -6 + v2.x * 3;
	vF.y = vF.y + v0.y * 3 + v1.y * -6 + v2.y * 3;
	vF.z = vF.z + v0.z * 3 + v1.z * -6 + v2.z * 3;
	vF *= t;
	
	vF.x = vF.x + v0.x * -3 + v1.x * 3;
	vF.y = vF.y + v0.y * -3 + v1.y * 3;
	vF.z = vF.z + v0.z * -3 + v1.z * 3;
	vF *= t;
	vF += v0;
}