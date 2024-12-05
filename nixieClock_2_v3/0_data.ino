HardwareSerial& ESPport = Serial;  //esp порт

//----------------Температура--------------
// переменные для работы библиотек.
struct sensorData {
  uint16_t temp;   //температура
  uint16_t press;  //давление
  uint8_t hum;     //влажность
  uint8_t type;    //тип датчика температуры
  boolean init;    //флаг инициализации порта
  boolean err;     //ошибка сенсора
} tempSens;

// библиотеки
// адрес BME280 задан в файле библиотеки
// проверить.
#include <TimerFreeTone.h>
#include "wire.h"
#include "RTC.h"
#include "BME.h"
#include "DS.h"
#include "WS.h"
#include <GyverButton.h>
#include "TimerMs.h"
#include "EEPROM.h"
#include "ESP.h"

// кнопки
GButton btnSet(BTN);
GButton btnUp(BTN);
GButton btnDwn(BTN);

// таймеры
TimerMs almTimer((uint32_t)ALM_TIMEOUT * 1000, 1);                       // таймер работы будильника 30сек если ALM_TIMEOUT=30
TimerMs dotTimer(500, 1);                                                // полсекундный таймер для часов 0.5 секунды 500 миллисекунд
TimerMs dotBrightTimer(DOT_TIMER, 1);                                    // таймер шага яркости точки
TimerMs flipTimer(FLIP_SPEED[FLIP_EFFECT], 1);                           // таймер эффектов
TimerMs glitchTimer(random(GLITCH_MIN * 1000L, GLITCH_MAX * 1000L), 1);  // таймер для глюков
TimerMs blinkTimer(500, 1);                                              // таймер мигания цифры в настройках
TimerMs trainTimer(random(train_MIN * 1000L, train_MAX * 1000L), 1);     // и для поезда
TimerMs testtimer(random(test_MIN * 1000L, test_MAX * 1000L), 1);        // и для перемотки индикаторов
TimerMs modeTimer((uint32_t)CLOCK_TIME * 1000, 1);                       // таймер для отображения температуры и влажности
TimerMs modeTimerP((uint32_t)TEMP_PRES * 1000, 1);                       // таймер давления
TimerMs modeTimerCO((uint32_t)TEMP_CO2 * 1000, 1);                       //таймер отобажения СО2
TimerMs ESPTimer((uint32_t)ESP_TIME * 1000, 1);                          // таймер опроса сервера точного времени 36000*1000 c - 10 часов
TimerMs effectTimer(100, 1);                                             // таймер эффектов подсветки, 100 для старта
TimerMs flipTimersec(12, 1);                                            // таймер для эффекта секунды 5-150 23-12



// переменные
volatile int8_t indiDimm[6];     // величина диммирования (0-24)
volatile int8_t indiCounter[6];  // счётчик каждого индикатора (0-24)
volatile int8_t indiDigits[6];   // цифры, которые должны показать индикаторы (0-9)
volatile int8_t curIndi;         // текущий индикатор (0-5)
bool flipIndics[6];
uint8_t newTime[6];
uint8_t startCathode[4], endCathode[4];  // только для эффектов

bool dotFlag, TrainFlag = true, TestFlag = true;  //!
int8_t alm_hrs, alm_mins;
volatile uint8_t stat_mode = Clock;  // 0 часы, 1 температура, 2 настройка будильника, 3 настройка часов, 4 дата и т.д.
bool changeFlag, blinkFlag;
uint8_t indiMaxBright = INDI_BRIGHT, dotMaxBright = DOT_BRIGHT, backlMaxBright = BACKL_BRIGHT;
bool alm_flag = false, alarm; // флаги для будильника
bool dotBrightFlag, dotBrightDirection, backlBrightFlag, backlBrightDirection, indiBrightDirection, indiBrightDirections;
int16_t dotBrightCounter, backlBrightCounter, indiBrightCounter, indiBrightCounters;
uint8_t dotBrightStep;
bool newTimeFlag, newTimeFlagsec;
bool flipInit;
uint8_t glitchCounter, glitchMax, glitchIndic;
bool glitchFlag, indiState;
volatile uint8_t curMode = 0;
bool currentDigit = false;
int8_t changeHrs, changeMins, changedays, changemnth, changeyers;
bool lampState = false;
bool anodeStates[] = { 1, 1, 1, 1, 1, 1 };
uint8_t currentLamp, flipEffectStages, testIndicator;
bool trainLeaving, ef_flag;
bool ZoneCounter = false;  // для "перевода часов"
bool tempFlag = true;      // флаг для работа датчиков DS BME одновременно
uint16_t ppm = 0;
uint8_t flipCounter;
uint8_t minsCount = 0;

