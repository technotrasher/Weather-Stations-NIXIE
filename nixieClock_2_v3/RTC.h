#define RTC_ADDR 0x68  //адрес RTC DS3231

struct time {  //структура времени
  uint8_t s = 0;
  uint8_t m = 0;
  uint8_t h = 22;
  uint8_t DD = 1;
  uint8_t MM = 1;
  uint16_t YY = 2022;
  uint8_t DW = 5;
} RTC_time;

const uint8_t daysInMonth[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };  //дней в месяце

//--------------------------------------Получить день недели------------------------------------------
uint8_t getWeekDay(uint16_t YY, uint8_t MM, uint8_t DD)  //получить день недели
{
  if (YY >= 2000) YY -= 2000;                                                   //если год больше 2000
  uint16_t days = DD;                                                           //записываем дату
  for (uint8_t i = 1; i < MM; i++) days += pgm_read_byte(&daysInMonth[i - 1]);  //записываем сколько дней прошло до текущего месяца
  if (MM > 2 && YY % 4 == 0) days++;                                            //если високосный год, прибавляем день
  return (days + 365 * YY + (YY + 3) / 4 - 2 + 6) % 7 + 1;                      //возвращаем день недели
}
//-------------------------------Максимальное количество дней--------------------------------------------------
uint8_t maxDays(void)  //максимальное количество дней
{
  return ((RTC_time.MM == 2 && !(RTC_time.YY % 4)) ? 1 : 0) + pgm_read_byte(&daysInMonth[RTC_time.MM - 1]);  //возвращаем количество дней в месяце
}
//--------------------------------------Распаковка short------------------------------------------
uint8_t unpackREG(uint8_t data)  //распаковка short
{
  return ((data >> 4) * 10 + (data & 0xF));  //возвращаем результат
}
//--------------------------------------Распаковка часов------------------------------------------
uint8_t unpackHours(uint8_t data)  //распаковка часов
{
  if (data & 0x20) return ((data & 0x0F) + 20);  //возвращаем результат
  else if (data & 0x10) return ((data & 0x0F) + 10);
  else return (data & 0x0F);
}
//-------------------------------Чтение коррекции хода-------------------------------------
uint8_t readAgingRTC(void)  //чтение коррекции хода
{
  if (wireRequestFrom(RTC_ADDR, 0x10)) return 0;  //запрашиваем чтение данных, если нет ответа выходим
  return wireReadEndByte();                       //возвращаем результат
}
//-------------------------------Запись коррекции хода-------------------------------------
void writeAgingRTC(uint8_t data)  //запись коррекции хода
{
  if (wireBeginTransmission(RTC_ADDR)) return;  //начало передачи
  wireWrite(0x10);                              //устанавливаем адрес записи
  wireWrite(data);                              //записываем коррекцию хода
  wireEnd();                                    //остановка шины wire
}

//--------------------------------------Отправить время в RTC------------------------------------------
void sendTime(void) //отправить время в RTC
{
  if (wireBeginTransmission(RTC_ADDR)); //устанавливаем ошибку модуля RTC
  else { //иначе отправляем данные
    RTC_time.DW = getWeekDay(RTC_time.YY,RTC_time.MM, RTC_time.DD); //получаем день недели
    wireWrite(0x00); //устанавливаем адрес записи
    wireWrite((((RTC_time.s / 10) << 4) | (RTC_time.s % 10))); //отправляем секунды
    wireWrite((((RTC_time.m / 10) << 4) | (RTC_time.m % 10))); //отправляем минуты
    if (RTC_time.h > 19) wireWrite((0x02 << 4) | (RTC_time.h % 20)); //отправляем часы
    else if (RTC_time.h > 9) wireWrite((0x01 << 4) | (RTC_time.h % 10));
    else wireWrite(RTC_time.h);
    wireWrite(0); //пропускаем день недели
    wireWrite(((RTC_time.DD / 10) << 4) | (RTC_time.DD % 10)); //отправляем дату
    wireWrite(((RTC_time.MM / 10) << 4) | (RTC_time.MM % 10)); //отправляем месяц
    wireWrite((((RTC_time.YY - 2000) / 10) << 4) | ((RTC_time.YY - 2000) % 10)); //отправляем год
    wireEnd(); //конец передачи
  }
}

