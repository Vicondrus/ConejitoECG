#include "mat3.h"

namespace egc {

	mat3& mat3::operator =(const mat3& srcMatrix) {
		for (int i = 0; i < 9; i++) {
			matrixData[i] = srcMatrix.matrixData[i];
		}
		return *this;
	}
	mat3 mat3::operator *(float scalarValue) const {
		mat3 result;
		for (int i = 0; i < 9; i++) {
			result.matrixData[i] = matrixData[i] * scalarValue;
		}
		return result;
	}
	mat3 mat3::operator *(const mat3& srcMatrix) const {
		mat3 result;
		for (int i = 0; i < 9; i++) {
			result.matrixData[i] = 0;
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					result.at(i, j) += this->at(i, k)*srcMatrix.at(k, j);
				}
			}
		}
		return result;
	}
	vec3 mat3::operator *(const vec3& srcVector) const {
		vec3 result;
		result.x = srcVector.x*matrixData[0] + srcVector.y*matrixData[3] + srcVector.z*matrixData[6];
		result.y = srcVector.x*matrixData[1] + srcVector.y*matrixData[4] + srcVector.z*matrixData[7];
		result.z = srcVector.x*matrixData[2] + srcVector.y*matrixData[5] + srcVector.z*matrixData[8];
		return result;
	}
	mat3 mat3::operator +(const mat3& srcMatrix) const {
		mat3 result;
		for (int i = 0; i < 9; i++) {
			result.matrixData[i] = matrixData[i] + srcMatrix.matrixData[i];
		}
		return result;
	}
	//get element by (row, column)
	float& mat3::at(int i, int j) {
		return matrixData[j * 3 + i];
	}
	const float& mat3::at(int i, int j) const {
		return matrixData[j * 3 + i];
	}
	float mat3::determinant() const {
		return matrixData[0] * matrixData[4] * matrixData[8] + matrixData[3] * matrixData[7] * matrixData[2] + matrixData[1] * matrixData[5] * matrixData[6] - matrixData[2] * matrixData[4] * matrixData[6] - matrixData[0] * matrixData[5] * matrixData[7] - matrixData[1] * matrixData[3] * matrixData[8];
	}
	
	/*float mat3::determinant() const {
		float rez = 0;
		float d1, d2, d3, d4, d5, d6;
		d1 = this->at(0, 0) * this->at(1, 1) * this->at(2, 2);
		d2 = this->at(1, 0) * this->at(2, 1) * this->at(0, 2);
		d3 = this->at(2, 0) * this->at(0, 1) * this->at(1, 2);
		d4 = this->at(0, 2) * this->at(1, 1) * this->at(2, 0);
		d5 = this->at(1, 2) * this->at(2, 1) * this->at(0, 0);
		d6 = this->at(2, 2) * this->at(0, 1) * this->at(1, 0);
		rez = d1 + d2 + d3 - d4 - d5 - d6;
		return rez;
	}*/

	mat3 mat3::inverse() const {
		mat3 aux = this->transpose();
		float det = this->determinant();
		mat3 result;
		result.matrixData[0] = (aux.matrixData[4] * aux.matrixData[8] - aux.matrixData[7] * aux.matrixData[5]) / det;
		result.matrixData[1] = -(aux.matrixData[3] * aux.matrixData[8] - aux.matrixData[5] * aux.matrixData[6]) / det;
		result.matrixData[2] = (aux.matrixData[3] * aux.matrixData[7] - aux.matrixData[4] * aux.matrixData[6]) / det;
		result.matrixData[3] = -(aux.matrixData[1] * aux.matrixData[8] - aux.matrixData[7] * aux.matrixData[2]) / det;
		result.matrixData[4] = (aux.matrixData[0] * aux.matrixData[8] - aux.matrixData[2] * aux.matrixData[6]) / det;
		result.matrixData[5] = -(aux.matrixData[0] * aux.matrixData[7] - aux.matrixData[1] * aux.matrixData[6]) / det;
		result.matrixData[6] = (aux.matrixData[1] * aux.matrixData[5] - aux.matrixData[2] * aux.matrixData[4]) / det;
		result.matrixData[7] = -(aux.matrixData[0] * aux.matrixData[5] - aux.matrixData[2] * aux.matrixData[3]) / det;
		result.matrixData[8] = (aux.matrixData[0] * aux.matrixData[4] - aux.matrixData[1] * aux.matrixData[3]) / det;
		return result;
	}
	mat3 mat3::transpose() const {
		mat3 result;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				result.at(i, j) = this->at(j, i);
			}
		}
		return result;
	}
}