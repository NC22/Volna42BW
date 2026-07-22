#include "StampUtils.h"

namespace StampUtils {

static const uint8_t dim_table[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t timeToSeconds(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    return (hours * 60 + minutes) * 60ul + seconds;
}

bool isLeap(uint16_t year) {
    return (((year & 3) == 0 && year % 100 != 0) || (year % 400 == 0));
}

uint8_t daysInMonth(uint8_t month) {
    return (month >= 1 && month <= 12) ? pgm_read_byte(&dim_table[month - 1]) : 0;
}

uint8_t daysInMonth(uint8_t month, uint16_t year) {
    return (month == 2 && isLeap(year)) ? (daysInMonth(month) + 1) : daysInMonth(month);
}

uint16_t daysToMonth(uint8_t month, uint16_t year) {
    uint16_t days = 0;
    for (uint8_t i = 1; i < month; i++) days += daysInMonth(i, year);
    return days;
}

uint16_t dateToYearDay(uint8_t day, uint8_t month, uint16_t year) {
    return day + daysToMonth(month, year);
}

uint16_t dateToDays2000(uint8_t day, uint8_t month, uint16_t year) {
    uint16_t yday = dateToYearDay(day, month, year);
    if (year >= 2000) year -= 2000;
    return yday + 365 * year + (year + 3) / 4 - (year + 99) / 100 + (year + 399) / 400 - 1;
}

uint8_t dateToWeekDay(uint8_t day, uint8_t month, uint16_t year) {
    return (dateToDays2000(day, month, year) + 5) % 7 + 1;
}

uint32_t dateToUnix(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, int16_t zone) {
    return _STAMP_2000_1_1 + (dateToDays2000(day, month, year)) * 86400ul + timeToSeconds(hour, minute, second) - zone * 60;
}

}  // namespace StampUtils
