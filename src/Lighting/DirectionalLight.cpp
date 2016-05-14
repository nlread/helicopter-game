//
// Created by Ned on 5/14/2016.
//

#include "DirectionalLight.h"

void DirectionalLight::apply(GLenum openglLightName) {
    float aglPos[] = {dir.x, dir.y, dir.z, 0.0f};
    glLightfv(openglLightName, GL_POSITION, aglPos);
    float aglZero[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glLightfv(openglLightName, GL_AMBIENT, aglZero);
    float aglIntensity[] = {radiance.x, radiance.y, radiance.z, 1.0f};
    glLightfv(openglLightName, GL_DIFFUSE, aglIntensity);
    glLightfv(openglLightName, GL_SPECULAR, aglIntensity);
    glLightf(openglLightName, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(openglLightName, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(openglLightName, GL_QUADRATIC_ATTENUATION, 0.0f);
}

float DirectionalLight::getDistanceFrom(float3 x) { return 900000000; }

float3 DirectionalLight::getLightDirAt(float3 x) { return dir; }

float3 DirectionalLight::getRadianceAt(float3 x) { return radiance; }

DirectionalLight::DirectionalLight(float3 dir, float3 radiance) : dir(dir), radiance(radiance) { }