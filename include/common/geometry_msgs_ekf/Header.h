#ifndef HEADER_H_
#define HEADER_H_

#include "time/time.h"
#include <iostream>

typedef unsigned int uint32;

namespace geometry_msgs
{

struct Header
{
    Header() {}

    uint32 seq;
    ros::Time stamp;
    std::string frame_id;

    ~Header(){}
};

}

#endif
