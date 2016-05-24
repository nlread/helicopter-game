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

class UFO : public MeshInstance, public Mover, public Seeker, public Collidable {
protected:
    bool dead = false;
    float speed = 4;
    static Material* exhaust = nullptr;
public:
    UFO(Material *material, Mesh *mesh) : MeshInstance(material, mesh) {
        this->radius = 15;
        if(exhaust == nullptr) {
            exhaust = new TexturedMaterial("bullet2.png");
        }
    }

    void move(double t, double dt);

    virtual bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                         std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard);


    float3 getCenter() {
        return this->position;
    }

    void wasShot() {
            dead  = true;
    }

    void setSpeed(double speed);
};


#endif //INC_3DOPENGL_UFO_H
