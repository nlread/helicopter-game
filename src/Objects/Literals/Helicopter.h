//
// Created by Ned on 5/23/2016.
//

#ifndef INC_3DOPENGL_HELICOPTER_H
#define INC_3DOPENGL_HELICOPTER_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// Needed on MsWindows
#include <windows.h>

#endif // Win32 platform

#include <math.h>
#include <mmsystem.h>
#include <winnt.h>
#include <cmath>
#include "../../Utility/float3.h"
#include "../../Camera.h"
#include "../Billboard.h"
#include "../General/Object.h"
#include "../General/ObjectCollection.h"
#include "../Properties/HeliCam.h"
#include "Bullet.h"

#define M_PI 3.1415926535

class Helicopter : public ObjectCollection, public Mover, public HeliCam, public Collidable {
protected:
    float3 mainRotorOffset = float3(0, 15, 4.5);

    MeshInstance *body;
    MeshInstance *mainRotor;
    MeshInstance *tailRotor;
    bool playingSound = false;

    bool engineOn = false;
    bool mayToggleEngine = true;
    bool soundPlaying = false;
    float tiltLimit = 13;
    double bulletDelay = .5;
    double timeSinceBullet = 0;
    Mesh *bulletMesh;
    Material *bulletMaterial;

    TexturedMaterial *smoke;
    int health = 3;
    int maxHealth = 3;
    double damageParticleDelay = .6;
    double timeSinceDamageParticle = 0;

    double bladeVelocity = 0;
    double bladeDrag = .7;


public:
    Helicopter(Material *material, Mesh *bodyMesh, Mesh *mainRotorMesh, Mesh *tailRotorMesh) : ObjectCollection(
            material) {


        body = new MeshInstance(material, bodyMesh);
        mainRotor = new MeshInstance(material, mainRotorMesh);
        tailRotor = new MeshInstance(material, tailRotorMesh);

        mainRotor->translate(float3(0, 15, 4.5f));
        tailRotor->translate(float3(.5, 15, -36));
        tailRotor->orientationAxis = (float3(1, 0, 0));
        addObject(body);
        addObject(mainRotor);
        addObject(tailRotor);

        bulletMesh = new Mesh("bullet.obj");
        bulletMaterial = new Material();
        bulletMaterial->kd = float3(.3, .3, .3);

        drag = float3(.5, .5, .5);
        angularDrag = .45;
        radius = 18;

        smoke = new TexturedMaterial("smoke1.png");


    }

    void move(double t, double dt);

    virtual bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                         std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard);

    void handleRotation(bool rotateLeft, bool rotateRight);

    void handleXZMovement(bool moveForward, bool moveBackward);

    Bullet *createBullet();

    void handleShooting(bool attemptShoot, std::vector<Object *> &spawn);

    void handleDamageParticles(std::vector<Billboard *> &spawnBillboard);

    float3 getLocationInFront(float distance);

    float3 getDistanceInFront(float distance);

    void createSmokeScreen(std::vector<Billboard *> &spawnBillboard);

    float getXDir();

    float getYDir();

    float getZDir();

    float3 getDir();

    void setCamera(Camera &camera, int camIndex);

    float3 getCenter();

    void wasShot();


    void handleUFOHits(std::vector<Object *> objects);
};


#endif //INC_3DOPENGL_HELICOPTER_H
