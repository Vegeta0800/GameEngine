
#pragma once
//EXTERNAL INCLUDES
#include <assert.h>
#include <cmath>
//INTERNAL INCLUDES
#include "ra_types.h"

//Namespace Math
namespace Math
{
	//Declare new 3x3 Matrix
	struct Mat3x3
	{
		float m11, m12, m13;
		float m21, m22, m23;
		float m31, m32, m33;

		//Declare identity and zero matrix. See mat3x3.cpp for definiton.
		static const Mat3x3 identity;
		static const Mat3x3 zero;
	};

	//Add each matrix value with its counterpart matrix.
	inline Mat3x3 operator+(const Mat3x3& lhs, const Mat3x3& rhs)
	{
		return Mat3x3
		{
			lhs.m11 + rhs.m11,
			lhs.m12 + rhs.m12,
			lhs.m13 + rhs.m13,

			lhs.m21 + rhs.m21,
			lhs.m22 + rhs.m22,
			lhs.m23 + rhs.m23,

			lhs.m31 + rhs.m31,
			lhs.m32 + rhs.m32,
			lhs.m33 + rhs.m33
		};
	}

	//Subtract each matrix value with its counterpart matrix.
	inline Mat3x3 operator-(const Mat3x3& lhs, const Mat3x3& rhs)
	{
		return Mat3x3
		{
			lhs.m11 - rhs.m11,
			lhs.m12 - rhs.m12,
			lhs.m13 - rhs.m13,
					
			lhs.m21 - rhs.m21,
			lhs.m22 - rhs.m22,
			lhs.m23 - rhs.m23,
					
			lhs.m31 - rhs.m31,
			lhs.m32 - rhs.m32,
			lhs.m33 - rhs.m33
		};
	}

	//Multiply columns by rows
	inline Mat3x3 operator*(const Mat3x3& lhs, const Mat3x3& rhs)
	{
		return Mat3x3
		{
			(lhs.m11 * rhs.m11) + (lhs.m12 * rhs.m21) + (lhs.m13 * rhs.m31),
			(lhs.m11 * rhs.m12) + (lhs.m12 * rhs.m22) + (lhs.m13 * rhs.m32),
			(lhs.m11 * rhs.m13) + (lhs.m12 * rhs.m23) + (lhs.m13 * rhs.m33),

			(lhs.m21 * rhs.m11) + (lhs.m22 * rhs.m21) + (lhs.m23 * rhs.m31),
			(lhs.m21 * rhs.m12) + (lhs.m22 * rhs.m22) + (lhs.m23 * rhs.m32),
			(lhs.m21 * rhs.m13) + (lhs.m22 * rhs.m23) + (lhs.m23 * rhs.m33),

			(lhs.m31 * rhs.m11) + (lhs.m32 * rhs.m21) + (lhs.m33 * rhs.m31),
			(lhs.m31 * rhs.m12) + (lhs.m32 * rhs.m22) + (lhs.m33 * rhs.m32),
			(lhs.m31 * rhs.m13) + (lhs.m32 * rhs.m23) + (lhs.m33 * rhs.m33)
		};
	}

	//Multiply each matrix value by a scalar product
	inline Mat3x3 operator*(const Mat3x3& lhs, float scalar)
	{
		return Mat3x3
		{
			lhs.m11 * scalar,
			lhs.m12 * scalar,
			lhs.m13 * scalar,
					
			lhs.m21 * scalar,
			lhs.m22 * scalar,
			lhs.m23 * scalar,
					
			lhs.m31 * scalar,
			lhs.m32 * scalar,
			lhs.m33 * scalar
		};
	}

	//Transform input matrix into transpose matrix (a1, b1, c1, a2, b2, c2, a3, b3, c3)
	inline Mat3x3 Transpose(const Mat3x3& mat)
	{
		return Mat3x3
		{
			mat.m11,
			mat.m21,
			mat.m31,

			mat.m12,
			mat.m22,
			mat.m32,

			mat.m13,
			mat.m23,
			mat.m33
		};
	}
}