void sendTimeH(void)  //отправить время в RTC только часы
{
  wireBeginTransmission(RTC_ADDR);                                  //начало передачи
  wireWrite(0x02);                                                  //устанавливаем адрес записи
  if (RTC_time.h > 19) wireWrite((0x02 << 4) | (RTC_time.h % 20));  //отправляем часы
  else if (RTC_time.h > 9) wireWrite((0x01 << 4) | (RTC_time.h % 10));
  else wireWrite(RTC_time.h);
  wireEnd();  //конец передачи
}

void sendTimeS(void)  //отправить время в RTC только секунды
{
  wireBeginTransmission(RTC_ADDR);                            //начало передачи
  wireWrite(0x00);                                            //устанавливаем адрес записи
  wireWrite((((RTC_time.s / 10) << 4) | (RTC_time.s % 10)));  //отправляем минуты
  wireEnd();                                                  //конец передачи
}

void sendTimeM(void)  //отправить время в RTC только минуты
{
  wireBeginTransmission(RTC_ADDR);                            //начало передачи
  wireWrite(0x01);                                            //устанавливаем адрес записи
  wireWrite((((RTC_time.m / 10) << 4) | (RTC_time.m % 10)));  //отправляем минуты
  wireEnd();                                                  //конец передачи
}

void sendTimeD(void)  //отправить время в RTC только дни
{
  wireBeginTransmission(RTC_ADDR);                            //начало передачи
  wireWrite(0x04);                                            //устанавливаем адрес записи
  wireWrite(((RTC_time.DD / 10) << 4) | (RTC_time.DD % 10));  //отправляем дату
  wireEnd();                                                  //конец передачи
}

void sendTimeMH(void)  //отправить время в RTC только месяц
{
  wireBeginTransmission(RTC_ADDR);                            //начало передачи
  wireWrite(0x05);                                            //устанавливаем адрес записи
  wireWrite(((RTC_time.MM / 10) << 4) | (RTC_time.MM % 10));  //отправляем месяц
  wireEnd();                                                  //конец передачи
}

void sendTimeY(void)  //отправить время в RTC только год
{
  wireBeginTransmission(RTC_ADDR);                                              //начало передачи
  wireWrite(0x06);                                                              //устанавливаем адрес записи
  wireWrite((((RTC_time.YY - 2000) / 10) << 4) | ((RTC_time.YY - 2000) % 10));  //отправляем год
  wireEnd();                                                                    //конец передачи
}

//--------------------------------------Запрашиваем время из RTC------------------------------------------
boolean getTime(void)  //запрашиваем время из RTC
{
  if (wireRequestFrom(RTC_ADDR, 0x00)) {  //запрашиваем чтение данных, если нет ответа выходим
    return 1;                             //выходим
  }
  RTC_time.s = unpackREG(wireRead());                               //получаем секунды
  RTC_time.m = unpackREG(wireRead());                               //получаем минуты
  RTC_time.h = unpackHours(wireRead());                             //получаем часы
  wireRead();                                                       //пропускаем день недели
  RTC_time.DD = unpackREG(wireRead());                              //получаем дату
  RTC_time.MM = unpackREG(wireRead());                              //получаем месяц
  RTC_time.YY = unpackREG(wireReadEndByte()) + 2000;                //получаем год
  RTC_time.DW = getWeekDay(RTC_time.YY, RTC_time.MM, RTC_time.DD);  //получаем день недели
  return 0;                                                         //выходим
}

//-----------------------------------Настройка SQW-----------------------------------------
void setSQW(void)  //настройка SQW
{
  if (wireRequestFrom(RTC_ADDR, 0x0E)) {  //запрашиваем чтение данных, если нет ответа то
    return;                               //выходим
  }
  uint8_t ctrlReg = wireReadEndByte() & 0x20;  //выключаем INTCON и устанавливаем частоту 1Гц

  if (wireBeginTransmission(RTC_ADDR));  //устанавливаем ошибку модуля RTC
  else {                                                         //иначе отправляем данные
    wireWrite(0x0E);                                             //устанавливаем адрес записи
    wireWrite(ctrlReg);                                          //отправляем настройку SQW
    wireEnd();                                                   //конец передачи
  }
}

//-------------------------------Чтение температуры-------------------------------------
boolean readTempRTC(void)  //чтение температуры
{
  if (wireRequestFrom(RTC_ADDR, 0x11)) return 0;  //запрашиваем чтение данных, если нет ответа выходим
  uint16_t temp = (((uint16_t)wireRead() << 2 | wireReadEndByte() >> 6) * 100) >> 2;
  tempSens.temp = (temp > 8500) ? 0 : temp;
  tempSens.press = 0;  //сбросили давление
  tempSens.hum = 0;    //сбросили влажность
  return 1;            //выходим
}