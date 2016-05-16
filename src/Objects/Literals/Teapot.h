//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_TEAPOT_H
#define INC_3DOPENGL_TEAPOT_H


#include <GL/glut.h>
#include "../General/Object.h"

class Teapot : public Object {
public:
    Teapot(Material *material) : Object(material) { }

    void drawModel();
};


#endif //INC_3DOPENGL_TEAPOT_H
