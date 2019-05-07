//
//  projection.cpp
//  Lab8_TODO
//
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "projection.h"

namespace egc {
    mat4 defineViewTransformMatrix(int startX, int startY, int width, int height)
    {
        mat4 viewTransformMatrix;
		viewTransformMatrix.at(0, 0) = width / 2;
		viewTransformMatrix.at(1, 1) = -height / 2;
		viewTransformMatrix.at(0, 3) = width / 2 + startX;
		viewTransformMatrix.at(1, 3) = height / 2 + startY;
        return viewTransformMatrix;
    }
    
    mat4 defineCameraMatrix(Camera myCamera)
    {
        mat4 cameraMatrix;
		vec3 w = -(myCamera.cameraTarget-myCamera.cameraPosition).normalize();
		vec3 u = crossProduct(myCamera.cameraUp, w).normalize();
		vec3 v = crossProduct(w,u);
		
		mat4 aux1;
		aux1.at(0, 0) = u.x;
		aux1.at(0, 1) = u.y;
		aux1.at(0, 2) = u.z;
		aux1.at(1, 0) = v.x;
		aux1.at(1, 1) = v.y;
		aux1.at(1, 2) = v.z;
		aux1.at(2, 0) = w.x;
		aux1.at(2, 1) = w.y;
		aux1.at(2, 2) = w.z;

		cameraMatrix.at(0, 3) = -myCamera.cameraPosition.x;
		cameraMatrix.at(1, 3) = -myCamera.cameraPosition.y;
		cameraMatrix.at(2, 3) = -myCamera.cameraPosition.z;

        return aux1 * cameraMatrix;
    }
    
    mat4 definePerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar)
    {
        mat4 perspectiveMatrix;
		perspectiveMatrix.at(0, 0) = 1 / (aspect * tan(fov / 2));
		perspectiveMatrix.at(1, 1) = 1 / (tan(fov / 2));
		perspectiveMatrix.at(2, 2) = (zNear + zFar) / (zNear - zFar);
		perspectiveMatrix.at(2, 3) = (2 * zNear*zFar) / -(zNear - zFar);
		perspectiveMatrix.at(3, 3) = 0;
		perspectiveMatrix.at(3, 2) = 1;
        return perspectiveMatrix;
    }
    
    void perspectiveDivide(vec4 &inputVertex)
    {
		inputVertex.x = inputVertex.x / inputVertex.w;
		inputVertex.y = inputVertex.y / inputVertex.w;
		inputVertex.z = inputVertex.z / inputVertex.w;
		inputVertex.w = inputVertex.w / inputVertex.w;
    }
}