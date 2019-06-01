
#pragma once
//EXTERNAL INCLUDES
#include <assert.h>
#include <cmath>
#include <limits>
//INTERNAL INCLUDES
#include "ra_types.h"

//Namespace Math
namespace Math
{
	//Declare new Vector2.
	struct Vec2
	{
		float x;
		float y;

		//Declare zero, up, down, right, left, and unit scale vector2. See vector2.cpp for definiton.
		static const Vec2 zero;
		static const Vec2 unit_x;
		static const Vec2 unit_y;
		static const Vec2 neg_unit_x;
		static const Vec2 neg_unit_y;
		static const Vec2 unit_scale;
	};

	//Add each vector2 value with its counterpart.
	inline Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2
		{ 
			(lhs.x + rhs.x),
			(lhs.y + rhs.y) 
		};
	}

	//Add each vector2 value with a scalar product.
	inline Vec2 operator+(const Vec2& lhs, float scalar)
	{
		return Vec2
		{
			(lhs.x + scalar),
			(lhs.y + scalar)
		};
	}

	//Subtract each vector2 value by its counterpart.
	inline Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2
		{
			(lhs.x - rhs.x),
			(lhs.y - rhs.y)
		};
	}

	//Subtract each vector2 value by a scalar product.
	inline Vec2 operator-(const Vec2& lhs, float scalar)
	{
		return Vec2
		{
			(lhs.x - scalar),
			(lhs.y - scalar)
		};
	}

	//Multiply each vector2 value by a scalar product.
	inline Vec2 operator*(const Vec2& lhs, float scalar)
	{
		return Vec2
		{
			(lhs.x * scalar),
			(lhs.y * scalar)
		};
	}

	//Divide each vector2 value by a scalar product.
	inline Vec2 operator/(const Vec2& lhs, float scalar)
	{
		assert(scalar != 0.0f);

		return Vec2
		{
			(lhs.x / scalar),
			(lhs.y / scalar)
		};
	}

	//Add each vector2 value with its counterpart.
	inline Vec2& operator+=(Vec2& lhs, const Vec2& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;

		return lhs;
	}

	//Add each vector2 value by a scalar product.
	inline Vec2& operator+=(Vec2& lhs, float scalar)
	{
		lhs.x += scalar;
		lhs.y += scalar;

		return lhs;
	}

	//Subtract each vector2 value with its counterpart.
	inline Vec2& operator-=(Vec2& lhs, const Vec2& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;

		return lhs;
	}

	//Subtract each vector2 value by a scalar product.
	inline Vec2& operator-=(Vec2& lhs, float scalar)
	{
		lhs.x -= scalar;
		lhs.y -= scalar;

		return lhs;
	}

	//Multiply each vector2 value by a scalar product.
	inline Vec2& operator*=(Vec2& lhs, float scalar)
	{
		lhs.x *= scalar;
		lhs.y *= scalar;

		return lhs;
	}

	//Divide each vector2 value by a scalar product.
	inline Vec2& operator/=(Vec2& lhs, float scalar)
	{
		assert(scalar != 0.0f);

		lhs.x /= scalar;
		lhs.y /= scalar;

		return lhs;
	}

	//Get square length with pythagoras without the square root.
	inline float SquareLength(const Vec2& vec)
	{
		return (vec.x * vec.x) + (vec.y * vec.y);
	}

	//Get square length with pythagoras.
	inline float Length(const Vec2& vec)
	{
		return sqrt(SquareLength(vec));
	}

	//Get distance between two vector2. 
	inline float Distance(const Vec2& lhs, const Vec2& rhs)
	{
		return Length(rhs) - Length(lhs);
	}

	//Normalize vector2.
	//Get inverse length and multiply by each vector2 value.
	inline float Normalize(Vec2& vec)
	{
		float length = Length(vec);

		if (length < FLT_MIN) 
		{
			return length;
		}

		float invLength = 1.0f / length;

		vec.x *= invLength;
		vec.y *= invLength;

		return length;
	}

	//Calculate dot product of 2 vector2.
	//Multiply each vector2 value with its counterpart and add the products.
	inline float Dot(const Vec2& lhs, const Vec2& rhs)
	{
		return
			(rhs.x * lhs.x) +
			(rhs.y * lhs.y);
	}
}
