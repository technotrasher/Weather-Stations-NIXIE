// читаем датчики температура влажность
void stat_modeTick() {
  if (modeTimer.tick()) {
    if (stat_mode == Clock) {
      stat_mode = Temperature;
      if (SHOW_TEMP_HUM && !SHOW_TEMP_DS) readTempBME();  // один датчик BME
      if (SHOW_TEMP_DS && !SHOW_TEMP_HUM) readTempDS();   // один датчик DS
      if (SHOW_TEMP_HUM && SHOW_TEMP_DS) {                // два датчика включены одновременно
        if (tempFlag) {
          readTempBME();
        } else {
          readTempDS();
        }
      }
      //uint16_t Temp = (tempSens.temp / 100);
      //uint8_t Hum = tempSens.hum;
      //Serial.println(String(tempSens.temp / 100) + " *C");
      //Serial.println(String(tempSens.hum) + " %");
      indiDigits[0] = (tempSens.temp / 100) / 10;
      indiDigits[1] = (tempSens.temp / 100) % 10;
      anodeStates[2] = 0;
      anodeStates[3] = 0;
      indiDigits[4] = tempSens.hum / 10;
      indiDigits[5] = tempSens.hum % 10;
      getTime();  // синхронизация времени
      //Serial.println(String(alm_mins) + " alm_mins");
      //Serial.println(String(alm_hrs) + " alm_hrs");
      //Serial.println(String(digitalRead(ALARM_PIN)) + " ALARM");
      if (BACKL_MODE != 0) setBackLight();
      modeTimer.setTime((uint32_t)TEMP_TIME * 1000);
    } else if (stat_mode == Temperature) {
      stat_mode = Clock;
      if (SHOW_TEMP_HUM && SHOW_TEMP_DS) tempFlag = !tempFlag;  // меняем датчики
      sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
      anodeStates[2] = 1;
      anodeStates[3] = 1;
      modeTimer.setTime((uint32_t)CLOCK_TIME * 1000);
    }
  }
}


// читаем датчик давления
void stat_modeTickP() {
  if (modeTimerP.tick()) {
    if (stat_mode == Clock) {
      stat_mode = Pressure;
      readTempBME();
      //uint16_t Pres = tempSens.press;
      //Serial.println(String(tempSens.press) + " mm.pt.");
      anodeStates[0] = 0;
      indiDigits[1] = (tempSens.press / 100) % 10;
      indiDigits[2] = (tempSens.press / 10) % 10;
      indiDigits[3] = tempSens.press % 10;
      anodeStates[4] = 0;
      anodeStates[5] = 0;
      getTime();  // синхронизация времени
      if (BACKL_MODE != 0) setBackLight();
      modeTimerP.setTime((uint32_t)TEMP_TIME * 1000);
    } else if (stat_mode == Pressure) {
      stat_mode = Clock;
      sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
      anodeStates[0] = 1;
      anodeStates[4] = 1;
      anodeStates[5] = 1;
      modeTimerP.setTime((uint32_t)TEMP_PRES * 1000);
    }
  }
}

// читаем датчик CO2
void stat_modeTickCO() {
  if (modeTimerCO.tick()) {
    if (stat_mode == Clock) {
      stat_mode = CO2;
      //Serial.println("PPM = " + String(ppm));
      indiDigits[0] = (ppm / 1000) % 10;
      if (indiDigits[0] == 0) {
        anodeStates[0] = 0;
      }
      indiDigits[1] = (ppm / 100) % 10;
      indiDigits[2] = (ppm / 10) % 10;
      indiDigits[3] = ppm % 10;
      anodeStates[4] = 0;
      anodeStates[5] = 0;
      getTime();                            // синхронизация времени
      if (BACKL_MODE != 0) setBackLight();  // меняем цвет для показаний на время отображения
      modeTimerCO.setTime((uint32_t)TEMP_TIME * 1000);
    } else if (stat_mode == CO2) {
      stat_mode = Clock;
      sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
      anodeStates[0] = 1;
      anodeStates[4] = 1;
      anodeStates[5] = 1;
      modeTimerCO.setTime((uint32_t)TEMP_CO2 * 1000);
    }
  }
}

void stat_modeData() {
  if (stat_mode == Clock) {
    stat_mode = DATES;
    indiDigits[0] = RTC_time.DD / 10;
    indiDigits[1] = RTC_time.DD % 10;
    indiDigits[2] = RTC_time.MM / 10;
    indiDigits[3] = RTC_time.MM % 10;
    indiDigits[4] = RTC_time.DW / 10;
    indiDigits[5] = RTC_time.DW % 10;
  } else if (stat_mode == DATES) {
    stat_mode = Clock;
    getTime();  // синхронизация времени
    sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
    anodeStates[0] = 1;
    anodeStates[1] = 1;
    anodeStates[2] = 1;
    anodeStates[3] = 1;
    anodeStates[4] = 1;
    anodeStates[5] = 1;
  }
}

void pwm() {                                                                                // Снимаем показания с датчика СО2 MHZ19 по PWM!!!!!!!!!!!!! тут какая то магия!
  static boolean pinState_CO2;                                                              //предыдущее состояние пина
  static uint32_t last_time;                                                                //предыдущее время замера
  if (digitalRead(PWMPIN_CO2) == pinState_CO2) {                                            //если изменилось состояние пина
    if ((pinState_CO2 = !pinState_CO2) && last_time) ppm = (millis() - last_time - 2) * 5;  // где 5 это мануальные 5000 //если запомнили высокий уровень и время предыдущего замера было определено то рассчитываем ppm
    last_time = millis();                                                                   //запомнили предыдущее время
  }
  //Serial.println("PPM = " + String(ppm));
}