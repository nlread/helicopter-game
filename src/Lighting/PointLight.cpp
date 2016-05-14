//
// Created by Ned on 5/14/2016.
//

#include "PointLight.h"

void PointLight::apply(GLenum openglLightName) {
    float aglPos[] = {pos.x, pos.y, pos.z, 1.0f};
    glLightfv(openglLightName, GL_POSITION, aglPos);
    float aglZero[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glLightfv(openglLightName, GL_AMBIENT, aglZero);
    float aglIntensity[] = {power.x, power.y, power.z, 1.0f};
    glLightfv(openglLightName, GL_DIFFUSE, aglIntensity);
    glLightfv(openglLightName, GL_SPECULAR, aglIntensity);
    glLightf(openglLightName, GL_CONSTANT_ATTENUATION, 0.0f);
    glLightf(openglLightName, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(openglLightName, GL_QUADRATIC_ATTENUATION, 0.25f / 3.14f);
}

float3 PointLight::getRadianceAt(float3 x) { return power * (1 / (x - pos).norm2() * 4 * 3.14); }

float3 PointLight::getLightDirAt(float3 x) { return (pos - x).normalize(); }

float PointLight::getDistanceFrom(float3 x) { return (pos - x).norm(); }

PointLight::PointLight(float3 pos, float3 power) : pos(pos), power(power) { }