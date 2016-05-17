//
// Created by Ned on 5/17/2016.
//

#ifndef INC_3DOPENGL_SEEKER_H
#define INC_3DOPENGL_SEEKER_H


#include "../General/Object.h"

class Seeker {
protected:
    Object* target = nullptr;

public:
    void setTarget(Object* target) {
        this->target = target;
    }

    float3 getDirToTarget(float3 position) {
        if(target == nullptr)
            return float3();
        return (target->position - position).normalize();
    }

};


#endif //INC_3DOPENGL_SEEKER_H
