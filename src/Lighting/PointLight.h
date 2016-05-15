//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_POINTLIGHT_H
#define INC_3DOPENGL_POINTLIGHT_H


#include "LightSource.h"
#include "../Utility/float3.h"

class PointLight : public LightSource {
    float3 pos;
    float3 power;
public:
    PointLight(float3 pos, float3 power);

    float3 getRadianceAt(float3 x);

    float3 getLightDirAt(float3 x);

    float getDistanceFrom(float3 x);

    void apply(GLenum openglLightName);
};


#endif //INC_3DOPENGL_POINTLIGHT_H
