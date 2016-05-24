//
// Created by Ned on 5/17/2016.
//

#include "UFO.h"

void UFO::move(double t, double dt) {
    float3 dirToTarget = getDirToTarget(position);
    setVelocity(dirToTarget * speed);

    applyAcceleration(t, dt);
    float3 dPosition = calcDeltaPosition(position, t, dt);
    float dOrientationAngle = calcDeltaOrientation(orientationAngle, t, dt);

    translate(dPosition);
    rotate(dOrientationAngle);
}

bool  UFO::control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                          std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard) {

    return dead;
}

void UFO::setSpeed(double speed) {
    this->speed = (float) speed;

}
