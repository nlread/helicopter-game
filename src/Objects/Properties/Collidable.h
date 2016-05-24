//
// Created by Ned on 5/23/2016.
//

#ifndef INC_3DOPENGL_COLLIDABLE_H
#define INC_3DOPENGL_COLLIDABLE_H


#include "../../Utility/float3.h"

class Collidable {
protected:
    float radius;
public:
    virtual bool intersectsWith(Collidable *other);

    float getRadius() {
        return radius;
    }

    virtual float3 getCenter()=0;

};


#endif //INC_3DOPENGL_COLLIDABLE_H
