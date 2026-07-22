#include <Arduino.h>
#include <Stamp.h>

void setup() {
    Serial.begin(115200);
    
    Stamp s(2023, 5, 8, 16, 26, 0);
    Serial.println(s.toString());

    Datime d = s.now();
    Serial.println(d.year);
    Serial.println(d.month);
    Serial.println(d.day);

    d.year = 2022;  // изменили год
    s.set(d);       // обновили Stamp
    Serial.println(s.toString());
}
void loop() {
}