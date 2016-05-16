//
// Created by Ned on 5/14/2016.
//

#include "ObjectCollection.h"

void ObjectCollection::draw() {
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

void ObjectCollection::addObject(Object *newObject) {
    objects.push_back(newObject);
}

void ObjectCollection::drawModel() {

}