//
// Created by Ned on 5/14/2016.
//

#ifndef INC_3DOPENGL_TEXTUREDMATERIAL_H
#define INC_3DOPENGL_TEXTUREDMATERIAL_H


#include <GL/gl.h>
#include <stddef.h>
#include "Material.h"
#include "../Objects/Object.h"

extern "C" unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);

class TexturedMaterial : public Material {
    GLuint texture;
public:
    TexturedMaterial(const char *imageName, GLint filtering = GL_LINEAR_MIPMAP_LINEAR) {
        unsigned char *data;
        int width;
        int height;
        int nComponents = 4;
        data = stbi_load(imageName, &width, &height, &nComponents, 0);
        if (data == NULL) {
            return;
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        if (nComponents == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else if (nComponents == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        delete data;
    }

    void apply() {
        Material::apply();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
};


#endif //INC_3DOPENGL_TEXTUREDMATERIAL_H
