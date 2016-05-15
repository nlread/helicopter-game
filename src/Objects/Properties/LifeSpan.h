//
// Created by Ned on 5/15/2016.
//

#ifndef INC_3DOPENGL_LIFESPAN_H
#define INC_3DOPENGL_LIFESPAN_H


class LifeSpan {
protected:
    bool liveForever = false;
    double lifeSpan = 3;
    double fadeOut = 1;
public:
    void age(double dt) {
        lifeSpan -= dt;
        if(fadeOut > 0) {

        }
    }

    bool isAlive() {
        return liveForever || lifeSpan > 0;
    }

    virtual void setLifeSpan(double lifeSpan) {
        this->lifeSpan = lifeSpan;
    }

};


#endif //INC_3DOPENGL_LIFESPAN_H
