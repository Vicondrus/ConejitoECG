#include "mat4.h"
#include "mat3.h"

namespace egc {

	mat4& mat4::operator =(const mat4& srcMatrix) {
		for (int i = 0; i < 16; i++) {
			matrixData[i] = srcMatrix.matrixData[i];
		}
		return *this;
	}
	mat4 mat4::operator *(float scalarValue) const {
		mat4 result;
		for (int i = 0; i < 16; i++) {
			result.matrixData[i] = matrixData[i] * scalarValue;
		}
		return result;
	}
	mat4 mat4::operator *(const mat4& srcMatrix) const {
		mat4 result;
		for (int i = 0; i < 16; i++) {
			result.matrixData[i] = 0;
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					result.at(i, j) += this->at(i, k)*srcMatrix.at(k, j);
				}
			}
		}
		return result;
	}
	vec4 mat4::operator *(const vec4& srcVector) const {
		vec4 result;
		result.x = srcVector.x*matrixData[0] + srcVector.y*matrixData[4] + srcVector.z*matrixData[8] + srcVector.w*matrixData[12];
		result.y = srcVector.x*matrixData[1] + srcVector.y*matrixData[5] + srcVector.z*matrixData[9] + srcVector.w*matrixData[13];
		result.z = srcVector.x*matrixData[2] + srcVector.y*matrixData[6] + srcVector.z*matrixData[10] + srcVector.w*matrixData[14];
		result.w = srcVector.x*matrixData[3] + srcVector.y*matrixData[7] + srcVector.z*matrixData[11] + srcVector.w*matrixData[15];
		return result;
	}
	mat4 mat4::operator +(const mat4& srcMatrix) const {
		mat4 result;
		for (int i = 0; i < 16; i++) {
			result.matrixData[i] = matrixData[i] + srcMatrix.matrixData[i];
		}
		return result;
	}
	//get element by (row, column)
	float& mat4::at(int i, int j) {
		return matrixData[j * 4 + i];
	}
	const float& mat4::at(int i, int j) const {
		return matrixData[j * 4 + i];
	}

	mat3 cofactor(const mat4& matrix, int row, int col)
	{
		mat3 minor;
		int minor_row, minor_col;
		for (int i = 0; i < 4; i++) {
			minor_row = i;
			if (i > row)
				minor_row--;
			for (int j = 0; j < 4; j++) {
				minor_col = j;
				if (j > col)
					minor_col--;
				if (i != row && j != col)
					minor.at(minor_row,minor_col) = matrix.at(i,j);
			}
		}
		return minor;
	}

	float mat4::determinant() const {
		mat3 minor1, minor2, minor3, minor4;
		minor1 = cofactor(*this, 0, 0);
		minor2 = cofactor(*this, 0, 1);
		minor3 = cofactor(*this, 0, 2);
		minor4 = cofactor(*this, 0, 3);
		return this->at(0, 0)*minor1.determinant() - this->at(0, 1)*minor2.determinant() + this->at(0, 2)*minor3.determinant() - this->at(0, 3)*minor4.determinant();
		//return this->at(0, 0)*(this->at(1, 1)*this->at(2, 2)*this->at(3, 3) + this->at(1, 2)*this->at(2, 3)*this->at(3, 1) + this->at(2, 1)*this->at(3, 2)*this->at(1, 3) - this->at(1, 3)*this->at(2, 2)*this->at(3, 1) - this->at(1, 1)*this->at(2, 3)*this->at(3, 2) - this->at(1, 2)*this->at(2, 1)*this->at(3, 3))
			//- this->at(1, 0)*(this->at(0, 1)*this->at(2, 2)*this->at(3, 3) + this->at(0, 2)*this->at(2, 3)*this->at(3, 1) + this->at(2, 1)*this->at(3, 2)*this->at(0, 3) - this->at(3, 1)*this->at(2, 2)*this->at(3, 0) - this->at(0, 1)*this->at(2, 3)*this->at(3, 2) - this->at(0, 2)*this->at(2, 1)*this->at(3, 3))
			//+ this->at(2, 0)*(this->at(0, 1)*this->at(1, 2)*this->at(3, 3) + this->at(0, 2)*this->at(1, 3)*this->at(3, 1) + this->at(1, 1)*this->at(3, 2)*this->at(0, 3) - this->at(0, 3)*this->at(1, 2)*this->at(3, 1) - this->at(0, 1)*this->at(1, 3)*this->at(3, 2) - this->at(1, 1)*this->at(0, 2)*this->at(3, 3))
			//- this->at(3, 0)*(this->at(0, 1)*this->at(1, 2)*this->at(2, 3) + this->at(1, 1)*this->at(2, 2)*this->at(0, 3) + this->at(2, 1)*this->at(0, 2)*this->at(1, 3) - this->at(0, 3)*this->at(2, 1)*this->at(1, 2) - this->at(1, 3)*this->at(2, 2)*this->at(0, 1) - this->at(1, 1)*this->at(0, 2)*this->at(2, 3));
	}
	mat4 mat4::inverse() const {
		float det = this->determinant();
		mat4 aux;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				int sign=1;
				if ((i + j) % 2 == 1)
					sign = -1;
				aux.at(i, j) = sign * cofactor(*this, j, i).determinant();
				aux.at(i, j) = aux.at(i, j) / det;
			}
		}
		return aux;
	}
	mat4 mat4::transpose() const {
		mat4 result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.at(i, j) = this->at(j, i);
			}
		}
		return result;
	}
}