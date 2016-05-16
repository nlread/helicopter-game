//
// Created by Ned on 5/15/2016.
//

#ifndef INC_3DOPENGL_HELICAM_H
#define INC_3DOPENGL_HELICAM_H


#include "../../Camera.h"

class HeliCam {
public:
    virtual void setCamera(Camera& camera, int position) =0;
};


#endif //INC_3DOPENGL_HELICAM_H
