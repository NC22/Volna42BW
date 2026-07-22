This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/Stamp.svg?color=brightgreen)](https://github.com/GyverLibs/Stamp/releases/latest/download/Stamp.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Stamp.svg)](https://registry.platformio.org/libraries/gyverlibs/Stamp)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Stamp?_x_tr_sl=ru&_x_tr_tl=en)

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Stamp
Library for storing and manipulating time
- More convenient to use than built-in time. h h
- Faster conversion algorithms are used than in time. h h
- Parsing from lines
- Can count and maintain time based on millis()
- Works until 2106

### Compatibility
Compatible with all Arduino platforms (Arduino features are used)

## Contents
- [Documentation.](#reference)
- [Example](#example)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="reference"></a>

## Documentation.
### Time zone
Configured globally for all library tools

```cpp
void setStampZone(int zone);    // Establish a global time zone in hours or minutes
int getStampZone();             // Get a global time zone in minutes
```

### Utilities.
A set of functions to work with time and date

```cpp
// momentarily
uint32_t StampUtils::timeToSeconds(uint8_t hours, uint8_t minutes, uint8_t seconds);

// leap year
bool StampUtils::isLeap(uint16_t year);

// Days in a month excluding year (February 28)
uint8_t StampUtils::daysInMonth(uint8_t month);

// Days in the month, taking into account the leap year
uint8_t StampUtils::daysInMonth(uint8_t month, uint16_t year);

// Days of year to month (January 0, February 31, March 59/60...)
uint16_t StampUtils::daysToMonth(uint8_t month, uint16_t year);

// date of the current year (starting with 1)
uint16_t StampUtils::dateToYearDay(uint8_t day, uint8_t month, uint16_t year);

// date on the day of the week (pn 1.. c 7)
uint8_t StampUtils::dateToWeekDay(uint8_t day, uint8_t month, uint16_t year);

// date in the number of days from 01.01.2000 (starting from 0)
uint16_t StampUtils::dateToDays2000(uint8_t day, uint8_t month, uint16_t year);

// date in unix time, zone in minutes
uint32_t StampUtils::dateToUnix(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t seconds, int16_t zone = 0);
```

### Macros
```cpp
// compilation
__TIME_SEC__
__TIME_MIN__
__TIME_HOUR__
__DATE_DAY__
__DATE_MONTH__
__DATE_YEAR__

// unix with computer time zone
__TIME_UNIX__
```

### DaySeconds
Stores time in seconds from the beginning of the current day
```cpp
DaySeconds(uint8_t hour, uint8_t minute, uint8_t second);

void set(uint8_t hour, uint8_t minute, uint8_t second);

uint32_t seconds = 0;
```

### Datime
Stores local date and time in individual variables:

```cpp
uint16_t year;      // year
uint8_t month;      // month (1.12)
uint8_t day;        // day of the month (1.28-31)
uint8_t hour;       // hour (0.23)
uint8_t minute;     // minute (0..59)
uint8_t second;     // second (0..59)
uint8_t weekDay();    // day of the week (1 pn. 7 cm) ISO 8601
uint16_t yearDay();   // day of the year (1. 365-366)
```

> These ranges apply to all other functions of the library.

```cpp
Datime() {}
Datime(const char* str);
Datime(uint32_t unix);
Datime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
Datime(uint16_t yh, uint16_t mm, uint16_t ds);
```
```cpp
// ============= SET =============
// set the time (year, month, day, hour, minute, second)
void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

// Set the time (year, month, day) or (hour, minute, second)
void set(uint16_t yh, uint16_t mm, uint16_t ds);

// install from unix time and global time zone setStampZone
void set(uint32_t unix);

// =========== EXPORT ============
// Remove from the beginning of the current day
uint32_t daySeconds();

// single-second
uint32_t getUnix() const;

// ========== TO STRING ==========
// Date the date in the format "dd.mm.yyy" [11]. Returns the pointer to the end of the line
char* dateToChar(char* buf);

// display the date in dd.mm.yyy format
String dateToString();

// Time is in the format of "hh:mm:ss" [9]. Returns the pointer to the end of the line
char* timeToChar(char* buf);

// Time in the format "hhh:mm:ss"
String timeToString();

// output in the format "dd.mm.yyy hh:mm:ss" [20]. Returns the pointer to the end of the line
char* toChar(char* buf, char div = ' ');

// output in dd.mm.yyy hh:mm:ss format
String toString(char div = ' ');

// ========== PARSE ==========
// yyyy-mm-dd
bool parseDate(const char* s);

// hh:mm:ss
bool parseTime(const char* s);

// hh:mm:ss or yyyy-mm-dd or yyyy-mm-ddThh:mm: ss
bool parse(const char* s);

// <day_week>, dd <month> yyyy hh:mm:ss
bool parseHTTP(const char* s);

// =========== COMPARE ===========
// Compared to Datime, uint32 t, DaySeconds

// same-time
bool equals(const Datime& dt);

// leap-year
bool isLeap();

// date and time correct
bool valid();

// Date 01.01. 2000
bool isY2K();

// day of the year as an array index from 0 to 365 regardless of the leap year. February 29 - Index 59
uint16_t dayIndex();

// ============= ADD =============
// second
void addSeconds(uint32_t s);

// minutes
void addMinutes(uint32_t m);

// clock
void addHours(uint32_t h);

// days
void addDays(uint32_t d);

// ============= NEXT =============
// next second
void nextSecond();

// next minute (xx:xx:00)
void nextMinute();

// next hour (xx:00:00)
void nextHour();

// next day (00:00:00)
void nextDay();

// next month (1 date 00:00:00)
void nextMonth();
```
> All time additions to Datime are done directly, without converting to unix.

```cpp
// ==============================
// The time conversion algorithm is set before connecting the library
// for research and just so that
#define UNIX_ALG UNIX_ALG_0       // ~402us and ~94B Flash (AVR)
#define UNIX_ALG UNIX_ALG_1       // ~298us and ~138B Flash (AVR)
#define UNIX_ALG UNIX_ALG_2       // ~216us and ~584B Flash (AVR)
#define UNIX_ALG UNIX_ALG_3       // ~297us and ~178B Flash (AVR)
#define UNIX_ALG UNIX_ALG_TIME_T  // ~246us and ~842B Flash (AVR)
```

### StampConvert
Converter for other classes

```cpp
virtual uint32_t getUnix() const;

// =========== GET TIME ===========
// export at local time Datime
Datime now();

operator Datime();

// ============ TO STRING ============
// display the date in dd.mm.yyy format
char* dateToChar(char* buf);
String dateToString();

// Date the date in yyyy-mm-dd format
char* dateToCharISO(char* buf);
String dateToStringISO();

// Time in the format "hhh:mm:ss"
char* timeToChar(char* buf);
String timeToString();

// Time and date in the format "dd.mm.yyy hh:mm:ss"
char* toChar(char* buf, char div = ' ');
String toString(char div = ' ');

// output in "yyyy-mm-dd hh:mm:ss" format
char* toCharISO(char* buf, char div = ' ');
String toStringISO(char div = ' ');

// ============ EXPORT EPOCH ============

// get seconds from epoch
uint32_t toSeconds();

// Get Minutes From Epoch
uint32_t toMinutes();

// epoch
uint32_t toHours();

// get a day off
uint32_t toDays();

// ============ DATIME ============

// Get seconds from the beginning of the current day (local time)
uint32_t daySeconds();

// get current seconds
uint8_t second();

// get current minutes
uint8_t minute();

// clock up
uint8_t hour();

// get the current day of the month
uint8_t day();

// get the current month
uint8_t month();

// current-year
uint16_t year();

// get the current day of the week
uint8_t weekDay();

// get
uint16_t yearDay();

// Compared to Datime, uint32 t, DaySeconds
```

### Stamp
Stores date and time in UNIX, inherits`StampConvert`:

```cpp
uint32_t unix = 0;
```
```cpp
Stamp;
Stamp(const char* str);
Stamp(uint32_t unix);
Stamp(const Datime& dt);
Stamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
Stamp(uint16_t yh, uint16_t mm, uint16_t ds);
```
```cpp
// =========== SET TIME ============
// set time
void set(Datime dt);

// set the time (year, month, day, hour, minute, second)
void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

// Set the time (year, month, day) or (hour, minute, second)
void set(uint16_t yh, uint16_t mm, uint16_t ds);

// =========== ADD ============
// second
void addSeconds(uint32_t s);

// minutes
void addMinutes(uint32_t m);

// clock
void addHours(uint32_t h);

// days
void addDays(uint32_t d);

// =========== PARSE ============
// hh:mm:ss or yyyy-mm-dd or yyyy-mm-ddThh:mm: ss
bool parse(const char* s);

// <day_week>, dd <month> yyyy hh:mm:ss
bool parseHTTP(const char* s);

// =========== OVERLOAD ===========
// time in seconds
uint32_t getUnix() const;
```

### StampKeeper
After "synchronization" saves time on the base`millis()`. It has a ticker, you need to call it in.`loop()`. The signal once per second in 0 milliseconds of a new second, has a mechanism of stable time flow:

- Does not miss seconds with delays in the program - will be called several times in a row to "catch up" time
- If the synchronization time lags behind the real - the ticker will be called several times in a row for the entire period of desynchronization.
- If time is in a hurry during synchronization, it will not be called until real time catches up with the internal time.

> If you do not call the ticker in the program at all, the time will simply count from the moment of synchronization.

```cpp
// set unix and milliseconds
StampKeeper(uint32_t unix = 0, uint16_t ms = 0);
```
```cpp
// set unix and milliseconds
void sync(uint32_t unix, uint16_t ms = 0, bool skipTicks = false);

// synchronize
void sync(StampKeeper& keeper, bool skipTicks = false);

// synchronize
void sync(const Datime& dt, uint16_t ms = 0, bool skipTicks = false);

// cancel out
void reset();

// Missing seconds (call after sync)
void skipTicks();

// There is a desynchronization that needs to be "doped"
bool hasDesync();

// timing
bool synced();

// second-flag
bool newSecond();

// connect the function-processor of the new second (view void f())
void onSecond(SecondCallback handler);

// connect the synchronization processing function (view void f(uint32 t unix))
void onSync(SyncCallback cb);

// get current unix
uint32_t getUnix() const;

// get milliseconds of the current second
uint16_t ms();

// get milliseconds from epoch
uint64_t getUnixMs();

// ticker, call the loop. It'll bring true back in another second.
bool tick();
```

### VirtualRTC
Interface for other libraries

```cpp
virtual void setUnix(uint32_t unix) = 0;
virtual uint32_t getUnix() = 0;
```

<a id="example"></a>

## Examples
### StampKeeper
Some libraries use some`StampKeeper`To support synchronized time (GyverNTP, GyverDS3231, Settings...). Examples fit all of these libraries:

```cpp
StampKeeper rtc;  // GyverNTP, GyverDS3231, Settings.rtc

void setup() {
    Serial.begin(115200);

    // time zone, set to all actions with objects
    setStampZone(3);

    // second handler (called from ticker)
    rtc.onSecond([]() {
        Serial.println("new second!");
    });

    // Synchronization processor (called from sync)
    rtc.onSync([](uint32_t unix) {
        Serial.println("sync: ");
        Serial.print(unix);
    });

    // manual synchronization for example
    // rtc.sync(1738237474);
}
void loop() {
    // The ticker will return true every second in 0 ms of a second if the time is synchronized.
    if (rtc.tick()) {
        // lineage
        Serial.print(rtc.toString());  // rtc.timeToString(), rtc.dateToString()
        Serial.print(':');
        Serial.println(rtc.ms());  // + milliseconds of the current second. Inside, the tick is always 0.

        // deduction
        Datime dt = rtc;  // or Datime dt(rtc)
        Serial.println(dt.year);
        Serial.println(dt.second);
        Serial.println(dt.hour);
        dt.weekDay();
        dt.yearDay();
        // ... and other Datime methods and variables

        // Reading directly slower than output in Datime
        rtc.second();
        rtc.minute();
        rtc.year();
        // ... and other methods of StampConvert

        // comparison
        rtc == DaySeconds(12, 35, 0);            // Compared to DaySeconds (time is 12:35:00)
        rtc == 1738237474ul;                     // comparison with unix
        rtc == Datime(2025, 1, 30, 14, 14, 30);  // comparison
    }

    if (rtc.newSecond()) {
        // You can catch another second here.
    }
}
```

If you do not call the ticker, the time will be counted simply from the moment of synchronization and will always be correct:

```cpp
StampKeeper t;

void setup() {
    Serial.begin(115200);
    setStampZone(3);  // time-zone
    t.sync(1738237474);
}
void loop() {
    Serial.println(t.toString());
    delay(2000);
}
```

<a id="versions"></a>
## Versions
- v1.0
- V1.1 – Many new tools and capabilities have been added
- v1.2.2 - Optimization, refactoring, improved StampTicker
- v1.3.9 - Fixed critical bug in toString/dateToString/timeToString output
- v1.4.0 - added StampKeeper, combines StampTicker and StampSync. Lots of small improvements

<a id="install"></a>
## Installation
- The library can be found by the name **Stamp** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/Stamp/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="feedback"></a>
## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
