
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
	//Declare new Vector3.
	struct Vec3
	{
		float x;
		float y;
		float z;

		//Declare zero, up, down, right, left, front, behind and unit scale vector2. See vector3.cpp for definiton.
		static const Vec3 zero;
		static const Vec3 unit_x;	
		static const Vec3 unit_y;	
		static const Vec3 unit_z;	
		static const Vec3 neg_unit_x;
		static const Vec3 neg_unit_y;
		static const Vec3 neg_unit_z;
		static const Vec3 unit_scale;
	};

	inline bool operator==(const Vec3& lhs, const Vec3& rhs)
	{
		if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
			return true;
		return false;
	}


	inline bool operator!=(const Vec3& lhs, const Vec3& rhs)
	{
		if (lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z)
			return true;
		return false;
	}

	//Add each vector3 value with its counterpart.
	inline Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3
		{
			(lhs.x + rhs.x),
			(lhs.y + rhs.y),
			(lhs.z + rhs.z)
		};
	}

	//Add each vector3 value with a scalar product.
	inline Vec3 operator+(const Vec3& lhs, float scalar)
	{
		return Vec3
		{
			(lhs.x + scalar),
			(lhs.y + scalar),
			(lhs.z + scalar)
		};
	}

	//Subtract each vector3 value by its counterpart.
	inline Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3
		{
			(lhs.x - rhs.x),
			(lhs.y - rhs.y),
			(lhs.z - rhs.z)
		};
	}

	//Subtract each vector3 value by a scalar product.
	inline Vec3 operator-(const Vec3& lhs, float scalar)
	{
		return Vec3
		{
			(lhs.x - scalar),
			(lhs.y - scalar),
			(lhs.z - scalar)
		};
	}

	//Multiply each vector3 value by a scalar product.
	inline Vec3 operator*(const Vec3& lhs, float scalar)
	{
		return Vec3
		{
			(lhs.x * scalar),
			(lhs.y * scalar),
			(lhs.z * scalar)
		};
	}

	inline Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3
		{
			(lhs.x * rhs.x),
			(lhs.y * rhs.y),
			(lhs.z * rhs.z)
		};
	}

	//Divide each vector3 value by a scalar product.
	inline Vec3 operator/(const Vec3& lhs, float scalar)
	{
		assert(scalar != 0.0f);

		return Vec3
		{
			(lhs.x / scalar),
			(lhs.y / scalar),
			(lhs.z / scalar)
		};
	}

	//Add each vector3 value with its counterpart.
	inline Vec3& operator+=(Vec3& lhs, const Vec3& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;

		return lhs;
	}

	//Add each vector3 value with a scalar product.
	inline Vec3& operator+=(Vec3& lhs, float scalar)
	{
		lhs.x += scalar;
		lhs.y += scalar;
		lhs.z += scalar;

		return lhs;
	}

	//Subtract each vector3 value with its counterpart.
	inline Vec3& operator-=(Vec3& lhs, const Vec3& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		lhs.z -= rhs.z;

		return lhs;
	}

	//Subtract each vector3 value with a scalar product.
	inline Vec3& operator-=(Vec3& lhs, float scalar)
	{
		lhs.x -= scalar;
		lhs.y -= scalar;
		lhs.z -= scalar;

		return lhs;
	}

	//Multiply each vector3 value by a scalar product.
	inline Vec3& operator*=(Vec3& lhs, float scalar)
	{
		lhs.x *= scalar;
		lhs.y *= scalar;
		lhs.z *= scalar;

		return lhs;
	}

	//Divide each vector3 value by a scalar product.
	inline Vec3& operator/=(Vec3& lhs, float scalar)
	{
		assert(scalar != 0.0f);

		lhs.x /= scalar;
		lhs.y /= scalar;
		lhs.z /= scalar;

		return lhs;
	}

	//Get square length with pythagoras without the square root.
	inline float SquareLength(const Vec3& vec)
	{
		return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
	}

	//Get square length with pythagoras.
	inline float Length(const Vec3& vec)
	{
		return sqrt(SquareLength(vec));
	}

	//Get distance between two vector3. 
	inline float Distance(const Vec3& lhs, const Vec3& rhs)
	{
		return sqrt(((rhs.x - lhs.x) * (rhs.x - lhs.x)) + ((rhs.y - lhs.y) * (rhs.y - lhs.y)) + ((rhs.z - lhs.z) * (rhs.z - lhs.z)));
	}

	//Normalize vector2.
	//Get inverse length and multiply by each vector2 value.
	inline float Normalize(Vec3& vec)
	{
		float length = Length(vec);

		if (length < FLT_MIN)
		{
			return length;
		}

		float invLength = 1.0f / length;

		vec.x *= invLength;
		vec.y *= invLength;
		vec.z *= invLength;

		return length;
	}

	//Calculate dot product of 2 vector2.
	//Multiply each vector2 value with its counterpart and add the products.
	inline float Dot(const Vec3& lhs, const Vec3& rhs)
	{
		return
			(lhs.x * rhs.x) +
			(lhs.y * rhs.y) +
			(lhs.z * rhs.z);
	}

	//Interpolate between two vector3.
	inline Vec3 Lerp(const Vec3& lhs, const Vec3& rhs, float t)
	{

		return ((lhs * (1.0f - t)) + (rhs * t));
	}

	//Interpolate between two vector3.
	inline Vec3 Cross(const Vec3& lhs, const Vec3& rhs)
	{
		Vec3 temp;
		temp.x = (lhs.y * rhs.z) - (rhs.y * lhs.z);
		temp.y = (lhs.z * rhs.x) - (rhs.z * lhs.x);
		temp.z = (lhs.x * rhs.y) - (rhs.x * lhs.y);

		return temp;
	}

	inline Vec3 GetForwardVector(const Vec3& rotation)
	{
		float roll = rotation.x * M_PI / 180.0f;
		float pitch = rotation.y * M_PI / 180.0f;
		float yaw = rotation.z * M_PI / 180.0f;


		return Vec3
		{
			-cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll),
			-sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll),
			cos(pitch) * sin(roll)
		};
	}

	inline Vec3 GetRightVector(const Vec3& rotation)
	{
		float roll = rotation.x * M_PI / 180.0f;
		float pitch = rotation.y * M_PI / 180.0f;
		float yaw = rotation.z * M_PI / 180.0f;

		return Vec3
		{
			cos(yaw) * cos(pitch),
			sin(yaw) * cos(pitch),
			sin(pitch)
		};
	}

	inline Vec3 Negate(const Vec3& vec)
	{
		return Vec3
		{
			-vec.x,
			-vec.y,
			-vec.z
		};
	}
	
	inline Vec3 Abs(const Vec3& vec)
	{
		return Vec3
		{
			abs(vec.x),
			abs(vec.y),
			abs(-vec.z)
		};
	}
}
