//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_OBJECTCOLLECTION_H
#define INC_3DOPENGL_OBJECTCOLLECTION_H


#include <GL/gl.h>
#include "../Materials/Material.h"
#include "Object.h"

class ObjectCollection : public Object {
    std::vector<Object *> objects;
public:
    ObjectCollection(Material *material) : Object(material) { }

    void draw() {
        material->apply();
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
        {
            applyTransformations();
            for (unsigned int i = 0; i < objects.size(); i++) {
                objects.at(i)->draw();
            }
        }
        glPopMatrix();
    }

    void drawModel() {

    }


    void addObject(Object *newObject) {
        objects.push_back(newObject);
    }
};


#endif //INC_3DOPENGL_OBJECTCOLLECTION_H
