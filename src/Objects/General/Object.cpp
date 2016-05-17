//
// Created by Ned on 5/14/2016.
//

#include "Object.h"

void Object::drawShadow(float3 lightDir) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
        glTranslatef(0, .01, 0);
        glScalef(1, 0, 1);

        glTranslatef(position.x, position.y, position.z);
        glRotatef(orientationAngle, orientationAxis.x, orientationAxis.y, orientationAxis.z);
        glScalef(scaleFactor.x, scaleFactor.y, scaleFactor.z);

        drawModel();
    }
    glPopMatrix();
}

void Object::draw() {
    material->apply();
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    {
        applyTransformations();
        drawModel();
    }
    glPopMatrix();
}

void Object::applyTransformations() {
    glTranslatef(position.x, position.y, position.z);
    glRotatef(orientationAngle, orientationAxis.x, orientationAxis.y, orientationAxis.z);
    glRotatef((float)forwardTilt, 1, 0, 0);
    glScalef(scaleFactor.x, scaleFactor.y, scaleFactor.z);
}

Object *Object::rotate(float angle) {
    orientationAngle += angle;
    return this;
}

Object *Object::tilt(float angle) {
    forwardTilt += angle;
    return this;
}

Object *Object::scale(float3 factor) {
    scaleFactor *= factor;
    return this;
}

Object *Object::translate(float3 offset) {
    position += offset;
    return this;
}

void Object::drawDirection() {
    float3 endArrow = position + orientationAxis * 20;
    float3 topArrow = position + orientationAxis * 10;
    topArrow.z += 1;
    topArrow.x += 1;
    topArrow.y += 1;
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex3d(position.x, position.y, position.z);
        glVertex3d(topArrow.x, topArrow.y, topArrow.z);
        glVertex3d(endArrow.x, endArrow.y, endArrow.z);

    }
    glEnd();

    endArrow = position + orientationAxis.cross(position).normalize() * 20;
    topArrow = (position + orientationAxis.cross(position).normalize() * 10) + 1;
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex3d(position.x, position.y, position.z);
        glVertex3d(topArrow.x, topArrow.y, topArrow.z);
        glVertex3d(endArrow.x, endArrow.y, endArrow.z);

    }
    glEnd();
}