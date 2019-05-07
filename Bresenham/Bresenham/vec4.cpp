#include "vec4.h"

namespace egc {
	vec4& vec4::operator =(const vec4 &srcVector) {
		vec4 result;
		x = srcVector.x;
		y = srcVector.y;
		z = srcVector.z;
		w = srcVector.w;
		return *this;
	}
	vec4 vec4::operator +(const vec4& srcVector) const {
		vec4 result;
		result.x = x + srcVector.x;
		result.y = y + srcVector.y;
		result.z = z + srcVector.z;
		result.w = w + srcVector.w;
		return result;
	}
	vec4& vec4::operator +=(const vec4& srcVector) {
		vec4 result;
		x = x + srcVector.x;
		y = y + srcVector.y;
		z = z + srcVector.z;
		w = w + srcVector.w;
		return *this;
	}
	vec4 vec4::operator *(float scalarValue) const {
		vec4 result;
		result.x = x * scalarValue;
		result.y = y * scalarValue;
		result.z = z * scalarValue;
		result.w = w * scalarValue;
		return result;
	}
	vec4 vec4::operator -(const vec4& srcVector) const {
		vec4 result;
		result.x = x - srcVector.x;
		result.y = y - srcVector.y;
		result.z = z - srcVector.z;
		result.w = w - srcVector.w;
		return result;
	}
	vec4& vec4::operator -=(const vec4& srcVector) {
		vec4 result;
		x = x - srcVector.x;
		y = y - srcVector.y;
		z = z - srcVector.z;
		w = w - srcVector.w;
		return *this;
	}
	vec4 vec4::operator -() const {
		vec4 result;
		result.x = -x;
		result.y = -y;
		result.z = -z;
		result.w = -w;
		return result;
	}
	float vec4::length() const {
		return sqrt(x * x + y * y + z * z + w * w);
	}
	vec4& vec4::normalize() {
		*this = *this*(1.0f / length());
		return *this;
	}

}