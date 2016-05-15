//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_BILLBOARD_H
#define INC_3DOPENGL_BILLBOARD_H


#include <GL/gl.h>
#include "../Utility/float3.h"
#include "../Camera.h"
#include "../Materials/Material.h"
#include "Properties/LifeSpan.h"

class Billboard : public LifeSpan{
protected:
    Material *material;
    double fadeOut = 1;
    double transparency = 1;
    float3 position;
    float3 scaleFactor = float3(1, 1, 1);
    float3 velocity = float3();
public:
    Billboard(float3 position, Material *material) {
        this->position = position;
        this->material = material;
    }

    Billboard * translate(float3 offset);

    Billboard * scale(float3 factor);

    virtual void draw(Camera &camera);

    virtual bool move(double t, double dt);

    //region Getters and Setters

    void setVelocity(float3 velocity) {
        this->velocity = velocity;
    }

    void setLifeSpan(double lifeSpan) {
        setLifeSpan(lifeSpan, 0);
    }

    void setLifeSpan(double lifeSpan, double transparency) {
        this->fadeOut = transparency;
        this->lifeSpan = lifeSpan;
    }

    float3 getPosition() {
        return this->position;
    }

    //endregion
};



#endif //INC_3DOPENGL_BILLBOARD_H
