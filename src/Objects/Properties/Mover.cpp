//
// Created by Ned on 5/15/2016.
//

#include "Mover.h"

void Mover::applyAcceleration(double t, double dt) {
    velocity += (acceleration + gravity) * dt;
    velocity *= drag.power(dt);
    angularVelocity += angularAcceleration * dt;
    angularVelocity *= pow(angularDrag, dt);
    tiltVelocity += tiltAcceleration * dt;
    tiltVelocity *= pow(.8, dt);
}

float3 Mover::calcDeltaPosition(float3 oldPosition, double t, double dt) {
    if (oldPosition.y < 0 && velocity.y < 0) {
        velocity.y *= -restitution;
        if (abs(velocity.y) < .1) {
            velocity.y = 0;
        }
    }
    return velocity * dt;
}

float Mover::calcDeltaOrientation(float oldOrientation, double t, double dt) {
    return angularVelocity * dt;
}

bool Mover::control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                    std::vector<Object *> &objects) {
}

float Mover::calcDeltaTilt(float oldTilt, double t, double dt) {
    return tiltVelocity * dt;
}
