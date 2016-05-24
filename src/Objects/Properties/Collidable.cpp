//
// Created by Ned on 5/23/2016.
//

#include "Collidable.h"

bool Collidable::intersectsWith(Collidable *other) {
    return (this->getCenter() - other->getCenter()).norm() < radius + other->getRadius();
}