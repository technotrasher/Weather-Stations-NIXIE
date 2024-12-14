void calculateTime() {
  dotFlag = !dotFlag;
  if (dotFlag) {
    dotBrightFlag = true;
    dotBrightDirection = true;
    dotBrightCounter = 0;
    newTimeFlagsec = true;  // флаг секунды
    changeBright();         // изменить яркость согласно времени суток
    RTC_time.s++;
    if (RTC_time.s > 59) {
      newTimeFlag = true;  // флаг что нужно поменять время
      RTC_time.s = 0;
      RTC_time.m++;
      changeMins++;  // в режиме настройки менять минуты при переходе 59 сек.
      minsCount++;
      //changeBright();  // изменить яркость согласно времени суток раз в минуту
      //TimerFreeTone(PIEZO_PORT, 500, 100, 10); //тест каждую минуту
      if (minsCount >= 30) {  // каждые 30 мин
        minsCount = 0;
        getTime();  // синхронизация с RTC
      }
      if (RTC_time.m % BURN_PERIOD == 0 && ef_flag && !alm_flag && stat_mode == Clock) burnIndicators();  // чистим чистим!
    }
    if (RTC_time.m > 59) {
      RTC_time.m = 0;
      RTC_time.h++;
      if (RTC_time.h >= 8 && RTC_time.h <= 22) TimerFreeTone(PIEZO_PORT, 1000, 150, 10);  // сигнал каждый час (пин, частота, длительность, громкость)
      if (RTC_time.h > 23) RTC_time.h = 0;
    }
    if (newTimeFlagsec) setNewTime();  // обновляем массив времени для эффектов
                                       // ---------------------------------------------------------------- для эффекта секунды
    for (uint8_t i = 4; i < 6; i++) {
      if (indiDigits[i] != newTime[i]) flipIndics[i] = true;
      else flipIndics[i] = false;
    }
    // ----------------------------------------------------------------
    if (newTimeFlagsec && stat_mode == Clock) sendTimeSec(RTC_time.s);  // отправляем секунды

    // не дает запускаться эффектам и датчикам при переходе через минуту
    if (RTC_time.s > 53 || RTC_time.s < 7) {
      ef_flag = false;  // флаг для эффектов
    } else {
      ef_flag = true;
    }

    //Serial.println(String(ef_flag) + " ef_flag");
    //Serial.println(analogRead(ALARM_PIN));
    //Serial.println(String(ZoneCounter) + " ZoneCounter");
    //Serial.println(String(alm_flag) + " alm_flag");

    // будильник
    if (alm_flag) {
      if (almTimer.tick() || !readBtn()) {  // если тумблером отключили или таймер закончил работу.
        alm_flag = false;
      }
      // мигать на будильнике светом и звуком
      if (alarm == true) {
        setLedBright(250);
        setLedHue(0, WHITE_ON);
        alarm = false;
       } else {
        alarm = true;
        //setLedHue(100, WHITE_ON);
        setLedBright(0);
      }
      showLeds();  //отрисовка светодиодов
      TimerFreeTone(PIEZO_PORT, FREQ, 500, 10);
    } else if (readBtn() && alm_mins == RTC_time.m && alm_hrs == RTC_time.h) {  // срабатывает на 3 условия.
      alm_flag = true;
      almTimer.start();
    }


    // переход зимнее летнее время
    if (!SHOW_ESP && RTC_time.MM == 3 && RTC_time.DD >= 25 && RTC_time.DW == 7 && RTC_time.h == 4 && RTC_time.m == 0 && ZoneCounter == false) {
      ZoneCounter = true;
      RTC_time.h = RTC_time.h + 1;
      sendTimeH();
    }
    if (!SHOW_ESP && RTC_time.MM == 10 && RTC_time.DD >= 25 && RTC_time.DW == 7 && RTC_time.h == 4 && RTC_time.m == 0 && ZoneCounter == false) {
      ZoneCounter = true;
      RTC_time.h = RTC_time.h - 1;
      //Serial.println(String(RTC_time.h) + " RTC_time.h");
      //Serial.println(String(RTC_time.h) + " RTC_time.h");
      sendTimeH();
    }
    if (RTC_time.h == 12 && ZoneCounter == true) {
      ZoneCounter = false;
    }
  }
}

// Будильник -  опрос кнопки
boolean readBtn() {
  static boolean state;     //состояние тумблера
  static uint32_t oldTime;  //предыдущее время
  //Serial.println(String(state) + " state");
  if ((analogRead(ALARM_PIN) < 700) != state) {     //если состояние тумблера изменилось < 700 - включен 1 выключен 0
    if ((oldTime + 50) < millis()) state = !state;  //если прошло 50 мс и состояние более не изменялось то меняем значение тумблера
  } else oldTime = millis();                        //иначе запоминаем текущее время
  return state;                                     //возвращаем состояние тумблера
}