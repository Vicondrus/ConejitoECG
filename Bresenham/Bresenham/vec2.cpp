#include "vec2.h"

namespace egc {
	vec2 vec2::operator +(const vec2& srcVector) const
	{
		vec2 result;
		result.x = x + srcVector.x;
		result.y = y + srcVector.y;
		return result;
	}

	vec2& vec2::operator =(const vec2 & srcVector)
	{
		vec2 result;
		x = srcVector.x;
		y = srcVector.y;
		return *this;
	}

	vec2& vec2::operator +=(const vec2& srcVector) {
		vec2 result;
		x = x + srcVector.x;
		y = y + srcVector.y;
		return *this;
	}
	vec2 vec2::operator *(float scalarValue) const {
		vec2 result;
		result.x = x * scalarValue;
		result.y = y * scalarValue;
		return result;
	}
	vec2 vec2::operator -(const vec2& srcVector) const {
		vec2 result;
		result.x = x - srcVector.x;
		result.y = y - srcVector.y;
		return result;
	}
	vec2& vec2::operator -=(const vec2& srcVector) {
		vec2 result;
		x = x - srcVector.x;
		y = y - srcVector.y;
		return *this;
	}
	vec2 vec2::operator -() const {
		vec2 result;
		result.x = -x;
		result.y = -y;
		return result;
	}
	float vec2::length() const {
		return sqrt(x * x + y * y);
	}
	vec2& vec2::normalize() {
		*this = *this*(1.0f / length());
		return *this;
	}

	float dotProduct(const vec2& v1, const vec2& v2) {
			float result;
			result = v1.x*v2.x + v1.y*v2.y;
			return result;
	}
}