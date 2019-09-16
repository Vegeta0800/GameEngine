
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
	inline float Clamp(float value, float min, float max)
	{
		if (value > max)
			value = max;
		if (value < min)
			value = min;

		return value;
	}

	inline fColorRGBA operator*(const fColorRGBA& vec4, Math::Mat4x4 mat)
	{
		mat = Math::Transpose(mat);

		return fColorRGBA
		{
			mat.m11 * vec4.r + mat.m12 * vec4.g + mat.m13 * vec4.b + mat.m14 * vec4.a,
			mat.m21 * vec4.r + mat.m22 * vec4.g + mat.m23 * vec4.b + mat.m24 * vec4.a,
			mat.m31 * vec4.r + mat.m32 * vec4.g + mat.m33 * vec4.b + mat.m34 * vec4.a,
			mat.m41 * vec4.r + mat.m42 * vec4.g + mat.m43 * vec4.b + mat.m44 * vec4.a,
		};
	}

	inline float Dot(const fColorRGBA& lhs, const fColorRGBA& rhs)
	{
		return
			(lhs.r * rhs.r) +
			(lhs.g * rhs.g) +
			(lhs.b * rhs.b) +
			(lhs.a * rhs.a);
	}

	inline Vec3 operator*(const Vec3& lhs, const Math::Mat4x4& mat)
	{
		return Vec3
		{
			mat.m11 * lhs.x + mat.m12 * lhs.y + mat.m13 * lhs.z + mat.m14 * 1.0f,
			mat.m21 * lhs.x + mat.m22 * lhs.y + mat.m23 * lhs.z + mat.m24 * 1.0f,
			mat.m31 * lhs.x + mat.m32 * lhs.y + mat.m33 * lhs.z + mat.m34 * 1.0f,
		};
	}

	inline void GetMinMaxOfProjection(Math::Vec3 corners[], Math::Vec3 axis, float& min, float& max)
	{
		min = 0.018736723663f;
		max = 0.018736723663f;

		for (int i = 0; i < 4; i++)
		{
			Math::Vec3 point = corners[i];

			float value = ((point.x * axis.x) + (point.y * axis.y)) / ((axis.x * axis.x) + (axis.y * axis.y));

			point = Math::Vec3{ value * axis.x, value * axis.y, 0.0f };

			float projection = Math::Dot(point, axis);

			if (min == 0.018736723663f)
			{
				min = projection;
				max = projection;
			}
			else
			{
				if (projection < min)
					min = projection;

				if (projection > max)
					max = projection;
			}
		}
	}

	inline fColorRGBA* Normalize(fColorRGBA* pout, fColorRGBA* pp)
	{
		FLOAT norm;

		norm = sqrt(pp->r * pp->r + pp->g * pp->g + pp->b * pp->b);
		if (norm)
		{
			pout->r = pp->r / norm;
			pout->g = pp->g / norm;
			pout->b = pp->b / norm;
			pout->a = pp->a / norm;
		}
		else
		{
			pout->r = 0.0f;
			pout->g = 0.0f;
			pout->b = 0.0f;
			pout->a = 0.0f;
		}
		return pout;
	}
	inline Math::Vec3 GetMirroredVector(Math::Vec3 dir, Math::Vec3 normal)
	{

	}
}