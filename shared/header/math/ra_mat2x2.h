
#pragma once
//EXTERNAL INCLUDES
#include <assert.h>
#include <cmath>
//INTERNAL INCLUDES
#include "ra_types.h"

//Namepsace Math
namespace Math
{
	//Declare new 2 x 2 Matrix
	struct Mat2x2
	{
		float m11, m12;
		float m21, m22;

		//Declare indentity and zero matrix. See mat2x2.cpp for definiton
		static const Mat2x2 identity;
		static const Mat2x2 zero;
	};

	//Add each matrix value with its counterpart matrix.
	inline Mat2x2 operator+(const Mat2x2& lhs, const Mat2x2& rhs)
	{
		return Mat2x2
		{
			lhs.m11 + rhs.m11,
			lhs.m12 + rhs.m12,

			lhs.m21 + rhs.m21,
			lhs.m22 + rhs.m22
		};
	}

	//Subtract each matrix value with its counterpart matrix.
	inline Mat2x2 operator-(const Mat2x2& lhs, const Mat2x2& rhs)
	{
		return Mat2x2
		{
			lhs.m11 - rhs.m11,
			lhs.m12 - rhs.m12,

			lhs.m21 - rhs.m21,
			lhs.m22 - rhs.m22
		};
	}

	//Multiply columns times rows
	inline Mat2x2 operator*(const Mat2x2& lhs, const Mat2x2& rhs)
	{
		return Mat2x2
		{
			(lhs.m11 * rhs.m11) + (lhs.m12 * rhs.m21),
			(lhs.m11 * rhs.m12) + (lhs.m12 * rhs.m22),
													 
			(lhs.m21 * rhs.m11) + (lhs.m22 * rhs.m21),
			(lhs.m21 * rhs.m12) + (lhs.m22 * rhs.m22)
		};
	}

	//Multiply each matrix value by a scalar product
	inline Mat2x2 operator*(const Mat2x2& lhs, float scalar)
	{
		return Mat2x2
		{
			lhs.m11 * scalar,
			lhs.m12 * scalar,

			lhs.m21 * scalar,
			lhs.m22 * scalar
		};
	}

	//Transform input matrix into Transpose Matrix (a1, b1, a2, b2)
	inline Mat2x2 Transpose(const Mat2x2& mat)
	{
		return Mat2x2
		{
			mat.m11,
			mat.m21,

			mat.m12,
			mat.m22
		};
	}
}

