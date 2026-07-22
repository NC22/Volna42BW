#pragma once
#include <Arduino.h>

#define _STAMP_2000_1_1 946684800ul

namespace StampUtils {

// время в секунды
uint32_t timeToSeconds(uint8_t hours, uint8_t minutes, uint8_t seconds);

// високосный год
bool isLeap(uint16_t year);

// дней в месяце без учёта года (февраль 28)
uint8_t daysInMonth(uint8_t month);

// дней в месяце с учётом високосного года
uint8_t daysInMonth(uint8_t month, uint16_t year);

// дней года к месяцу (янв 0, фев 31, март 59/60...)
uint16_t daysToMonth(uint8_t month, uint16_t year);

// дата в день текущего года (начиная с 1)
uint16_t dateToYearDay(uint8_t day, uint8_t month, uint16_t year);

// дата в день недели (пн 1.. вс 7)
uint8_t dateToWeekDay(uint8_t day, uint8_t month, uint16_t year);

// дата в количество дней с 01.01.2000 (начиная с 0)
uint16_t dateToDays2000(uint8_t day, uint8_t month, uint16_t year);

// дата в unix время, zone в минутах
uint32_t dateToUnix(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t seconds, int16_t zone = 0);

}  // namespace StampUtils