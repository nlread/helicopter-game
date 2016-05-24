//
// Created by Ned on 5/15/2016.
//

#ifndef INC_3DOPENGL_BULLET_H
#define INC_3DOPENGL_BULLET_H


#include <stdio.h>
#include "../General/MeshInstance.h"
#include "../Properties/Mover.h"
#include "../../Materials/TexturedMaterial.h"
#include "../Properties/Collidable.h"
#include "UFO.h"

class Bullet : public MeshInstance, public Mover, public Collidable {
protected:
    TexturedMaterial *particleMaterial = nullptr;
    double particleOffset = .5;
    double timeSinceParticle = 0;
    double lifeSpan = 5;
public:
    Bullet(Material *material, Mesh *mesh) : MeshInstance(material, mesh) {
        this->radius = 5;
        gravity = float3(0, 0, 0);
        drag = float3(1, 1, 1);
    }

    virtual void move(double t, double dt) {
        applyAcceleration(t, dt);
        float3 dPos = calcDeltaPosition(position, t, dt);
        translate(calcDeltaPosition(position, t, dt));
        timeSinceParticle += dt;
        lifeSpan -= dt;
    }


    virtual bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                         std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard) {
        if (particleMaterial != nullptr) {
            if (timeSinceParticle > particleOffset) {
                float3 particlePos = position;
                particlePos.z -= 2.5;
                Billboard *particle = new Billboard(particlePos, particleMaterial);
                spawnBillboard.push_back(particle);
                timeSinceParticle = 0;
            }
        }

        for (int i = 0; i < objects.size(); i++) {
            Object *o = objects.at(i);
            if (UFO *u = dynamic_cast<UFO *>(o)) {
                if (this->intersectsWith(u)) {
                    u->wasShot();
                    return true;
                }

            }
        }

        return lifeSpan < 0;
    }

    void setParticleMaterial(TexturedMaterial *particleMaterial) {
        this->particleMaterial = particleMaterial;
    }

    void setScaleFactor(float3 scaleFactor) {
        this->scaleFactor = scaleFactor;
    }

    float3 getCenter() {
        return this->position;
    }


};


#endif //INC_3DOPENGL_BULLET_H
