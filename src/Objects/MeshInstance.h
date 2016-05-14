//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_MESHINSTANCE_H
#define INC_3DOPENGL_MESHINSTANCE_H


#include "Object.h"
#include "Mesh.h"

class MeshInstance : public Object {
    Mesh *mesh;
public:
    MeshInstance(Material *material, Mesh *mesh) : Object(material) {
        this->mesh = mesh;
    }

    void drawModel();
};


#endif //INC_3DOPENGL_MESHINSTANCE_H
