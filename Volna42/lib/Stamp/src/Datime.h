#pragma once
#include <Arduino.h>

#include "./core/StampUtils.h"
#include "./core/stamp_zone.h"
#include "DaySeconds.h"

#define UNIX_ALG_0 0       // ~402us / ~94B Flash (AVR)
#define UNIX_ALG_1 1       // ~298us / ~138B Flash (AVR)
#define UNIX_ALG_2 2       // ~216us / ~584B Flash (AVR)
#define UNIX_ALG_3 3       // ~297us / ~178B Flash (AVR)
#define UNIX_ALG_TIME_T 4  // ~246us / ~842B Flash (AVR)

#ifndef UNIX_ALG
#define UNIX_ALG UNIX_ALG_3
#endif
#define _UNIX_ALG UNIX_ALG

#if _UNIX_ALG == UNIX_ALG_TIME_T
#include <time.h>
#endif

class Datime {
   public:
    // год
    uint16_t year = 2000;

    // месяц (1.. 12)
    uint8_t month = 1;

    // день месяца (1.. 28-31)
    uint8_t day = 1;

    // час (0.. 23)
    uint8_t hour = 0;

    // минута (0.. 59)
    uint8_t minute = 0;

    // секунда (0.. 59)
    uint8_t second = 0;

    // день недели (1 пн.. 7 вс)
    uint8_t weekDay() const {
        return StampUtils::dateToWeekDay(day, month, year);
    }

    // день года (1.. 365-366)
    uint16_t yearDay() const {
        return StampUtils::dateToYearDay(day, month, year);
    }

    // uint8_t weekDay = 1;
    // uint16_t yearDay = 1;

