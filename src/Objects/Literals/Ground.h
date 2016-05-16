//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_GROUND_H
#define INC_3DOPENGL_GROUND_H


#include <GL/gl.h>
#include "../../Utility/float3.h"
#include "../General/Object.h"

class Ground : public Object {

public:
    Ground(Material *material) : Object(material) { }

    void drawModel();

    void drawShadow(float3 lightDir);
};


#endif //INC_3DOPENGL_GROUND_H
