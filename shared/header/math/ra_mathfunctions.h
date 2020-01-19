
#pragma once
//EXTERNAL INCLUDES
#include <assert.h>
#include <cmath>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "math/ra_mat4x4.h"
#include "math/ra_vector3.h"

namespace Math
{
	//Clamp float value between 2 values
	inline float Clamp(float value, float min, float max)
	{
		if (value > max)
			value = max;
		if (value < min)
			value = min;

		return value;
	}

	//Compute dot product between two vec4s
	//inline float Dot(const Vec4& lhs, const Vec4& rhs)
	//{
	//	return
	//		(lhs.r * rhs.r) +
	//		(lhs.g * rhs.g) +
	//		(lhs.b * rhs.b) +
	//		(lhs.a * rhs.a);
	//}

	//Multiply vec3 with a matrix
	//inline Vec3 operator*(const Vec3& lhs, const Math::Mat4x4& mat)
	//{
	//	return Vec3
	//	{
	//		mat.m11 * lhs.x + mat.m12 * lhs.y + mat.m13 * lhs.z + mat.m14 * 1.0f,
	//		mat.m21 * lhs.x + mat.m22 * lhs.y + mat.m23 * lhs.z + mat.m24 * 1.0f,
	//		mat.m31 * lhs.x + mat.m32 * lhs.y + mat.m33 * lhs.z + mat.m34 * 1.0f,
	//	};
	//}
}