    // ========= CONSTRUCTOR =========
    Datime() {}
    // Datime(const Datime& dat) = default;
    Datime(const char* str) {
        parse(str);
    }
    Datime(uint32_t unix) {
        set(unix);
    }
    Datime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        set(year, month, day, hour, minute, second);
    }
    Datime(uint16_t yh, uint16_t mm, uint16_t ds) {
        set(yh, mm, ds);
    }

    // Datime& operator=(uint32_t unix) {
    //     set(unix);
    //     return *this;
    // }

    // ============= SET =============
    // установить время (год, месяц, день, час, минута, секунда)
    void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        Datime::year = year;
        Datime::month = month;
        Datime::day = day;
        Datime::hour = hour;
        Datime::minute = minute;
        Datime::second = second;
        // updateDays();
    }

    // установить время (год, месяц, день) или (час, минута, секунда)
    void set(uint16_t yh, uint16_t mm, uint16_t ds) {
        if (yh >= 24) setDate(yh, mm, ds);
        else setTime(yh, mm, ds);
    }

    // установить дату (год, месяц, день)
    void setDate(uint16_t y, uint16_t m, uint16_t d) {
        year = y;
        month = m;
        day = d;
        // updateDays();
    }

    // установить время (час, минута, секунда)
    void setTime(uint8_t h, uint8_t m, uint8_t s) {
        hour = h;
        minute = m;
        second = s;
    }

    // установить из unix времени и глобального часового пояса setStampZone
    void set(uint32_t unix) {
        if (!unix) return;
        unix += getStampZone() * 60l;

#if _UNIX_ALG == UNIX_ALG_0
        Datime::second = unix % 60ul;
        unix /= 60ul;
        Datime::minute = unix % 60ul;
        unix /= 60ul;
        Datime::hour = unix % 24ul;
        unix /= 24ul;
        // Datime::weekDay = (unix + 3) % 7 + 1;
        uint32_t z = unix + 719468;
        uint8_t era = z / 146097ul;
        uint16_t doe = z - era * 146097ul;
        uint16_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        uint16_t y = yoe + era * 400;
        uint16_t doy = doe - (yoe * 365 + yoe / 4 - yoe / 100);
        uint16_t mp = (doy * 5 + 2) / 153;
        Datime::day = doy - (mp * 153 + 2) / 5 + 1;
        Datime::month = mp + (mp < 10 ? 3 : -9);
        y += (Datime::month <= 2);
        Datime::year = y;
        // Datime::yearDay = StampUtils::dateToYearDay(Datime::day, Datime::month, Datime::year);

#elif _UNIX_ALG == UNIX_ALG_1
        int32_t days, rem;
        int years400, years100, years4, remainingyears;

        days = unix / 86400ul - 11017;
        rem = unix % 86400ul;
        if (rem < 0) {
            rem += 86400ul;
            days--;
        }

        Datime::hour = rem / 3600ul;
        rem %= 3600ul;
        Datime::minute = rem / 60ul;
        Datime::second = rem % 60ul;

        // if ((Datime::weekDay = ((3 + days) % 7)) < 0) Datime::weekDay += 7;
        // if (!Datime::weekDay) Datime::weekDay = 7;

        years400 = days / 146097L;
        days -= years400 * 146097L;
        if (days < 0) {
            days += 146097L;
            years400--;
        }

        years100 = days / 36524L;
        if (years100 == 4) years100--;
        days -= years100 * 36524L;
        years4 = days / 1461L;
        days -= years4 * 1461L;
        remainingyears = days / 365L;
        if (remainingyears == 4) remainingyears--;
        days -= remainingyears * 365L;
        Datime::year = 2000 + years400 * 400 + years100 * 100 + years4 * 4 + remainingyears;
        bool yearleap = remainingyears == 0 && (years4 != 0 || years100 == 0);

        // Datime::yearDay = days + 31 + 28 + yearleap;
        // if (Datime::yearDay >= 365u + yearleap) {
        //     Datime::yearDay -= 365u + yearleap;
        //     Datime::year++;
        // }
        // Datime::yearDay++;

        uint16_t yearDay = days + 31 + 28 + yearleap;
        if (yearDay >= 365u + yearleap) {
            yearDay -= 365u + yearleap;
            Datime::year++;
        }

        Datime::month = 2;
        while (1) {
            uint8_t dm = StampUtils::daysInMonth(Datime::month + 1);  // from 0. Feb 28 here
            if (Datime::month == 1) dm++;                             // 1 Feb
            if (days < dm) break;
            days -= dm;
            if (++Datime::month >= 12) Datime::month = 0;
        }
        Datime::month++;
        Datime::day = days + 1;

#elif _UNIX_ALG == UNIX_ALG_2
        int32_t fract;
        uint16_t days, n, leapyear, years;
        ldiv_t lresult;
        div_t result;
        if (unix < _STAMP_2000_1_1) unix = _STAMP_2000_1_1;

        unix -= _STAMP_2000_1_1;        // to 2000-01-01 UTC
        days = unix / 86400UL;          // 38753+
        fract = unix - days * 86400UL;  // 86400

        lresult = ldiv(fract, 60L);  // 86400
        Datime::second = lresult.rem;
        result = div((int)lresult.quot, 60);  // 1440
        Datime::minute = result.rem;
        Datime::hour = result.quot;

        n = days + 6;
        n %= 7;
        // Datime::weekDay = n;
        // if (!Datime::weekDay) Datime::weekDay = 7;

        lresult = ldiv((long)days, 36525L);  // 38753+
        years = 100 * lresult.quot;
        lresult = ldiv(lresult.rem, 1461L);  // 36525
        years += 4 * lresult.quot;
        days = lresult.rem;  // <- 1461
        if (years > 100) days++;

        leapyear = 1;
        if (years == 100) leapyear = 0;
        n = 364 + leapyear;

        if (days > n) {
            days -= leapyear;
            leapyear = 0;
            result = div(days, 365);  // 1461
            years += result.quot;
            days = result.rem;  // <- 365
        }
        Datime::year = 100 + years + 1900;
        // Datime::yearDay = days + 1;

        n = 59 + leapyear;
        if (days < n) {
            result = div(days, 31);  // 1461
            Datime::month = result.quot;
            Datime::day = result.rem;
        } else {
            days -= n;
            result = div(days, 153);  // 1461
            Datime::month = 2 + result.quot * 5;
            result = div(result.rem, 61);  // 153
            Datime::month += result.quot * 2;
            result = div(result.rem, 31);  // 61
            Datime::month += result.quot;
            Datime::day = result.rem;
        }

        Datime::day++;
        Datime::month++;

#elif _UNIX_ALG == UNIX_ALG_3
        unix -= _STAMP_2000_1_1;
        Datime::second = unix % 60ul;
        unix /= 60ul;
        Datime::minute = unix % 60ul;
        unix /= 60ul;
        Datime::hour = unix % 24ul;

        uint16_t days = unix / 24ul;
        // Datime::weekDay = (days + 5) % 7 + 1;

        bool leap = 0;
        for (Datime::year = 0;; Datime::year++) {
            leap = !(Datime::year & 3);
            if (days < 365u + leap) break;
            days -= 365u + leap;
        }
        Datime::year += 2000;
        // Datime::yearDay = days + 1;

        for (Datime::month = 1; Datime::month < 12; Datime::month++) {
            uint8_t dm = StampUtils::daysInMonth(Datime::month);
            if (leap && Datime::month == 2) dm++;
            if (days < dm) break;
            days -= dm;
        }
        Datime::day = days + 1;

#elif _UNIX_ALG == UNIX_ALG_TIME_T
        time_t t = unix - _STAMP_2000_1_1;
        tm tt;
        gmtime_r(&t, &tt);
        Datime::year = tt.tm_year + 1900;
        Datime::month = tt.tm_mon + 1;
        Datime::day = tt.tm_mday;
        Datime::hour = tt.tm_hour;
        Datime::minute = tt.tm_min;
        Datime::second = tt.tm_sec;
        // Datime::weekDay = tt.tm_wday;
        // if (!Datime::weekDay) Datime::weekDay = 7;
        // Datime::yearDay = tt.tm_yday + 1;
#endif
    }

    // =========== EXPORT ============
    // вывести в секунды с начала текущих суток
    uint32_t daySeconds() const {
        uint32_t sec = second;
        if (minute) sec += minute * 60;
        if (hour) sec += hour * 3600ul;
        return sec;
    }

    // вывести в unix-секунды
    uint32_t getUnix() const {
        return StampUtils::dateToUnix(day, month, year, hour, minute, second, getStampZone());
    }

    // ========== TO STRING ==========
    // вывести дату в формате "dd.mm.yyyy" [11]. Вернёт указатель на конец строки
    char* dateToChar(char* buf) const {
        _btoa(day, buf + 0);
        buf[2] = '.';
        _btoa(month, buf + 3);
        buf[5] = '.';
        itoa(min(year, (uint16_t)9999), buf + 6, DEC);
        return buf + 10;
    }
    // вывести дату в формате "yyyy-mm-dd" [11]. Вернёт указатель на конец строки
    char* dateToCharISO(char* buf) const {
        itoa(min(year, (uint16_t)9999), buf, DEC);
        buf[4] = '-';
        _btoa(month, buf + 5);
        buf[7] = '-';
        _btoa(day, buf + 8);
        buf[10] = 0;
        return buf + 10;
    }

    // вывести дату в формате "dd.mm.yyyy"
    String dateToString() const {
        char buf[11];
        dateToChar(buf);
        return buf;
    }
    // вывести дату в формате "dd.mm.yyyy"
    String dateToStringISO() const {
        char buf[11];
        dateToCharISO(buf);
        return buf;
    }

    // вывести время в формате "hh:mm:ss" [9]. Вернёт указатель на конец строки
    char* timeToChar(char* buf) const {
        _btoa(hour, buf + 0);
        buf[2] = ':';
        _btoa(minute, buf + 3);
        buf[5] = ':';
        _btoa(second, buf + 6);
        buf[8] = 0;
        return buf + 8;
    }

    // вывести время в формате "hh:mm:ss"
    String timeToString() const {
        char buf[9];
        timeToChar(buf);
        return buf;
    }

    // вывести в формате "dd.mm.yyyy hh:mm:ss" [20]. Вернёт указатель на конец строки
    char* toChar(char* buf, char div = ' ') const {
        dateToChar(buf);
        buf[10] = div;
        timeToChar(buf + 11);
        return buf + 19;
    }
    // вывести в формате "yyyy-mm-dd hh:mm:ss" [20]. Вернёт указатель на конец строки
    char* toCharISO(char* buf, char div = ' ') const {
        dateToCharISO(buf);
        buf[10] = div;
        timeToChar(buf + 11);
        return buf + 19;
    }

    // вывести в формате "dd.mm.yyyy hh:mm:ss"
    String toString(char div = ' ') const {
        char buf[20];
        toChar(buf, div);
        return buf;
    }
    // вывести в формате "dd.mm.yyyy hh:mm:ss"
    String toStringISO(char div = ' ') const {
        char buf[20];
        toCharISO(buf, div);
        return buf;
    }

    // ========== PARSE ==========
    // yyyy-mm-dd
    bool parseDate(const char* s) {
        if (!s) return 0;
        if (strlen(s) < 10) return 0;

        uint16_t y = atoi(s);
        const char* p = strchr(s, '-');
        if (!p) return 0;
        uint8_t m = atoi(++p);
        p = strchr(p, '-');
        if (!p) return 0;
        uint8_t d = atoi(++p);

        if (!_valid(y, m, d, hour, minute, second)) return 0;
        year = y;
        month = m;
        day = d;
        return 1;
    }

    // hh:mm:ss
    bool parseTime(const char* s) {
        if (!s) return 0;
        if (strlen(s) < 8) return 0;

        uint8_t h = atoi(s);
        const char* p = strchr(s, ':');
        if (!p) return 0;
        uint8_t m = atoi(++p);
        p = strchr(p, ':');
        if (!p) return 0;
        uint8_t sec = atoi(++p);

        if (!_valid(year, month, day, h, m, sec)) return 0;
        hour = h;
        minute = m;
        second = sec;
        return 1;
    }

    // hh:mm:ss или yyyy-mm-dd или yyyy-mm-dd hh:mm:ss
    bool parse(const char* s) {
        if (!s) return 0;
        uint16_t len = strlen(s);
        Datime dt = *this;
        if (len == 19) {  // dateXtime
            if (!dt.parseDate(s)) return 0;
            if (!dt.parseTime(s + 11)) return 0;
        } else if (len == 10) {  // date
            if (!dt.parseDate(s)) return 0;
        } else if (len == 8) {  // time
            if (!dt.parseTime(s)) return 0;
        } else {
            return 0;
        }
        *this = dt;
        return 1;
    }

    // <day_week>, dd <month> yyyy hh:mm:ss
    bool parseHTTP(const char* s) {
        if (!s) return 0;
        char* comma = strchr(s, ',');
        if (!comma) return 0;

        s = comma + 2;
        if (strlen(s) < 20) return 0;

        uint8_t d = atoi(s);
        uint8_t mo = 0;
        switch (s[3]) {
            case 'J': mo = (s[4] == 'a') ? 1 : ((s[5] == 'n') ? 6 : 7); break;
            case 'F': mo = 2; break;
            case 'A': mo = (s[5] == 'r') ? 4 : 8; break;
            case 'M': mo = (s[5] == 'r') ? 3 : 5; break;
            case 'S': mo = 9; break;
            case 'O': mo = 10; break;
            case 'N': mo = 11; break;
            case 'D': mo = 12; break;
            default: return 0;
        }

        uint16_t y = atoi(s + 7);
        uint8_t h = atoi(s + 12);
        uint8_t m = atoi(s + 15);
        uint8_t sec = atoi(s + 18);
        if (!_valid(y, mo, d, h, m, sec)) return 0;

        year = y;
        month = mo;
        day = d;
        hour = h;
        minute = m;
        second = sec;
        return 1;
    }

    // =========== COMPARE ===========
    // сравнить, вернёт 0 если равны
    int8_t compare(const Datime& dt) const {
        if (year != dt.year) return (year < dt.year) ? -1 : 1;
        if (month != dt.month) return (month < dt.month) ? -1 : 1;
        if (day != dt.day) return (day < dt.day) ? -1 : 1;
        if (hour != dt.hour) return (hour < dt.hour) ? -1 : 1;
        if (minute != dt.minute) return (minute < dt.minute) ? -1 : 1;
        if (second != dt.second) return (second < dt.second) ? -1 : 1;
        return 0;
    }

    bool operator==(const Datime& dt) const { return compare(dt) == 0; }
    bool operator!=(const Datime& dt) const { return compare(dt) != 0; }
    bool operator<(const Datime& dt) const { return compare(dt) < 0; }
    bool operator<=(const Datime& dt) const { return compare(dt) <= 0; }
    bool operator>(const Datime& dt) const { return compare(dt) > 0; }
    bool operator>=(const Datime& dt) const { return compare(dt) >= 0; }

    bool operator==(const DaySeconds& ds) const { return daySeconds() == ds.seconds; }
    bool operator!=(const DaySeconds& ds) const { return daySeconds() != ds.seconds; }
    bool operator>(const DaySeconds& ds) const { return daySeconds() > ds.seconds; }
    bool operator>=(const DaySeconds& ds) const { return daySeconds() >= ds.seconds; }
    bool operator<(const DaySeconds& ds) const { return daySeconds() < ds.seconds; }
    bool operator<=(const DaySeconds& ds) const { return daySeconds() <= ds.seconds; }

    bool operator==(uint32_t u) const { return getUnix() == u; }
    bool operator!=(uint32_t u) const { return getUnix() != u; }
    bool operator>(uint32_t u) const { return getUnix() > u; }
    bool operator>=(uint32_t u) const { return getUnix() >= u; }
    bool operator<(uint32_t u) const { return getUnix() < u; }
    bool operator<=(uint32_t u) const { return getUnix() <= u; }

    // одинаковое время
    bool equals(const Datime& dt) const {
        return compare(dt) == 0;
    }

    // високосный ли год
    bool isLeap() const {
        return StampUtils::isLeap(year);
    }

    // дата и время корректны
    bool valid() const {
        return _valid(year, month, day, hour, minute, second);
    }

    // дата 01.01.2000
    bool isY2K() const {
        return year == 2000 && month == 1 && day == 1;
    }

    // день года как индекс массива от 0 до 365 независимо от високосного года. 29 февраля имеет индекс 59
    uint16_t dayIndex() const {
        uint16_t yday = yearDay();
        // return ((yearDay >= 60) ? (isLeap() ? yearDay : yearDay + 1) : yearDay) - 1;
        return ((yday >= 60) ? (isLeap() ? yday : yday + 1) : yday) - 1;
    }

    // ============= ADD =============
    // добавить секунды
    void addSeconds(uint32_t s) {
        if (!s) return;
        if (s == 1) {
            second++;
            _update();
        } else {
            s += second;
            ldiv_t res;
            res = ldiv(s, 60L);
            second = res.rem;
            addMinutes(res.quot);
        }
    }

    // добавить минуты
    void addMinutes(uint32_t m) {
        if (!m) return;
        if (m == 1) {
            minute++;
            _update();
        } else {
            m += minute;
            ldiv_t res;
            res = ldiv(m, 60L);
            minute = res.rem;
            addHours(res.quot);
        }
    }

    // добавить часы
    void addHours(uint32_t h) {
        if (!h) return;
        if (h == 1) {
            hour++;
            _update();
        } else {
            h += hour;
            ldiv_t res;
            res = ldiv(h, 24L);
            hour = res.rem;
            addDays(res.quot);
        }
    }

    // добавить дни
    void addDays(uint32_t d) {
        while (d) {
            --d;
            ++day;
            // weekDay++;
            // yearDay++;
            _update();
        }
        /*
        // fast
        while (d) {
            uint8_t dim = StampUtils::daysInMonth(month, year);
            uint32_t leftInMonth = dim - day;

            if (d <= leftInMonth) {
                day += d;
                break;
            }

            d -= (leftInMonth + 1);
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
        */
    }

    // ============= NEXT =============
    // следующая секунда
    void nextSecond() {
        addSeconds(1);
    }

    // следующая минута (xx:xx:00)
    void nextMinute() {
        addMinutes(1);
        second = 0;
    }

    // следующий час (xx:00:00)
    void nextHour() {
        addHours(1);
        second = minute = 0;
    }

    // следующий день (00:00:00)
    void nextDay() {
        addDays(1);
        second = minute = hour = 0;
    }

    // следующий месяц (1 число 00:00:00)
    void nextMonth() {
        month++;
        day = 1;
        second = minute = hour = 0;
        if (month > 12) {
            year++;
            month = 1;
        }
        // updateDays();
    }

    // обновить weekDay и yearDay исходя из текущей даты (после ручного изменения)
    void updateDays() {
        // weekDay = StampUtils::dateToWeekDay(day, month, year);
        // yearDay = StampUtils::dateToYearDay(day, month, year);
    }

   private:
    static bool _valid(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        return (year >= 2000) && (month >= 1 && month <= 12) && (day >= 1 && day <= StampUtils::daysInMonth(month, year)) && (hour <= 23) && (minute <= 59) && (second <= 59);
    }

    void _update() {
        if (second > 59) {
            second = 0;
            minute++;
        }
        if (minute > 59) {
            minute = 0;
            hour++;
        }
        if (hour > 23) {
            hour = 0;
            // weekDay++;
            // yearDay++;
            day++;
        }
        // if (weekDay > 7) weekDay = 1;
        if (day > StampUtils::daysInMonth(month, year)) {
            month++;
            day = 1;
        }
        if (month > 12) {
            year++;
            month = 1;
            // yearDay = 1;
        }
    }

    void _btoa(uint8_t v, char* str) const {
        uint8_t d = v / 10;
        str[0] = d + '0';
        str[1] = v - d * 10 + '0';
    }
};
