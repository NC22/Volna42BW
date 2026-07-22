#pragma once
#include <inttypes.h>

// установить часовую зону в часах или минутах
void setStampZone(int16_t zone);

// получить часовую зону в минутах
int16_t getStampZone();