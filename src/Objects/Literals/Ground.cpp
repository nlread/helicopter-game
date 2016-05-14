//
// Created by Ned on 5/14/2016.
//

#include "Ground.h"

void Ground::drawModel() {
    material->apply();
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex3d(-100, 0, -100);
        glVertex3d(-100, 0, 100);
        glVertex3d(100, 0, 100);
        glVertex3d(100, 0, -100);
    }
    glEnd();
}

void Ground::drawShadow(float3 lightDir) {

}