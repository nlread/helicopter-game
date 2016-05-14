//
// Created by Ned on 5/14/2016.
//

#include "Camera.h"

void Camera::move(float dt, std::vector<bool> &keysPressed) {
    if (keysPressed.at('w'))
        eye += ahead * dt * 20;
    if (keysPressed.at('s'))
        eye -= ahead * dt * 20;
    if (keysPressed.at('a'))
        eye -= right * dt * 20;
    if (keysPressed.at('d'))
        eye += right * dt * 20;
    if (keysPressed.at('q'))
        eye -= float3(0, 1, 0) * dt * 20;
    if (keysPressed.at('e'))
        eye += float3(0, 1, 0) * dt * 20;

    float yaw = atan2f(ahead.x, ahead.z);
    float pitch = -atan2f(ahead.y, sqrtf(ahead.x * ahead.x + ahead.z * ahead.z));

    yaw -= mouseDelta.x * 0.02f;
    pitch += mouseDelta.y * 0.02f;
    if (pitch > 3.14 / 2) pitch = 3.14 / 2;
    if (pitch < -3.14 / 2) pitch = -3.14 / 2;

    mouseDelta = float2(0, 0);

    ahead = float3(sin(yaw) * cos(pitch), -sin(pitch), cos(yaw) * cos(pitch));
    right = ahead.cross(float3(0, 1, 0)).normalize();
    up = right.cross(ahead);

    lookAt = eye + ahead;
}

void Camera::startDrag(int x, int y) {
    lastMousePos = float2(x, y);
}

void Camera::drag(int x, int y) {
    float2 mousePos(x, y);
    mouseDelta = mousePos - lastMousePos;
    lastMousePos = mousePos;
}

void Camera::endDrag() {
    mouseDelta = float2(0, 0);
}

void Camera::apply() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov / 3.14 * 180, aspect, 0.1, 500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z, lookAt.x, lookAt.y, lookAt.z, 0.0, 1.0, 0.0);
}

Camera::Camera() {
    eye = float3(0, 0, -5);
    lookAt = float3(0, 0, 0);
    right = float3(1, 0, 0);
    up = float3(0, 1, 0);

    fov = 1.1;
    aspect = 1;
}