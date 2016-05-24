//
// Created by Ned on 5/24/2016.
//

#ifndef INC_3DOPENGL_EFFECTMANAGER_H
#define INC_3DOPENGL_EFFECTMANAGER_H


#include <bits/stl_bvector.h>
#include "../../Utility/DelayedEffect.h"

class EffectManager {
protected:
    std::vector<DelayedEffect *> delayedEffects;

public:
    void timePassed(double dt) {
        for(unsigned int i=0; i<delayedEffects.size(); i++) {
            delayedEffects.at(i)->timePassed(dt);
        }
    }
};


#endif //INC_3DOPENGL_EFFECTMANAGER_H
