//
// Created by Ned on 5/24/2016.
//

#ifndef INC_3DOPENGL_DELAYEDEFFECT_H
#define INC_3DOPENGL_DELAYEDEFFECT_H


class DelayedEffect {
protected:
    double timeSinceEffect;
    double delay;
public:

    DelayedEffect(double delay) {
        this->delay = delay;
        timeSinceEffect = 0;
    }

    bool shouldActivateEffect();

    bool shouldActivateEffect(bool willActivateNow);

    void timePassed(double dt);

    void setDelay(double delay) {
        this->delay = delay;
    }

};


#endif //INC_3DOPENGL_DELAYEDEFFECT_H
