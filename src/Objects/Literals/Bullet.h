//
// Created by Ned on 5/15/2016.
//

#ifndef INC_3DOPENGL_BULLET_H
#define INC_3DOPENGL_BULLET_H


#include <stdio.h>
#include "../General/MeshInstance.h"
#include "../Properties/Mover.h"
#include "../../Materials/TexturedMaterial.h"

class Bullet : public MeshInstance, public Mover {
protected:
    TexturedMaterial* particleMaterial = nullptr;
    double particleOffset = .5;
    double timeSinceParticle = 0;
    double lifeSpan = 6;
public:
    Bullet(Material *material, Mesh *mesh) : MeshInstance(material, mesh) {
        gravity = float3(0, 0, 0);
        velocity.z = 10;
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
        if(particleMaterial != nullptr) {
            if(timeSinceParticle > particleOffset) {
                float3 particlePos = position;
                particlePos.z -= 2.5;
                Billboard* particle = new Billboard(particlePos, particleMaterial);
                spawnBillboard.push_back(particle);
                timeSinceParticle = 0;
            }
        }

        return lifeSpan < 0;
    }

    void setParticleMaterial(TexturedMaterial* particleMaterial) {
        this->particleMaterial = particleMaterial;
    }


};


#endif //INC_3DOPENGL_BULLET_H
