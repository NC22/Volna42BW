#pragma once
#include <inttypes.h>
// https://stackoverflow.com/a/44271643

#define _BS_SEC_DAY 86400UL
#define _BS_SEC_YEAR (_BS_SEC_DAY * 365)
#define _BS_FDAY(year) (((year) % 400) == 0UL ? 29UL : (((year) % 100) == 0UL ? 28UL : (((year) % 4) == 0UL ? 29UL : 28UL)))

#define _BS_DEC(c) (c & 0xf)
#define _BS_DEC_2(str, i) (_BS_DEC(str[i]) * 10 + _BS_DEC(str[i + 1]))
#define _BS_DEC_4(str, i) (_BS_DEC(str[i]) * 1000 + _BS_DEC(str[i + 1]) * 100 + _BS_DEC(str[i + 2]) * 10 + _BS_DEC(str[i + 3]))

#define _BS_MONTH(str, i) (                   \
    str[i] == 'J' && str[i + 1] == 'a'   ? 1  \
    : str[i] == 'F'                      ? 2  \
    : str[i] == 'M' && str[i + 2] == 'r' ? 3  \
    : str[i] == 'A' && str[i + 1] == 'p' ? 4  \
    : str[i] == 'M'                      ? 5  \
    : str[i] == 'J' && str[i + 2] == 'n' ? 6  \
    : str[i] == 'J'                      ? 7  \
    : str[i] == 'A'                      ? 8  \
    : str[i] == 'S'                      ? 9  \
    : str[i] == 'O'                      ? 10 \
    : str[i] == 'N'                      ? 11 \
    : str[i] == 'D'                      ? 12 \
                                         : 0)

#define _BS_YDAY(year, month, day) (      \
    day +                                 \
    (month >= 2 ? 31UL : 0UL) +           \
    (month >= 3 ? _BS_FDAY(year) : 0UL) + \
    (month >= 4 ? 31UL : 0UL) +           \
    (month >= 5 ? 30UL : 0UL) +           \
    (month >= 6 ? 31UL : 0UL) +           \
    (month >= 7 ? 30UL : 0UL) +           \
    (month >= 8 ? 31UL : 0UL) +           \
    (month >= 9 ? 31UL : 0UL) +           \
    (month >= 10 ? 30UL : 0UL) +          \
    (month >= 11 ? 31UL : 0UL) +          \
    (month >= 12 ? 30UL : 0UL))

#define _BS_STAMP(year, month, day, hour, minute, second) \
    (second +                                             \
     minute * 60UL +                                      \
     hour * 60UL * 60UL +                                 \
     (_BS_YDAY(year, month, day) - 1) * _BS_SEC_DAY +     \
     (year - 1970UL) * _BS_SEC_YEAR +                     \
     ((year - 1969UL) / 4UL) * _BS_SEC_DAY -              \
     ((year - 1901UL) / 100UL) * _BS_SEC_DAY +            \
     ((year - 1601UL) / 400UL) * _BS_SEC_DAY)

// public
#define __TIME_SEC__ _BS_DEC_2(__TIME__, 6)
#define __TIME_MIN__ _BS_DEC_2(__TIME__, 3)
#define __TIME_HOUR__ _BS_DEC_2(__TIME__, 0)
#define __DATE_DAY__ _BS_DEC_2(__DATE__, 4)
#define __DATE_MONTH__ _BS_MONTH(__DATE__, 0)
#define __DATE_YEAR__ _BS_DEC_4(__DATE__, 7)
#define __TIME_UNIX__ (_BS_STAMP(__DATE_YEAR__, __DATE_MONTH__, __DATE_DAY__, __TIME_HOUR__, __TIME_MIN__, __TIME_SEC__))

//
#define BUILD_STAMP_LOCAL __TIME_UNIX__