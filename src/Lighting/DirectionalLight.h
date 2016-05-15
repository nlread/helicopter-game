//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_DIRECTIONALLIGHT_H
#define INC_3DOPENGL_DIRECTIONALLIGHT_H


#include <GL/gl.h>
#include "../Utility/float3.h"
#include "LightSource.h"

class DirectionalLight : public LightSource {
    float3 dir;
    float3 radiance;
public:
    DirectionalLight(float3 dir, float3 radiance);

    float3 getRadianceAt(float3 x);

    float3 getLightDirAt(float3 x);

    float getDistanceFrom(float3 x);

    void apply(GLenum openglLightName);
};



#endif //INC_3DOPENGL_DIRECTIONALLIGHT_H
