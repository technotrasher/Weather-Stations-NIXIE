void setup() {
  //---- подключение ESP -------------------------------------------------------
  //Serial.begin(115200);  // Терминал
  ESPport.begin(115200);  // ESP8266
  //----------------------------------------------------------------------------
  // случайное зерно для генератора случайных чисел
  //randomSeed(analogRead(A6) + analogRead(A7));
  randomSeed(RTC_time.s * (RTC_time.m + RTC_time.h) + RTC_time.DD * RTC_time.MM);
  //Serial.println(analogRead(A6));
  //Serial.println(analogRead(A7));

  // настройка пинов на выход
  pinMode(DECODER0, OUTPUT);
  pinMode(DECODER1, OUTPUT);
  pinMode(DECODER2, OUTPUT);
  pinMode(DECODER3, OUTPUT);
  pinMode(KEY0, OUTPUT);
  pinMode(KEY1, OUTPUT);
  pinMode(KEY2, OUTPUT);
  pinMode(KEY3, OUTPUT);
  pinMode(KEY4, OUTPUT);
  pinMode(KEY5, OUTPUT);
  pinMode(GEN, OUTPUT);
  pinMode(DOT, OUTPUT);
  pinMode(ALARM_PIN, INPUT);
  pinMode(PWMPIN_CO2, INPUT);
  pinMode(BTN, INPUT);

  // задаем частоту ШИМ на 9 и 10 выводах 31 кГц
  TCCR1B = TCCR1B & 0b11111000 | 1;  // ставим делитель 1

  wireInit();  //инициализация шины wire
  BACKL_INIT;  //инициализация подсветки
  //OCR1B = 255; // тест точек полное включение

  TIMSK1 = 0;  //отключаем прерывания Timer 1
  // задаем частоту ШИМ на D9 и D10 выводах 31 кГц Timer 1
  TCCR1B = (1 << CS10);                               //делитель 1 CLK/1
  OCR1A = DUTY;                                       //задаем значение регистра сравнения на D9. достигло - сбросилось в 0
  TCCR1A = (1 << COM1A1 | 1 << COM1B1 | 1 << WGM10);  //подключаем D9(COM1A1) и D10(COM1B1), WGM10 - режим коррекции фазы ШИМ

  // задаем частоту ШИМ на пинах D3 и D11 на 7.8 кГц и разрешаем прерывания COMPA Timer 2
  TCCR2B = (1 << CS21);                              // делитель 8 CLK/8
  TCCR2A = (1 << COM2A1 | 1 << WGM20 | 1 << WGM21);  //подключаем D11 (COM2A1), Режим CTC (сброс по совпадению) - WGM21, WGM20 - режим коррекции фазы ШИМ
  TIMSK2 |= (1 << OCIE2A);                           // включить прерывания по совпадению COMPA

  // EEPROM
  if (EEPROM_ReadByte(16) != 25) {  // первый запуск
    EEPROM_UpdateByte(16, 25);
    EEPROM_UpdateByte(0, FLIP_EFFECT); // режим эффектов
    EEPROM_UpdateByte(1, BACKL_MODE); // режим подсветки
    EEPROM_UpdateByte(2, SHOW_ESP);  // отключить получение времени из интернета
    EEPROM_UpdateByte(3, 0);         // esptime = false первый запуск перевод зима лето вручную
    EEPROM_UpdateByte(4, SHOW_SEC);  // отключение эффкта секунд
    EEPROM_UpdateByte(8, 0);         // часы будильника
    EEPROM_UpdateByte(12, 0);        // минуты будильника
  }
  FLIP_EFFECT = EEPROM_ReadByte(0);
  BACKL_MODE = EEPROM_ReadByte(1);
  SHOW_ESP = EEPROM_ReadByte(2);
  //esptime = EEPROM_ReadByte(3);
  SHOW_SEC = EEPROM_ReadByte(4);
  alm_hrs = EEPROM_ReadByte(8);
  alm_mins = EEPROM_ReadByte(12);

  // установка времени вручную
  //RTC_time.s = 0; //секунда
  //RTC_time.m = 06; //минута
  //RTC_time.h = 23;  //час
  //TC_time.MM = 07; //месяц
  //RTC_time.DD = 13; //день
  //RTC_time.YY = 2024; // год
  //sendTime();
  // - после отправки закоментировать и прошить.

  //кнопки
  //btnSet.setType(LOW_PULL);
  btnSet.setDebounce(180);        // настройка антидребезга (по умолчанию 80 мс)
  //btnSet.setTimeout(600);        // настройка таймаута на удержание (по умолчанию 500 мс)
  //btnSet.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)

  btnUp.setDebounce(100);        // настройка антидребезга (по умолчанию 80 мс)
  //btnUp.setTimeout(500);        // настройка таймаута на удержание (по умолчанию 500 мс)
  //btnUp.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)

  btnDwn.setDebounce(200);        // настройка антидребезга (по умолчанию 80 мс)
  //btnDwn.setTimeout(500);        // настройка таймаута на удержание (по умолчанию 500 мс)
  //btnDwn.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)


  getTime();  // синхронизация времени
  // установить яркость подсветки и включить ее
  indiBrightCounter = indiMaxBright;  // для эффектов
  changeBright();                     // изменить яркость согласно времени суток и установить яркость на индикаторы
  TestTimer_Event();                  // протестировть индикаторы

  sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);  // отправить время на индикаторы

  // расчёт шага яркости точки
  dotBrightStep = ceil((float)dotMaxBright * 2 / DOT_TIME * DOT_TIMER);
  if (dotBrightStep == 0) dotBrightStep = 1;
}