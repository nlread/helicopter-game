//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_CAMERA_H
#define INC_3DOPENGL_CAMERA_H


#include <vector>
#include <GL/glu.h>
#include "Utility/float3.h"
#include "Utility/float2.h"

class Camera {
    friend class Billboard;

    float3 eye;

    float3 lookAt;
    float3 right;
    float3 up;

    float fov;
    float aspect;

    float2 lastMousePos;
    float2 mouseDelta;

public:
    float3 getEye() {
        return eye;
    }

    Camera();

    void apply();

    void setAspectRatio(float ar) { aspect = ar; }

    void move(float dt, std::vector<bool> &keysPressed);

    void startDrag(int x, int y);

    void drag(int x, int y);

    void endDrag();

    float3 ahead;
};


#endif //INC_3DOPENGL_CAMERA_H
