#pragma once
#include <inttypes.h>

class VirtualRTC {
   public:
    virtual void setUnix(uint32_t unix) = 0;
    virtual uint32_t getUnix() = 0;
};