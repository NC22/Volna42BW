#pragma once
#include <Arduino.h>

#include "../Datime.h"
#include "../DaySeconds.h"

class StampConvert {
   public:
    virtual uint32_t getUnix() const = 0;

    // =========== GET TIME ===========
    // экспортировать в локальное время Datime
    Datime now() const {
        uint32_t u = getUnix();
        return Datime(u);
    }

    operator Datime() const {
        return getUnix();
    }
    operator uint32_t() const {
        return getUnix();
    }

    // ============ TO STRING ============
    // вывести дату в формате "dd.mm.yyyy"
    char* dateToChar(char* buf) const {
        return now().dateToChar(buf);
    }
    String dateToString() const {
        return now().dateToString();
    }

    // вывести время в формате "hh:mm:ss"
    char* timeToChar(char* buf) const {
        return now().timeToChar(buf);
    }
    String timeToString() const {
        return now().timeToString();
    }

    // вывести время и дату в формате "dd.mm.yyyy hh:mm:ss"
    char* toChar(char* buf, char div = ' ') const {
        return now().toChar(buf, div);
    }
    String toString(char div = ' ') const {
        return now().toString(div);
    }

    // ============ EXPORT EPOCH ============

    // получить секунды с epoch
    uint32_t toSeconds() const {
        return getUnix();
    }

    // получить минуты с epoch
    uint32_t toMinutes() const {
        return getUnix() / 60ul;
    }

    // получить часы с epoch
    uint32_t toHours() const {
        return getUnix() / 3600ul;
    }

    // получить сутки с epoch
    uint32_t toDays() const {
        return getUnix() / 86400ul;
    }

    // ============ DATIME ============

    // получить секунды с начала текущих суток (локальное время)
    uint32_t daySeconds() const {
        return _localUnix() % 86400;
    }

    // получить текущие секунды
    uint8_t second() const {
        return _localUnix() % 60ul;
    }

    // получить текущие минуты
    uint8_t minute() const {
        return (_localUnix() / 60ul) % 60ul;
    }

    // получить текущие часы
    uint8_t hour() const {
        return (_localUnix() / 3600ul) % 24ul;
    }

    // получить текущий день месяца
    uint8_t day() const {
        return now().day;
    }

    // получить текущий месяц
    uint8_t month() const {
        return now().month;
    }

    // получить текущий год
    uint16_t year() const {
        return now().year;
    }

    // получить текущий день недели
    uint8_t weekDay() const {
        return now().weekDay();
    }

    // получить текущий день года
    uint16_t yearDay() const {
        return now().yearDay();
    }

    // =========== OVERLOAD ===========
    bool operator==(const Datime& dt) const { return getUnix() == dt.getUnix(); }
    bool operator!=(const Datime& dt) const { return getUnix() != dt.getUnix(); }
    bool operator>(const Datime& dt) const { return getUnix() > dt.getUnix(); }
    bool operator>=(const Datime& dt) const { return getUnix() >= dt.getUnix(); }
    bool operator<(const Datime& dt) const { return getUnix() < dt.getUnix(); }
    bool operator<=(const Datime& dt) const { return getUnix() <= dt.getUnix(); }

    bool operator==(const StampConvert& s) const { return getUnix() == s.getUnix(); }
    bool operator!=(const StampConvert& s) const { return getUnix() != s.getUnix(); }
    bool operator>(const StampConvert& s) const { return getUnix() > s.getUnix(); }
    bool operator>=(const StampConvert& s) const { return getUnix() >= s.getUnix(); }
    bool operator<(const StampConvert& s) const { return getUnix() < s.getUnix(); }
    bool operator<=(const StampConvert& s) const { return getUnix() <= s.getUnix(); }

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

    // deprecated
    // экспортировать в локальное время Datime
    Datime get() const {
        return getUnix();
    }

    // экспортировать в переменную типа Datime
    void get(Datime& dt) const {
        dt = getUnix();
    }

   private:
    uint32_t _localUnix() const {
        return getUnix() + getStampZone() * 60l;
    }
};
