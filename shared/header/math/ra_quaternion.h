
#pragma once
//EXTERNAL INCLUDES
#include <assert.h>
#include <cmath>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "ra_vector3.h"

//Namespace Math
namespace Math 
{
	//Declare new Quaternion (w,x,y,z)
	struct Quaternion
	{
		float w;
		float x;
		float y;
		float z;

		//Declare identity and zero quaternion. See quaternion.cpp for definition.
		static const Quaternion identity;
		static const Quaternion zero;
	};

	//Transform a vector3 into a quaternion
	inline Quaternion MakeQuaternion(const Vec3& vec)
	{
		return Quaternion
		{
			0.0f,
			vec.x,
			vec.y,
			vec.z
		};
	}

	//Transform a vector3 into a quaternion with an input angle.
	inline Quaternion MakeQuaternion(const Vec3& axis, float angle)
	{
		float halfangle = angle * 0.5f;

		return Quaternion
		{
			cos(halfangle),
			axis.x * sin(halfangle),
			axis.y * sin(halfangle),
			axis.z * sin(halfangle)
		};
	}

	//Add each quaternion value by its quaternion counterpart.
	inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion
		{
			(lhs.w + rhs.w),
			(lhs.x + rhs.x),
			(lhs.y + rhs.y),
			(lhs.z + rhs.z)
		};
	}

	//Subtract each quaternion value by its quaternion counterpart.
	inline Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion
		{
			(lhs.w - rhs.w),
			(lhs.x - rhs.x),
			(lhs.y - rhs.y),
			(lhs.z - rhs.z)
		};
	}

	//Quaternion multiplication.
	//w: (w, x, y, z). Operators: -,-,-
	//x: (x, w, z, y). Operators: +,-,+
	//y: (y, z, x, w). Operators: +,+,-
	//z: (z, y, w, x). Operators: -,+,+
	inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion
		{
			rhs.w * lhs.w - rhs.x * lhs.x - rhs.y * lhs.y - rhs.z * lhs.z,
			rhs.w * lhs.x + rhs.x * lhs.w - rhs.y * lhs.z + rhs.z * lhs.y,
			rhs.w * lhs.y + rhs.x * lhs.z + rhs.y * lhs.w - rhs.z * lhs.x,
			rhs.w * lhs.z - rhs.x * lhs.y + rhs.y * lhs.x + rhs.z * lhs.w
		};
	}

	//Multiply each quaternion value by a scalar product.
	inline Quaternion operator*(const Quaternion& lhs, float scalar)
	{
		return Quaternion
		{
			(lhs.w * scalar),
			(lhs.x * scalar),
			(lhs.y * scalar),
			(lhs.z * scalar)
		};
	}

	//Divide each quaternion value by a scalar product, if its not 0.
	inline Quaternion operator/(const Quaternion& lhs, float scalar)
	{
		assert(scalar != 0.0f);

		return Quaternion
		{
			(lhs.w / scalar),
			(lhs.x / scalar),
			(lhs.y / scalar),
			(lhs.z / scalar)
		};
	}

	//Canjugate input quaternion.
	//Vector3 part (x, y, z) turns negativ. 
	inline Quaternion Conjugate(const Quaternion& quat)
	{
		return Quaternion
		{
			 quat.w,
			-quat.x,
			-quat.y,
			-quat.z
		};
	}

	//Calculate quaternion length with pythagoras.
	inline float Length(const Quaternion& quat)
	{
		return sqrt
		(
			(quat.w * quat.w) +
			(quat.x * quat.x) +
			(quat.y * quat.y) +
			(quat.z * quat.z)
		);
	}

	//Calculate quaternion dot product:
	//Multiply each quaternion value with its quaternion counterpart
	//And add all products.
	inline float Dot(const Quaternion& lhs, const Quaternion& rhs)
	{
		return
			(lhs.w * rhs.w) +
			(lhs.x * rhs.x) +
			(lhs.y * rhs.y) +
			(lhs.z * rhs.z);
	}

	//Normalize a quaternion.
	//Get inverse length of quaternion and multiply each quaternion value by inverse length.
	inline float Normalize(Quaternion& quat)
	{
		float length = Length(quat);

		if (length < FLT_MIN)
		{
			return length;
		}

		float invLength = 1.0f / length;

		quat.w = quat.w * invLength;
		quat.x = quat.x * invLength;
		quat.y = quat.y * invLength;
		quat.z = quat.z * invLength;

		return length;
	}

	//Interpolate between two quaternions. 
	inline Quaternion Lerp(const Quaternion& lhs, const Quaternion& rhs, float t)
	{
		Quaternion res = ((lhs * (1.0f - t)) + (rhs * t));

		Normalize(res);

		return res;
	}

	//Spherical Interpolation between two quaternions.
	inline Quaternion Slerp(const Quaternion& lhs, const Quaternion& rhs, float t)
	{
		float dotProduct = Dot(lhs, rhs);

		if (dotProduct > 0.995f) 
		{
			return Lerp(lhs, rhs, t);
		}

		float theta0 = acos(dotProduct);
		float theta = theta0 * t;

		Quaternion temp = (rhs - lhs) * dotProduct;

		Normalize(temp);

		return lhs * cos(theta) + temp * sin(theta);
	}

	//Rotate a vector3 by a quaternion
	inline void Rotate(Vec3& lhs, const Quaternion& q)
	{
		Quaternion p = MakeQuaternion(lhs);

		p = q * p * Conjugate(q);
			
		lhs.x = p.x;
		lhs.y = p.y;
		lhs.z = p.z;
	}
}
