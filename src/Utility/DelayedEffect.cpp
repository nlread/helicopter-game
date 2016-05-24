//
// Created by Ned on 5/24/2016.
//

#include "DelayedEffect.h"

bool DelayedEffect::shouldActivateEffect(bool willActivateNow) {
    if(timeSinceEffect > delay) {
        if(willActivateNow)
            timeSinceEffect= 0;
        return true;
    }
    return false;
}

void DelayedEffect::timePassed(double dt) {
    timeSinceEffect += dt;
}

bool DelayedEffect::shouldActivateEffect() {
    return shouldActivateEffect(true);
}