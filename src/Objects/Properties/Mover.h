//
// Created by Ned on 5/15/2016.
//

#ifndef INC_3DOPENGL_MOVING_H
#define INC_3DOPENGL_MOVING_H

#define GRAVITY float3(0, -9.8, 0);

#include <vector>
#include "../../Utility/float3.h"
#include "../Object.h"

class Mover {
protected:
    float3 velocity = float3(0, 0, 0);
    float3 acceleration = float3(0, 0, 0);
    float3 drag = float3(.6, .4, .6);
    float3 gravity = GRAVITY;
    float angularVelocity = 0.0;
    float angularAcceleration = 0.0;
    float restitution = .5;
public:

    void applyAcceleration(double t, double dt);

    float3 calcDeltaPosition(float3 oldPosition, double t, double dt);

    float calcDeltaOrientation(float oldOrientation, double t, double dt);

    bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                 std::vector<Object *> &objects);


};


#endif //INC_3DOPENGL_MOVING_H
