//
// Created by Ned on 5/14/2016.
//

#include "Billboard.h"

void Billboard::draw(Camera &camera) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDepthMask(GL_FALSE);

    material->apply();

    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);
    glScalef(scaleFactor.x, scaleFactor.y, scaleFactor.z);
    float camRotation[] = {
            camera.right.x, camera.right.y, camera.right.z, 0,
            camera.up.x, camera.up.y, camera.up.z, 0,
            camera.ahead.x, camera.ahead.y, camera.ahead.z, 0,
            0, 0, 0, 1
    };
    glMultMatrixf(camRotation);
    glScalef(4, 4, 4);
    glColor4d(1, 1, 1, transparency);
    glBegin(GL_QUADS);
    {
        glTexCoord3d(0, 0, 0);
        glVertex3d(-1, -1, 0);

        glTexCoord3d(1, 0, 0);
        glVertex3d(1, -1, 0);

        glTexCoord3d(1, 1, 0);
        glVertex3d(1, 1, 0);

        glTexCoord3d(0, 1, 0);
        glVertex3d(-1, 1, 0);
    }
    glEnd();
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_LIGHTING);
}

bool Billboard::move(double t, double dt) {
    translate(velocity * dt);
    lifeSpan -= dt;
    if(lifeSpan < fadeOut) {
        transparency = lifeSpan / fadeOut;
    }
    return lifeSpan < 0;
}

Billboard *Billboard::scale(float3 factor) {
    scaleFactor *= factor;
    return this;
}

Billboard *Billboard::translate(float3 offset) {
    position += offset;
    return this;
}