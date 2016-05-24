//
// Created by Ned on 5/17/2016.
//

#ifndef INC_3DOPENGL_UFO_H
#define INC_3DOPENGL_UFO_H


#include "../Properties/Seeker.h"
#include "../Properties/Mover.h"
#include "../General/MeshInstance.h"
#include "../Properties/Collidable.h"
#include "../../Materials/TexturedMaterial.h"
#include "../../Utility/DelayedEffect.h"
#include "../Properties/EffectManager.h"

class UFO : public MeshInstance, public Mover, public Seeker, public Collidable, public EffectManager {
protected:
    bool dead = false;
    float speed = 4;
    static Material* exhaustMaterial;// = nullptr;
    static bool exhaustCreated;
    DelayedEffect* exhaustDelay = new DelayedEffect(.75);
public:
    UFO(Material *material, Mesh *mesh) : MeshInstance(material, mesh) {
        this->radius = 15;
        if(!exhaustCreated) {
            exhaustMaterial = new TexturedMaterial("bullet2.png");
            exhaustCreated = true;
        }
        addDelayedEffect(exhaustDelay);
    }

    void move(double t, double dt);

    virtual bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                         std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard);

    Billboard* createExhaust();

    float3 getCenter() {
        return this->position;
    }

    void wasShot() {
            dead  = true;
    }

    void setSpeed(double speed);
};


#endif //INC_3DOPENGL_UFO_H
