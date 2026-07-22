#include <Arduino.h>
#include <StampKeeper.h>

StampKeeper rtc;  // GyverNTP, GyverDS3231, Settings.rtc

void setup() {
    Serial.begin(115200);

    // часовой пояс, установить до всех действий с объектами
    setStampZone(3);

    // обработчик секунды (вызывается из тикера)
    rtc.onSecond([]() {
        Serial.println("new second!");
    });

    // обработчик синхронизации (вызывается из sync)
    rtc.onSync([](uint32_t unix) {
        Serial.println("sync: ");
        Serial.print(unix);
    });

    // синхронизация вручную для примера
    // rtc.sync(1738237474);
}
void loop() {
    // тикер вернёт true каждую секунду в 0 мс секунды, если время синхронизировано
    if (rtc.tick()) {
        // вывод даты и времени строкой
        Serial.print(rtc.toString());  // rtc.timeToString(), rtc.dateToString()
        Serial.print(':');
        Serial.println(rtc.ms());  // + миллисекунды текущей секунды. Внутри tick всегда равно 0

        // вывод в Datime
        Datime dt = rtc;  // или Datime dt(rtc)
        Serial.println(dt.year);
        Serial.println(dt.second);
        Serial.println(dt.hour);
        dt.weekDay();
        dt.yearDay();
        // ... и прочие методы и переменные Datime

        // чтение напрямую, медленнее чем вывод в Datime
        rtc.second();
        rtc.minute();
        rtc.year();
        // ... и прочие методы StampConvert

        // сравнение
        rtc == DaySeconds(12, 35, 0);            // сравнение с DaySeconds (время равно 12:35:00)
        rtc == 1738237474ul;                     // сравнение с unix
        rtc == Datime(2025, 1, 30, 14, 14, 30);  // сравнение с Datime
    }

    if (rtc.newSecond()) {
        // новую секунду можно поймать и здесь
    }
}
