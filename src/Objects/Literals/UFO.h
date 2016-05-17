//
// Created by Ned on 5/17/2016.
//

#ifndef INC_3DOPENGL_UFO_H
#define INC_3DOPENGL_UFO_H


#include "../Properties/Seeker.h"
#include "../Properties/Mover.h"
#include "../General/MeshInstance.h"

class UFO : public MeshInstance, Mover, public Seeker {

public:
    UFO(Material *material, Mesh *mesh) : MeshInstance(material, mesh) { }

    void draw() {

    }

    void move(double t, double dt) {
        float3 dirToTarget = getDirToTarget(position);
        setVelocity(dirToTarget * 4);
    }
};


#endif //INC_3DOPENGL_UFO_H
