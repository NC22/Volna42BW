#include "stamp_zone.h"

static int16_t _dt_zone = 0;

void setStampZone(int16_t zone) {
    if (zone >= -12 && zone <= 14) zone *= 60;
    _dt_zone = zone;
}

int16_t getStampZone() {
    return _dt_zone;
}