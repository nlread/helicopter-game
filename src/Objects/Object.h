//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_OBJECT_H
#define INC_3DOPENGL_OBJECT_H


#include <GL/gl.h>
#include "../Utility/float3.h"
#include "Billboard.h"

class Object {
protected:
public:
    Material *material;
    float3 scaleFactor;
    float3 position;
    float3 orientationAxis;
    float orientationAngle;

    Object(Material *material) : material(material), orientationAngle(0.0f), scaleFactor(1.0, 1.0, 1.0),
                                 orientationAxis(0.0, 1.0, 0.0) { }

    virtual ~Object() { }

    //region Transformations

    Object * translate(float3 offset);

    Object * scale(float3 factor);

    Object * rotate(float angle);

    //endregion

    virtual void applyTransformations();

    virtual void draw();

    virtual void drawDirection();

    virtual void drawModel() = 0;

    virtual void move(double t, double dt) { }

    virtual bool control(std::vector<bool> &keysPressed, std::vector<Object *> &spawn,
                         std::vector<Object *> &objects, std::vector<Billboard *> &spawnBillboard) { return false; }

    virtual void drawShadow(float3 lightDir);
};

#endif //INC_3DOPENGL_OBJECT_H
