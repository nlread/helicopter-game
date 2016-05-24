//
// Created by Ned on 5/17/2016.
//

#include "UFO.h"

bool UFO::exhaustCreated = false;
Material* UFO::exhaustMaterial = nullptr;

void UFO::move(double t, double dt) {
    timePassed(dt);
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
    if (exhaustDelay->shouldActivateEffect()) {
        Billboard *exhaust = createExhaust();
        spawnBillboard.push_back(exhaust);
    }
    return dead;
}

Billboard *UFO::createExhaust() {
    Billboard *exhaust = new Billboard(position, exhaustMaterial);
    exhaust->setLifeSpan(3, 1);
    exhaust->setVelocity(-velocity);
}

void UFO::setSpeed(double speed) {
    this->speed = (float) speed;

}
