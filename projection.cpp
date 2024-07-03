//
//  projection.cpp
//  Lab8_TODO
//
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "projection.h"


namespace egc {
    //define the viewport transformation matrix
    //see section 3 from the laboratory work
    mat4 defineViewTransformMatrix(int startX, int startY, int width, int height)
    {
     
        mat4 S = mat4(); 
        mat4 T_center = mat4(); 
        mat4 T_start = mat4(); 

        S.at(0, 0) = width / 2.0f;
        S.at(1, 1) = -height / 2.0f; 
        S.at(2, 2) = 1.0f;
        S.at(3, 3) = 1.0f;

  
        T_center.at(0, 0) = 1.0f;
        T_center.at(1, 1) = 1.0f;
        T_center.at(2, 2) = 1.0f;
        T_center.at(3, 3) = 1.0f;
        T_center.at(0, 3) = width / 2.0f;
        T_center.at(1, 3) = height / 2.0f;

        
        T_start.at(0, 0) = 1.0f;
        T_start.at(1, 1) = 1.0f;
        T_start.at(2, 2) = 1.0f;
        T_start.at(3, 3) = 1.0f;
        T_start.at(0, 3) = startX;
        T_start.at(1, 3) = startY;

        
        mat4 viewTransformMatrix = T_start * T_center * S;

        return viewTransformMatrix;
    }
    
    //define the camera transformation matrix
    //see section 4 from the laboratory work
    mat4 defineCameraMatrix(Camera mc)
    {
        mat4 cameraMatrix;

        vec3 e = mc.cameraPosition;
        vec3 g= mc.cameraTarget - mc.cameraPosition;
        vec3 t = mc.cameraUp;

        vec3 w = -g.normalize();
        vec3 u = crossProduct(t, w).normalize();
        vec3 v = crossProduct(w, u);

        cameraMatrix.at(0, 0) = u.x;
        cameraMatrix.at(0, 1) = u.y;
        cameraMatrix.at(0, 2) = u.z;
        cameraMatrix.at(0, 3) = 0.0f;
        cameraMatrix.at(1, 0) = v.x;
        cameraMatrix.at(1, 1) = v.y;
        cameraMatrix.at(1, 2) = v.z;
        cameraMatrix.at(1, 3) = 0.0f;
        cameraMatrix.at(2, 0) = w.x;
        cameraMatrix.at(2, 1) = w.y;
        cameraMatrix.at(2, 2) = w.z;
        cameraMatrix.at(2, 3) = 0.0f;
        cameraMatrix.at(3, 0) = 0.0f;
        cameraMatrix.at(3, 1) = 0.0f;
        cameraMatrix.at(3, 2) = 0.0f;
        cameraMatrix.at(3, 3) = 1.0f;

        mat4 matT = translate(-mc.cameraPosition.x, -mc.cameraPosition.y, -mc.cameraPosition.z);

        cameraMatrix = cameraMatrix *matT;

        return cameraMatrix;
    }
    
    //define the projection transformation matrix
    //see section 5 from the laboratory work
    mat4 definePerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar)
    {
        mat4 perspectiveProjectionMatrix;

        float tanHalfFOV = tan(fov / 2.0f);
        float range = zNear - zFar;

        perspectiveProjectionMatrix.at(0, 0) = -(1.0f) / (aspect * tanHalfFOV);
        perspectiveProjectionMatrix.at(1, 1) = -(1.0f) / tanHalfFOV;
        perspectiveProjectionMatrix.at(2, 2) = (zNear + zFar) / range;
        perspectiveProjectionMatrix.at(2, 3) = (2.0f * zNear * zFar) / range;
        perspectiveProjectionMatrix.at(3, 2) = 1.0f;
        perspectiveProjectionMatrix.at(3, 3) = 0.0f;

        return perspectiveProjectionMatrix;
    }
    
    //define the perspective divide operation
    //see section 5 from the laboratory work
    void perspectiveDivide(vec4 &iv)
    {
		iv.x = iv.x / iv.w;
		iv.y = iv.y / iv.w;
		iv.z = iv.z / iv.w;
		iv.w = 1.0f;
		
    }

    //check if a point should be clipped
    //see section 9 from the laboratory work
    bool clipPointInHomogeneousCoordinate(const egc::vec4 &vertex)
    {
        float w = abs(vertex.w);
        if (-w <= vertex.x && vertex.x <= w &&
            -w <= vertex.y && vertex.y <= w &&
            -w <= vertex.z && vertex.z <= w) return false;
        return true;
    }

    //check if a triangle should be clipped
    //clip only those triangles for which all vertices are outside the viewing volume
    bool clipTriangleInHomegeneousCoordinates(const std::vector<egc::vec4> &triangle)
    {
        for (int i = 0; i < 3; i++) {
			if (!clipPointInHomogeneousCoordinate(triangle.at(i))) return true;
		}
        return false;

    }

    //compute the normal vector to a triangle
    //see section 7 from the laboratory work
    egc::vec3 findNormalVectorToTriangle(const std::vector<egc::vec4> &triangle)
    {
        egc::vec3 normal;
        
        egc::vec3 n;
        normal = crossProduct(triangle.at(1) - triangle.at(0), triangle.at(2) - triangle.at(0)).normalize();

   
        
        return normal;
    }

    //compute the coordinates of the triangle's center
    //(we will use this point to display the normal vector)
    egc::vec4 findCenterPointOfTriangle(const std::vector<egc::vec4> &triangle)
    {
        egc::vec4 triangleCenter,traingleCenterBeforeDivision;
         traingleCenterBeforeDivision= (triangle.at(0) + triangle.at(1) + triangle.at(2));
         triangleCenter = traingleCenterBeforeDivision * (1 / 3);
        return triangleCenter;
    }

    //check if the triangle is visible (front facing)
    //see section 8 from the laboratory work
    bool isTriangleVisible(const std::vector<egc::vec4> &triangle, const egc::vec3 &normalVector)
    {    
        vec3 triangleCenter=(triangle[0]+triangle[1]+triangle[2])*(1/3);
        vec3 viewDirection = vec3(0, 0, 0);
        vec3 triangleCenterToCamera = viewDirection - triangleCenter;
        if (dotProduct(normalVector, triangleCenterToCamera) > 0) {
			return false;
		}
        return true;
    }

    //display the normal vector of a triangle
    //see section 7 from the laboratory work
    //use the SDL_RenderDrawLine to draw the normal vector
    void displayNormalVectors(vec3& normalVector, vec4& triangleCenter, SDL_Renderer* renderer, mat4 viewTransformMatrix, mat4 perspectiveMatrix) {
        
        vec4 normalLine=vec4(normalVector.x, normalVector.y, normalVector.z, 1.0f);
        vec4 secondPoint = triangleCenter.operator+( normalLine * 0.05f);
        vec4 triangleCenterAfterProjection = perspectiveMatrix  * triangleCenter;
        triangleCenterAfterProjection = viewTransformMatrix * triangleCenterAfterProjection;
        vec4 secondPointAfterProjection = perspectiveMatrix * secondPoint;
        secondPointAfterProjection = viewTransformMatrix * secondPointAfterProjection;
        SDL_RenderDrawLine(renderer, triangleCenterAfterProjection.x, triangleCenterAfterProjection.y, secondPointAfterProjection.x, secondPointAfterProjection.y);

    }
}
