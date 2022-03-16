#ifndef TRANSFORM_STAMPED_H_
#define TRANSFORM_STAMPED_H_

#include "Transform.h"
#include "Header.h"
#include <iostream>

namespace geometry_msgs
{

struct TransformStamped_z
{
    TransformStamped_z() {}

    Header header;
    std::string child_frame_id;
    Transform_z transform;

    ~TransformStamped_z(){}
};

}

#endif
