#pragma once
#include <Arduino.h>

#include "./core/StampConvert.h"

class Stamp : public StampConvert {
   public:
    uint32_t unix = 0;

    // ========== CONSTRUCT ==========
    Stamp() {}
    Stamp(const Stamp& s) = default;
    Stamp(uint32_t unix) : unix(unix) {}
    Stamp(const char* str) {
        parse(str);
    }
    Stamp(const Datime& dt) {
        set(dt);
    }
    Stamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        set(year, month, day, hour, minute, second);
    }
    Stamp(uint16_t yh, uint16_t mm, uint16_t ds) {
        set(yh, mm, ds);
    }

    // =========== SET TIME ============
    // установить время из Datime
    void set(const Datime& dt) {
        unix = dt.getUnix();
    }

    // установить время (год, месяц, день, час, минута, секунда)
    void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        set(Datime(year, month, day, hour, minute, second));
    }

    // установить время (год, месяц, день) или (час, минута, секунда)
    void set(uint16_t yh, uint16_t mm, uint16_t ds) {
        set(Datime(yh, mm, ds));
    }

    // =========== ADD ============
    // добавить секунды
    void addSeconds(uint32_t s) {
        unix += s;
    }

    // добавить минуты
    void addMinutes(uint32_t m) {
        unix += m * 60ul;
    }

    // добавить часы
    void addHours(uint32_t h) {
        unix += h * 3600ul;
    }

    // добавить дни
    void addDays(uint32_t d) {
        unix += d * 86400ul;
    }

    // =========== PARSE ============
    // hh:mm:ss или yyyy-mm-dd или yyyy-mm-ddThh:mm:ss
    bool parse(const char* s) {
        Datime dt;
        if (!dt.parse(s)) return 0;
        set(dt);
        return 1;
    }

    // <day_week>, dd <month> yyyy hh:mm:ss
    bool parseHTTP(const char* s) {
        Datime dt;
        if (!dt.parseHTTP(s)) return 0;
        set(dt);
        return 1;
    }

    // =========== OVERLOAD ===========
    // получить время в секундах
    uint32_t getUnix() const override {
        return unix;
    }
};