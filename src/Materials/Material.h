//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_MATERIAL_H
#define INC_3DOPENGL_MATERIAL_H


#include "../float3.h"

class Material {
public:
    float3 kd;            // diffuse reflection coefficient
    float3 ks;            // specular reflection coefficient
    float shininess;    // specular exponent

    Material();

    virtual void apply();
};


#endif //INC_3DOPENGL_MATERIAL_H
