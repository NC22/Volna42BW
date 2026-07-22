[![latest](https://img.shields.io/github/v/release/GyverLibs/Stamp.svg?color=brightgreen)](https://github.com/GyverLibs/Stamp/releases/latest/download/Stamp.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Stamp.svg)](https://registry.platformio.org/libraries/gyverlibs/Stamp)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Stamp?_x_tr_sl=ru&_x_tr_tl=en)

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Stamp
Библиотека для хранения и манипуляции со временем
- Более удобная в использовании, чем встроенная time.h
- Используются более быстрые алгоритмы преобразования, чем в time.h
- Парсинг из строк
- Может считать и поддерживать время на базе millis()
- Работает ~до 2106 года

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

## Содержание
- [Документация](#reference)
- [Пример](#example)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="reference"></a>

## Документация
### Часовой пояс
Настраивается глобально для всех инструментов библиотеки

```cpp
void setStampZone(int zone);    // установить глобальную часовую зону в часах или минутах
int getStampZone();             // получить глобальную часовую зону в минутах
```

### Утилиты
Набор функций для работы со временем и датой

```cpp
// время в секунды
uint32_t StampUtils::timeToSeconds(uint8_t hours, uint8_t minutes, uint8_t seconds);

// високосный год
bool StampUtils::isLeap(uint16_t year);

// дней в месяце без учёта года (февраль 28)
uint8_t StampUtils::daysInMonth(uint8_t month);

// дней в месяце с учётом високосного года
uint8_t StampUtils::daysInMonth(uint8_t month, uint16_t year);

// дней года к месяцу (янв 0, фев 31, март 59/60...)
uint16_t StampUtils::daysToMonth(uint8_t month, uint16_t year);

// дата в день текущего года (начиная с 1)
uint16_t StampUtils::dateToYearDay(uint8_t day, uint8_t month, uint16_t year);

// дата в день недели (пн 1.. вс 7)
uint8_t StampUtils::dateToWeekDay(uint8_t day, uint8_t month, uint16_t year);

// дата в количество дней с 01.01.2000 (начиная с 0)
uint16_t StampUtils::dateToDays2000(uint8_t day, uint8_t month, uint16_t year);

// дата в unix время, zone в минутах
uint32_t StampUtils::dateToUnix(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t seconds, int16_t zone = 0);
```

### Макросы
```cpp
// время и дата компиляции
__TIME_SEC__
__TIME_MIN__
__TIME_HOUR__
__DATE_DAY__
__DATE_MONTH__
__DATE_YEAR__

// unix с часовой зоной компьютера
__TIME_UNIX__
```

### DaySeconds
Хранит время в секундах с начала текущих суток
```cpp
DaySeconds(uint8_t hour, uint8_t minute, uint8_t second);

void set(uint8_t hour, uint8_t minute, uint8_t second);

uint32_t seconds = 0;
```

### Datime
Хранит локальные дату и время в отдельных переменных:

```cpp
uint16_t year;      // год
uint8_t month;      // месяц (1.. 12)
uint8_t day;        // день месяца (1.. 28-31)
uint8_t hour;       // час (0.. 23)
uint8_t minute;     // минута (0.. 59)
uint8_t second;     // секунда (0.. 59)
uint8_t weekDay();    // день недели (1 пн.. 7 вс) ISO 8601
uint16_t yearDay();   // день года (1.. 365-366)
```

> Указанные диапазоны справедливы для всех остальных функций библиотеки!

```cpp
Datime() {}
Datime(const char* str);
Datime(uint32_t unix);
Datime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
Datime(uint16_t yh, uint16_t mm, uint16_t ds);
```
```cpp
// ============= SET =============
// установить время (год, месяц, день, час, минута, секунда)
void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

// установить время (год, месяц, день) или (час, минута, секунда)
void set(uint16_t yh, uint16_t mm, uint16_t ds);

// установить из unix времени и глобального часового пояса setStampZone
void set(uint32_t unix);

// =========== EXPORT ============
// вывести в секунды с начала текущих суток
uint32_t daySeconds();

// вывести в unix-секунды
uint32_t getUnix() const;

// ========== TO STRING ==========
// вывести дату в формате "dd.mm.yyyy" [11]. Вернёт указатель на конец строки
char* dateToChar(char* buf);

// вывести дату в формате "dd.mm.yyyy"
String dateToString();

// вывести время в формате "hh:mm:ss" [9]. Вернёт указатель на конец строки
char* timeToChar(char* buf);

// вывести время в формате "hh:mm:ss"
String timeToString();

// вывести в формате "dd.mm.yyyy hh:mm:ss" [20]. Вернёт указатель на конец строки
char* toChar(char* buf, char div = ' ');

// вывести в формате "dd.mm.yyyy hh:mm:ss"
String toString(char div = ' ');

// ========== PARSE ==========
// yyyy-mm-dd
bool parseDate(const char* s);

// hh:mm:ss
bool parseTime(const char* s);

// hh:mm:ss или yyyy-mm-dd или yyyy-mm-ddThh:mm:ss
bool parse(const char* s);

// <day_week>, dd <month> yyyy hh:mm:ss
bool parseHTTP(const char* s);

// =========== COMPARE ===========
// сравнивается с Datime, uint32_t, DaySeconds

// одинаковое время
bool equals(const Datime& dt);

// високосный ли год
bool isLeap();

// дата и время корректны
bool valid();

// дата 01.01.2000
bool isY2K();

// день года как индекс массива от 0 до 365 независимо от високосного года. 29 февраля имеет индекс 59
uint16_t dayIndex();

// ============= ADD =============
// добавить секунды
void addSeconds(uint32_t s);

// добавить минуты
void addMinutes(uint32_t m);

// добавить часы
void addHours(uint32_t h);

// добавить дни
void addDays(uint32_t d);

// ============= NEXT =============
// следующая секунда
void nextSecond();

// следующая минута (xx:xx:00)
void nextMinute();

// следующий час (xx:00:00)
void nextHour();

// следующий день (00:00:00)
void nextDay();

// следующий месяц (1 число 00:00:00)
void nextMonth();
```
> Все добавления времени в Datime выполняются напрямую, без конвертации в unix, т.е. довольно быстро

```cpp
// ============= АЛГОРИТМ =============
// Алгоритм преобразования времени задаётся перед подключением библиотеки
// для исследования и просто так чтобы было
#define UNIX_ALG UNIX_ALG_0       // ~402us и ~94B Flash (AVR)
#define UNIX_ALG UNIX_ALG_1       // ~298us и ~138B Flash (AVR)
#define UNIX_ALG UNIX_ALG_2       // ~216us и ~584B Flash (AVR)
#define UNIX_ALG UNIX_ALG_3       // ~297us и ~178B Flash (AVR)
#define UNIX_ALG UNIX_ALG_TIME_T  // ~246us и ~842B Flash (AVR)
```

### StampConvert
Конвертер для других классов

```cpp
virtual uint32_t getUnix() const;

// =========== GET TIME ===========
// экспортировать в локальное время Datime
Datime now();

operator Datime();

// ============ TO STRING ============
// вывести дату в формате "dd.mm.yyyy"
char* dateToChar(char* buf);
String dateToString();

// вывести дату в формате "yyyy-mm-dd"
char* dateToCharISO(char* buf);
String dateToStringISO();

// вывести время в формате "hh:mm:ss"
char* timeToChar(char* buf);
String timeToString();

// вывести время и дату в формате "dd.mm.yyyy hh:mm:ss"
char* toChar(char* buf, char div = ' ');
String toString(char div = ' ');

// вывести в формате "yyyy-mm-dd hh:mm:ss"
char* toCharISO(char* buf, char div = ' ');
String toStringISO(char div = ' ');

// ============ EXPORT EPOCH ============

// получить секунды с epoch
uint32_t toSeconds();

// получить минуты с epoch
uint32_t toMinutes();

// получить часы с epoch
uint32_t toHours();

// получить сутки с epoch
uint32_t toDays();

// ============ DATIME ============

// получить секунды с начала текущих суток (локальное время)
uint32_t daySeconds();

// получить текущие секунды
uint8_t second();

// получить текущие минуты
uint8_t minute();

// получить текущие часы
uint8_t hour();

// получить текущий день месяца
uint8_t day();

// получить текущий месяц
uint8_t month();

// получить текущий год
uint16_t year();

// получить текущий день недели
uint8_t weekDay();

// получить текущий день года
uint16_t yearDay();

// сравнивается с Datime, uint32_t, DaySeconds
```

### Stamp
Хранит дату и время в UNIX, наследует `StampConvert`:

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
// установить время из Datime
void set(Datime dt);

// установить время (год, месяц, день, час, минута, секунда)
void set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

// установить время (год, месяц, день) или (час, минута, секунда)
void set(uint16_t yh, uint16_t mm, uint16_t ds);

// =========== ADD ============
// добавить секунды
void addSeconds(uint32_t s);

// добавить минуты
void addMinutes(uint32_t m);

// добавить часы
void addHours(uint32_t h);

// добавить дни
void addDays(uint32_t d);

// =========== PARSE ============
// hh:mm:ss или yyyy-mm-dd или yyyy-mm-ddThh:mm:ss
bool parse(const char* s);

// <day_week>, dd <month> yyyy hh:mm:ss
bool parseHTTP(const char* s);

// =========== OVERLOAD ===========
// получить время в секундах
uint32_t getUnix() const;
```

### StampKeeper
После "синхронизации" сохраняет счёт времени на базе `millis()`. Имеет тикер, нужно вызывать его в `loop()`. Сигналит раз в секунду в 0 миллисекунд новой секунды, имеет **механизм стабильного хода** времени:

- Не пропускает секунды при задержках в программе - будет вызван несколько раз подряд, чтобы "догнать" время
- Если при синхронизации время отстаёт от реального - тикер будет вызван несколько раз подряд на весь период рассинхронизации
- Если при синхронизации время спешит - не будет вызываться, пока реальное время не догонит внутреннее

> Если вообще не вызывать тикер в программе - время будет просто считаться с момента синхронизации

```cpp
// установить unix и миллисекунды
StampKeeper(uint32_t unix = 0, uint16_t ms = 0);
```
```cpp
// установить unix и миллисекунды
void sync(uint32_t unix, uint16_t ms = 0, bool skipTicks = false);

// синхронизировать с другим кипером
void sync(StampKeeper& keeper, bool skipTicks = false);

// синхронизировать с Datime
void sync(const Datime& dt, uint16_t ms = 0, bool skipTicks = false);

// сбросить синхронизацию
void reset();

// пропустить отставшие секунды (вызывать после sync)
void skipTicks();

// есть рассинхронизация, которую нужно "дотикать"
bool hasDesync();

// время синхронизировано
bool synced();

// секундный флаг
bool newSecond();

// подключить функцию-обработчик новой секунды (вида void f())
void onSecond(SecondCallback handler);

// подключить функцию-обработчик синхронизации (вида void f(uint32_t unix))
void onSync(SyncCallback cb);

// получить текущий unix
uint32_t getUnix() const;

// получить миллисекунды текущей секунды
uint16_t ms();

// получить миллисекунды с epoch
uint64_t getUnixMs();

// тикер, вызывать в loop. Вернёт true на новой секунде
bool tick();
```

### VirtualRTC
Интерфейс для других библиотек

```cpp
virtual void setUnix(uint32_t unix) = 0;
virtual uint32_t getUnix() = 0;
```

<a id="example"></a>

## Примеры
### StampKeeper
Некоторые библиотеки используют `StampKeeper` для поддержки синхронизированного времени (GyverNTP, GyverDS3231, Settings...). Примеры подходят под все эти библиотеки:

```cpp
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
```

Если не вызывать тикер - время будет считаться просто с момента синхронизации и всегда будет верным:

```cpp
StampKeeper t;

void setup() {
    Serial.begin(115200);
    setStampZone(3);  // часовой пояс
    t.sync(1738237474);
}
void loop() {
    Serial.println(t.toString());
    delay(2000);
}
```

<a id="versions"></a>
## Версии
- v1.0
- v1.1 - добавлено много новых инструментов и возможностей
- v1.2.2 - оптимизация, рефакторинг, улучшен StampTicker
- v1.3.9 - исправлен критический баг в выводе toString/dateToString/timeToString
- v1.4.0 - добавлен StampKeeper, объединяет StampTicker и StampSync. Куча мелких улучшений

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **Stamp** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/Stamp/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
