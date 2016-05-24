//
// Created by Ned on 5/23/2016.
//

#include <math.h>
#include "Helicopter.h"

void Helicopter::move(double t, double dt) {
    if (engineOn) {
        mainRotor->orientationAngle += 20;
        tailRotor->orientationAngle += 20;

        if (!soundPlaying) {
            PlaySound(TEXT("sounds/helicopter.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            soundPlaying = true;
        }
    } else {
        if (soundPlaying) {
            PlaySound(NULL, 0, SND_ASYNC);
            soundPlaying = false;
        }
    }

    timeSinceBullet += dt;
    timeSinceDamageParticle += dt;

    applyAcceleration(t, dt);
    float3 dPosition = calcDeltaPosition(position, t, dt);
    float dOrientationAngle = calcDeltaOrientation(orientationAngle, t, dt);
    float dTilt = calcDeltaTilt(forwardTilt, t, dt);

    translate(dPosition);
    rotate(dOrientationAngle);
    if (!(forwardTilt > tiltLimit && dTilt > 0 || forwardTilt < -tiltLimit && dTilt < 0))
        tilt(dTilt);
}

bool Helicopter::control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                                 std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard) {
    if (keysPressed.at('p')) {
        if (mayToggleEngine) {
            engineOn = !engineOn;
            mayToggleEngine = false;
        }
    } else {
        mayToggleEngine = true;
    }

    if (engineOn) {
        if (keysPressed.at('y')) {
            acceleration.y = -gravity.y + 15;
        } else if (keysPressed.at('i')) {
            acceleration.y = -gravity.y - 15;
        } else {
            acceleration.y = -gravity.y;
        }
    } else {
        acceleration.y = 0;
    }

    handleRotation(keysPressed.at('h'), keysPressed.at('k'));

    handleXZMovement(keysPressed.at('u'), keysPressed.at('j'));

    handleShooting(keysPressed.at(' '), spawn);

    if(keysPressed.at('x'))
        createSmokeScreen(spawnBillboard);

    handleUFOHits(objects);

    handleDamageParticles(spawnBillboard);

    return false;
}

void Helicopter::handleXZMovement(bool moveForward, bool moveBackward) {
    if (moveForward) {
        acceleration.x = (float) (getXDir() * 20);
        acceleration.z = (float) (getZDir() * 20);
        tiltVelocity = 8;
    } else if (moveBackward) {
        acceleration.x = (float) (-getXDir() * 20);
        acceleration.z = (float) (-getZDir() * 20);
        tiltVelocity = -8;
    } else {
        acceleration.x = 0;
        acceleration.z = 0;
        if (std::abs(forwardTilt) < 1) {
            tiltVelocity = 0;
        } else if (forwardTilt > 0) {
            tiltVelocity = -6;
        } else {
            tiltVelocity = 6;
        }
    }
}

void Helicopter::handleRotation(bool rotateLeft, bool rotateRight) {
    if (rotateLeft) {
        angularAcceleration = 100;
    } else if (rotateRight) {
        angularAcceleration = -100;
    } else {
        angularAcceleration = 0;
    }
}

void Helicopter::handleShooting(bool attemptShoot, std::vector<Object *> &spawn) {
    if (attemptShoot) {
        if (timeSinceBullet > bulletDelay) {
            Bullet* b = createBullet();
            spawn.push_back(b);
            timeSinceBullet = 0;
        }
        // PlaySound(TEXT("sounds/machgun2.wav"), NULL, SND_ASYNC);
    }
}

void Helicopter::handleDamageParticles(std::vector<Billboard *> &spawnBillboard) {
    if(health < maxHealth) {
        if(health < 0) {
            PlaySound(TEXT("sounds/blast.wav"),NULL, SND_FILENAME | SND_SYNC);
            exit(0);
        }
        if(timeSinceDamageParticle > damageParticleDelay) {
            for(int i=0; i< pow(maxHealth - health, 2); i++) {
                int angle = rand() % 6500;
                Billboard *b = new Billboard(position + mainRotorOffset, smoke);
                b->scale(float3(6, 6, 6));
                b->setVelocity(float3(-cos(angle) * 7, 0, sin(angle) * 7));
                b->setLifeSpan(6, 2);
                spawnBillboard.push_back(b);
            }
            timeSinceDamageParticle = 0;
        }
    }
}

void Helicopter::handleUFOHits(std::vector<Object *> objects) {
    for (unsigned int i=0; i<objects.size(); i++) {
        Object* o=objects.at(i);
        if(UFO* u = dynamic_cast<UFO*>(o)) {
            if(this->intersectsWith(u)) {
                u->wasShot();
                this->wasShot();
            }
        }
    }
}
Bullet *Helicopter::createBullet() {
    Bullet *b = new Bullet(bulletMaterial, bulletMesh);
    b->position = getLocationInFront(15);
    b->position.y += 5;
    b->orientationAngle = this->orientationAngle;
    b->setVelocity(float3(getXDir(), getYDir(), getZDir()).normalize() * 50);
    b->setScaleFactor(float3(.5, .5, .5));
    b->forwardTilt = forwardTilt;
    return b;
}

float3 Helicopter::getLocationInFront(float distance) {
    return getDistanceInFront(distance) + position;

}

float3 Helicopter::getDistanceInFront(float distance) {
    return getDir() * distance;
}

void Helicopter::createSmokeScreen(std::vector<Billboard *> &spawnBillboard) {
    for (double i = 0; i < M_PI * 2; i += .4) {
        Billboard *b = new Billboard(position + mainRotorOffset, smoke);
        b->scale(float3(8, 8, 8));
        b->setVelocity(float3(-cos(i) * 7, 0, sin(i) * 7));
        b->setLifeSpan(6, 2);
        spawnBillboard.push_back(b);
    }
}

float Helicopter::getXDir() {
    return (float) sin(orientationAngle * M_PI / 180);
}

float Helicopter::getYDir() {
    return (float) (-forwardTilt / 180);
}

float Helicopter::getZDir() {
    return (float) cos(orientationAngle * M_PI / 180);
}

float3 Helicopter::getDir() {
    return float3(getXDir(), getYDir(), getZDir()).normalize();
}

void Helicopter::setCamera(Camera &camera, int camIndex) {
    camIndex = camIndex % 3;
    if (camIndex == 1) {
        float3 eye = getLocationInFront(25);
        eye.y += 10;
        camera.setEye(eye);

        float3 ahead = getDir();
        camera.setAhead(ahead);
    } else if (camIndex == 2) {
        float3 eye = position;
        eye.x -= getXDir() * 100;
        eye.y += 35;
        eye.z -= getZDir() * 100;
        camera.setEye(eye);

        float3 ahead = float3(getXDir(), 0, getZDir()).normalize();
        camera.setAhead(ahead);
    }
}

float3 Helicopter::getCenter() {
    return position + float3(0, 8, 0);
}



void Helicopter::wasShot() {
    health--;
}