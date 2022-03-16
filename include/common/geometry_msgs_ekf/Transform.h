#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "Vector3.h"
#include "Quaternion.h"
#include <iostream>

namespace geometry_msgs
{
struct Transform_z
{
    Transform_z() {}

    Vector3_z translation;
    Quaternion_z rotation;

    ~Transform_z(){}
};

}

#endif
