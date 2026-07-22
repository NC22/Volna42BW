#include <Arduino.h>
// сравнение с time.h

#include <Stamp.h>
#include <time.h>

void print_t(tm& t) {
    Serial.println(String() + (t.tm_year + 1900) + '.' + (t.tm_mon + 1) + '.' + (t.tm_mday) + ' ' + (t.tm_hour) + ':' + (t.tm_min) + ':' + (t.tm_sec));
}
void print_dt(Datime& t) {
    Serial.println(String() + (t.year) + '.' + (t.month) + '.' + (t.day) + ' ' + (t.hour) + ':' + (t.minute) + ':' + (t.second));
}

void setup() {
    Serial.begin(115200);
    // unix to date test
    for (uint32_t i = 0; i < 365 * 100ul; i++) {
        uint32_t unix = _STAMP_2000_1_1 + 86400ul * i + random(86400);
        time_t t = unix - _STAMP_2000_1_1;
        tm tt;
        gmtime_r(&t, &tt);

        Stamp dt(unix);
        Datime dt_s = dt.get();

        if (dt_s.year != (uint16_t)(tt.tm_year + 1900) ||
            dt_s.month != (tt.tm_mon + 1) ||
            dt_s.day != (tt.tm_mday) ||
            dt_s.hour != (tt.tm_hour) ||
            dt_s.minute != (tt.tm_min) ||
            dt_s.second != (tt.tm_sec)) {
            Serial.println();
            Serial.println(dt.toString());
            print_t(tt);
        }
    }

    // date to unix test
    for (uint32_t i = 0; i < 365 * 100ul; i++) {
        uint32_t unix = _STAMP_2000_1_1 + 86400ul * i + random(86400);
        time_t t = unix - _STAMP_2000_1_1;
        tm tt;
        gmtime_r(&t, &tt);

        Datime dt_s(tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec);
        Stamp dt(dt_s);

        if (dt.unix != unix) {
            Serial.println();
            Serial.println(dt.toString());
            print_t(tt);
        }
    }

    Serial.println("test end");
}

void loop() {
}
