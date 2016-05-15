//
// Created by Ned on 5/14/2016.
//

#include <stdio.h>
#include "Ground.h"

void Ground::drawModel() {
    material->apply();
    glBegin(GL_QUADS);
    {
        glVertex3d(-100, 0, -100);
        glNormal3d(0, 1, 0);
        glVertex3d(-100, 0, 100);
        glNormal3d(0, 1, 0);
        glVertex3d(100, 0, 100);
        glNormal3d(0, 1, 0);
        glVertex3d(100, 0, -100);
        glNormal3d(0, 1, 0);
    }
    glEnd();
}

void Ground::drawShadow(float3 lightDir) {
}