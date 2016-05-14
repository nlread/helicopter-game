//
// Created by Ned on 5/14/2016.
//

#include <GL/gl.h>
#include "Material.h"

void Material::apply() {
    float aglDiffuse[] = {kd.x, kd.y, kd.z, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, aglDiffuse);
    float aglSpecular[] = {kd.x, kd.y, kd.z, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, aglSpecular);
    if (shininess <= 128)
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    else
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
    glDisable(GL_TEXTURE_2D);
}

Material::Material() {
    kd = float3(0.5, 0.5, 0.5) + float3::random() * 0.5;
    ks = float3(1, 1, 1);
    shininess = 15;
}