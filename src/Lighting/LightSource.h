//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_LIGHTSOURCE_H
#define INC_3DOPENGL_LIGHTSOURCE_H


#include <GL/gl.h>
#include "../float3.h"

class LightSource {
public:
    virtual float3 getRadianceAt(float3 x) = 0;

    virtual float3 getLightDirAt(float3 x) = 0;

    virtual float getDistanceFrom(float3 x) = 0;

    virtual void apply(GLenum openglLightName) = 0;
};


#endif //INC_3DOPENGL_LIGHTSOURCE_H