const uint8_t CRTgamma[256] PROGMEM = {
  0,    0,    1,    1,    1,    1,    1,    1,
  1,    1,    1,    1,    1,    1,    1,    1,
  2,    2,    2,    2,    2,    2,    2,    2,
  3,    3,    3,    3,    3,    3,    4,    4,
  4,    4,    4,    5,    5,    5,    5,    6,
  6,    6,    7,    7,    7,    8,    8,    8,
  9,    9,    9,    10,   10,   10,   11,   11,
  12,   12,   12,   13,   13,   14,   14,   15,
  15,   16,   16,   17,   17,   18,   18,   19,
  19,   20,   20,   21,   22,   22,   23,   23,
  24,   25,   25,   26,   26,   27,   28,   28,
  29,   30,   30,   31,   32,   33,   33,   34,
  35,   35,   36,   37,   38,   39,   39,   40,
  41,   42,   43,   43,   44,   45,   46,   47,
  48,   49,   49,   50,   51,   52,   53,   54,
  55,   56,   57,   58,   59,   60,   61,   62,
  63,   64,   65,   66,   67,   68,   69,   70,
  71,   72,   73,   74,   75,   76,   77,   79,
  80,   81,   82,   83,   84,   85,   87,   88,
  89,   90,   91,   93,   94,   95,   96,   98,
  99,   100,  101,  103,  104,  105,  107,  108,
  109,  110,  112,  113,  115,  116,  117,  119,
  120,  121,  123,  124,  126,  127,  129,  130,
  131,  133,  134,  136,  137,  139,  140,  142,
  143,  145,  146,  148,  149,  151,  153,  154,
  156,  157,  159,  161,  162,  164,  165,  167,
  169,  170,  172,  174,  175,  177,  179,  180,
  182,  184,  186,  187,  189,  191,  193,  194,
  196,  198,  200,  202,  203,  205,  207,  209,
  211,  213,  214,  216,  218,  220,  222,  224,
  226,  228,  230,  232,  233,  235,  237,  239,
  241,  243,  245,  247,  249,  251,  253,  255,
};

uint8_t getPWM_CRT(uint8_t val) {
  return pgm_read_byte(&(CRTgamma[val]));
}

// быстрый digitalWrite
void setPin(uint8_t pin, uint8_t x) {
  switch (pin) {  // откл pwm
    case 3:       // 2B
      bitClear(TCCR2A, COM2B1);
      break;
    case 5:  // 0B
      bitClear(TCCR0A, COM0B1);
      break;
    case 6:  // 0A
      bitClear(TCCR0A, COM0A1);
      break;
    case 9:  // 1A
      bitClear(TCCR1A, COM1A1);
      break;
    case 10:  // 1B
      bitClear(TCCR1A, COM1B1);
      break;
    case 11:  // 2A
      bitClear(TCCR2A, COM2A1);
      break;
  }

  if (pin < 8) bitWrite(PORTD, pin, x);
  else if (pin < 14) bitWrite(PORTB, (pin - 8), x);
  else if (pin < 20) bitWrite(PORTC, (pin - 14), x);
  else return;
}

void (*resetFunc)(void) = 0;  //перезагрузка