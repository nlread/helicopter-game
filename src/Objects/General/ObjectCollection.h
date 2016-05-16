//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_OBJECTCOLLECTION_H
#define INC_3DOPENGL_OBJECTCOLLECTION_H


#include <GL/gl.h>
#include "../../Materials/Material.h"
#include "Object.h"

class ObjectCollection : public Object {
    std::vector<Object *> objects;
public:
    ObjectCollection(Material *material) : Object(material) { }

    virtual void draw();

    virtual void drawModel();

    void addObject(Object *newObject);
};


#endif //INC_3DOPENGL_OBJECTCOLLECTION_H
