#pragma once
#include <Arduino.h>

// Синхронизатор Unix, getUnix() выдаёт актуальное время на базе установленного
// в update + millis() с момента обновления

#include "./core/StampConvert.h"

#define STAMP_SYNC_LOOP_PRD (7ul * 24 * 60 * 60 * 1000)

class StampSync : public StampConvert {
   public:
    // установить unix и миллисекунды
    StampSync(uint32_t unix = 0, uint16_t ms = 0) {
        update(unix, ms);
    }

    // установить unix и миллисекунды
    void update(uint32_t unix, uint16_t ms = 0) {
        _unix = unix;
        _syncTime = millis() - ms;
    }

    // время синхронизировано
    inline bool synced() const {
        return _unix;
    }

    // время синхронизировано
    explicit operator bool() const {
        return synced();
    }

    // получить текущий unix
    uint32_t getUnix() const {
        if (!synced()) return 0;
        uint32_t diff = millis() - _syncTime;
        if (diff > STAMP_SYNC_LOOP_PRD) {
            uint32_t sec = diff / 1000ul;
            _syncTime += sec * 1000ul;
            _unix += sec;
            return _unix;
        }
        return _unix + diff / 1000ul;
    }

    // получить миллисекунды текущей секунды
    uint16_t ms() const {
        return synced() ? ((millis() - _syncTime) % 1000ul) : 0;
    }

    // получить миллисекунды с epoch
    uint64_t unixMs() const {
        return synced() ? (getUnix() * 1000ull + ms()) : 0;
    }

   private:
    mutable uint32_t _unix = 0;
    mutable uint32_t _syncTime = 0;
};