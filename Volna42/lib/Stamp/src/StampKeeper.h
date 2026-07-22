#pragma once
#include <Arduino.h>

#ifndef __AVR__
#include <functional>
#endif

#include "./core/StampConvert.h"

#define ST_SKIP_THRESHOLD 30  // макс. порог ушедших секунд

class StampKeeper : public StampConvert {
#ifdef __AVR__
    typedef void (*SecondCallback)();
    typedef void (*SyncCallback)(uint32_t unix);
#else
    typedef std::function<void()> SecondCallback;
    typedef std::function<void(uint32_t unix)> SyncCallback;
#endif

   public:
    // установить unix и миллисекунды
    StampKeeper(uint32_t unix = 0, uint16_t ms = 0) {
        sync(unix, ms, true);
    }

    // установить unix и миллисекунды
    void sync(uint32_t unix, uint16_t ms = 0, bool skipTicks = false) {
        if (!unix) return;

        _syncTmr = millis() - ms;
        if (_unix > unix && _unix - unix > ST_SKIP_THRESHOLD) skipTicks = true;
        else if (unix > _unix && unix - _unix > ST_SKIP_THRESHOLD) skipTicks = true;

        if (!_unix || skipTicks) {
            _unix = unix;
            _diff = 0;
        } else {
            _diff = unix - _unix;
        }
        if (_sync_cb) _sync_cb(unix);
    }

    // синхронизировать с другим кипером
    void sync(StampKeeper& keeper, bool skipTicks = false) {
        sync(keeper.getUnix(), keeper.ms(), skipTicks);
    }

    // синхронизировать с Datime
    void sync(const Datime& dt, uint16_t ms = 0, bool skipTicks = false) {
        sync(dt.getUnix(), ms, skipTicks);
    }

    // сбросить синхронизацию
    void reset() {
        _unix = 0;
        _tickFlag = false;
        _ready = false;
    }

    // пропустить отставшие секунды (вызывать после sync)
    void skipTicks() {
        if (_diff) {
            _unix += _diff;
            _diff = 0;
        }
    }

    // есть рассинхронизация, которую нужно "дотикать"
    bool hasDesync() const {
        return _tickFlag && (millis() - _syncTmr >= 1000 || _diff > 0);
    }

    // время синхронизировано
    inline bool synced() const {
        return _unix;
    }

    // секундный флаг
    inline bool newSecond() const {
        return _ready;
    }

    // подключить функцию-обработчик новой секунды (вида void f())
    void onSecond(SecondCallback handler) {
        _sec_cb = handler;
    }

    // подключить функцию-обработчик синхронизации (вида void f(uint32_t unix))
    void onSync(SyncCallback cb) {
        _sync_cb = cb;
    }

    // получить текущий unix
    uint32_t getUnix() const override {
        return synced() ? (_tickFlag ? _unix : (_unix + _diff + (millis() - _syncTmr) / 1000ul)) : 0;
    }

    // получить миллисекунды текущей секунды
    uint16_t ms() const {
        return synced() ? ((millis() - _syncTmr) % 1000ul) : 0;
    }

    // получить миллисекунды с epoch
    uint64_t getUnixMs() const {
        return synced() ? (getUnix() * 1000ull + ms()) : 0;
    }

    // тикер, вызывать в loop. Вернёт true на новой секунде
    bool tick() {
        if (_ready) _ready = false;
        if (synced() && (millis() - _syncTmr >= 1000 || _diff > 0)) {
            if (!_tickFlag) {
                _tickFlag = true;
                skipTicks();
                uint32_t dt = (millis() - _syncTmr) / 1000ul;
                if (dt) --dt;
                _unix += dt;
                _syncTmr += dt * 1000ul;
            }
            if (_diff) {
                if (_diff > 0) {
                    ++_unix;
                    --_diff;
                } else {
                    _syncTmr += 1000;
                    ++_diff;
                    return false;
                }
            } else {
                ++_unix;
                _syncTmr += 1000;
            }
            if (_sec_cb) _sec_cb();
            _ready = true;
            return true;
        }
        return false;
    }

   private:
    SecondCallback _sec_cb = nullptr;
    SyncCallback _sync_cb = nullptr;
    uint32_t _unix = 0;
    uint32_t _syncTmr = 0;
    int16_t _diff = 0;
    bool _tickFlag = false;
    bool _ready = false;
};