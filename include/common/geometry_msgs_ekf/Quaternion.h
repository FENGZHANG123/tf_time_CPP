#ifndef GEOMETRY_QUATERNION_H_
#define GEOMETRY_QUATERNION_H_

#include <iostream>


namespace geometry_msgs
{

struct Quaternion_z
{
    Quaternion_z() {}

    float x;
    float y;
    float z;
    float w;

    ~Quaternion_z(){}
};

}

#endif
