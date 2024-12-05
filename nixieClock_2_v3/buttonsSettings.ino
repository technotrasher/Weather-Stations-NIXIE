void settingsTick() {
  // 1- настройка времени 2- настойка будильника 3- настройка даты 4- настройка года
  if (curMode == 1 || curMode == 2 || curMode == 3 || curMode == 4) {
    //Serial.println(String(curMode) + " curMode");
    //Serial.println(String(stat_mode) + " stat_mode");
    if (blinkTimer.tick()) {
      if (stat_mode == ClockSet) sendTimeInd(changeHrs, changeMins, RTC_time.s);
      if (stat_mode == AlarmSet) sendTimeInd(alm_hrs, alm_mins, 0);
      if (stat_mode == DataSet) sendTimeInd(changedays, changemnth, RTC_time.DW);
      if (stat_mode == DataSetY) sendTimeInd(20, changeyers, 0);
      lampState = !lampState;
      if (lampState) {
        anodeStates[0] = 1;
        anodeStates[1] = 1;
        anodeStates[2] = 1;
        anodeStates[3] = 1;
        anodeStates[4] = 1;
        anodeStates[5] = 1;
      } else {
        if (!currentDigit) {
          anodeStates[0] = 0;
          anodeStates[1] = 0;
        } else {
          anodeStates[2] = 0;
          anodeStates[3] = 0;
        }
      }
    }
  }
}

void buttonsTick() {
  uint16_t analog = analogRead(BTN);
  _delay_us(1000);

  //Serial.println(String(analog) + " подстройка нажатия кнопки");  //bebug
  //btnSet.tick(analog > 700 && analog < 800);                      //set желтая
  //btnUp.tick(analog > 400 && analog < 500);                       //  + черная
  //btnDwn.tick(analog >= 0 && analog < 50);                      // - красная
  btnSet.tick(analog > 250 && analog < 340);                      //set желтая
  btnUp.tick(analog > 360 && analog < 500);                       //  + черная
  btnDwn.tick(analog >= 0 && analog < 200);                      // - красная

  if (curMode == 1) {
    if (btnDwn.isClick()) {  //  клик по правой кнопке в режиме настройки часов
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (!currentDigit) {
        changeHrs++;
        if (changeHrs > 23) changeHrs = 0;
        //Serial.println(String(changeHrs) + "  =changeHrs");
      } else {
        changeMins++;
        if (changeMins > 59) {  // если минут наклацали больше 60, то прибавить час
          changeMins = 0;
          changeHrs++;
          if (changeHrs > 23) changeHrs = 0;
        }
        //Serial.println(String(changeMins) + "  =changeMins");
      }
    }

    if (btnUp.isClick()) {  //  клик по левой кнопке в режиме настройки часов
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (!currentDigit) {
        changeHrs--;
        if (changeHrs < 0) changeHrs = 23;
        //Serial.println(String(changeHrs) + "  =changeHrs");
      } else {
        changeMins--;
        if (changeMins < 0) {  // если минут наклацали меньше 0, то отнять час
          changeMins = 59;
          changeHrs--;
          if (changeHrs < 0) changeHrs = 23;
        }
        //Serial.println(String(changeMins) + "  =changeMins");
      }
    }

  } else if (curMode == 0) {

    // переключение эффектов цифр
    if (btnDwn.isClick()) {
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (++FLIP_EFFECT >= FLIP_EFFECT_NUM) FLIP_EFFECT = 0;
      //Serial.println (String(FLIP_EFFECT) + " FLIP_EFFECT"); // тест
      EEPROM_UpdateByte(0, FLIP_EFFECT);
      flipTimer.setTime(FLIP_SPEED[FLIP_EFFECT]);
      for (uint8_t i = 0; i < 6; i++) {
        indiDimm[i] = indiMaxBright;
        anodeStates[i] = 1;
      }
      // показать эффект
      newTimeFlag = true;
      for (uint8_t i = 0; i < 4; i++) indiDigits[i] = FLIP_EFFECT;
    }

    // переключение эффектов подсветки
    if (btnUp.isClick()) {
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (++BACKL_MODE > 8) BACKL_MODE = 0;
      EEPROM_UpdateByte(1, BACKL_MODE);
    }

    // показать дату
    if (btnUp.isHolded()) {
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      stat_modeData();
    }

    // отключить получение времени из интернета
    if (btnDwn.isHolded()) {
      TimerFreeTone(PIEZO_PORT, 700, 150, 5);
      SHOW_ESP = !SHOW_ESP;
      EEPROM_UpdateByte(2, SHOW_ESP);
      Serial.println(String(SHOW_ESP) + " = SHOW_ESP");
    }

    // перевод зима лето вручную
    if (btnUp.isDouble()) {
      TimerFreeTone(PIEZO_PORT, 700, 150, 5);
      esptime = !esptime;
      EEPROM_UpdateByte(3, esptime);
      Serial.println(String(esptime) + " = esptime");
    }

    // отключение эффкта секунд
    if (btnSet.isClick()) {
      TimerFreeTone(PIEZO_PORT, 700, 80, 5);
      if (++SHOW_SEC > 2) SHOW_SEC = 0;
      EEPROM_UpdateByte(4, SHOW_SEC);
    }

  } else if (curMode == 2) {
    if (btnDwn.isClick()) {  //  клик по правой кнопке в режиме настройки будильника
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (!currentDigit) {
        alm_hrs++;
        if (alm_hrs > 23) alm_hrs = 0;
      } else {
        alm_mins++;
        if (alm_mins > 59) {  // если минут наклацали больше 60, то прибавить час
          alm_mins = 0;
          alm_hrs++;
          if (alm_hrs > 23) alm_hrs = 0;
        }
      }
    }

    if (btnUp.isClick()) {  //  клик по левой кнопке в режиме настройки будильника
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (!currentDigit) {
        alm_hrs--;
        if (alm_hrs < 0) alm_hrs = 23;
      } else {
        alm_mins--;
        if (alm_mins < 0) {  // если минут наклацали меньше 0, то отнять час
          alm_mins = 59;
          alm_hrs--;
          if (alm_hrs < 0) alm_hrs = 23;
        }
      }
    }

  } else if (curMode == 3) {
    if (btnDwn.isClick()) {  //  клик по правой кнопке в режиме настройки даты
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (!currentDigit) {
        changedays++;
        if (changedays > 31) changedays = 0;
        //Serial.println(String(changedays) + "  =changedays");
      } else {
        changemnth++;
        if (changemnth > 12) changemnth = 0;
        //Serial.println(String(changemnth) + "  =changemnth");
      }
    }

    if (btnUp.isClick()) {  //  клик по правой кнопке в режиме настройки даты
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (!currentDigit) {
        changedays--;
        if (changedays < 0) changedays = 31;
        //Serial.println(String(changedays) + "  =changedays");
      } else {
        changemnth--;
        if (changemnth < 0) changemnth = 12;
        //Serial.println(String(changemnth) + "  =changemnth");
      }
    }
  } else if (curMode == 4) {
    if (btnDwn.isClick()) {  //  клик по правой кнопке в режиме настройки годов
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (!currentDigit) {
        // 20 не меняем.
      } else {
        changeyers++;
        if (changeyers > 40) changeyers = 0;
      }
    }
    if (btnUp.isClick()) {  //  клик по правой кнопке в режиме настройки годов
      TimerFreeTone(PIEZO_PORT, 700, 50, 5);
      if (!currentDigit) {
        // 20 не меняем.
      } else {
        changeyers--;
        if (changeyers < 0) changeyers = 40;
      }
    }
  }

  if (btnSet.isHolded()) {
    anodeStates[0] = 1;
    anodeStates[1] = 1;
    anodeStates[2] = 1;
    anodeStates[3] = 1;
    currentDigit = false;
    if (++curMode > 4) curMode = 0;  // изменение curMode на +1
    TimerFreeTone(PIEZO_PORT, 700, 50, 5);
    switch (curMode) {
      case 0:
        stat_mode = Clock;
        if (RTC_time.h != changeHrs) {
          RTC_time.h = changeHrs;
          sendTimeH();
        }
        if (RTC_time.m != changeMins) {
          RTC_time.m = changeMins;
          sendTimeM();
        }
        if (RTC_time.DD != changedays) {
          RTC_time.DD = changedays;
          sendTimeD();
        }
        if (RTC_time.MM != changemnth) {
          RTC_time.MM = changemnth;
          sendTimeMH();
        }
        if (RTC_time.YY != changeyers + 2000) {
          RTC_time.YY = changeyers + 2000;
          sendTimeY();
        }
        //RTC_time.s = 0; // чтоб секунды обнулялись при настройке
        getTime();
        EEPROM_UpdateByte(8, alm_hrs);
        EEPROM_UpdateByte(12, alm_mins);
        sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
        break;
      case 1:
        ef_flag = false;  // не разрешаем запуск эффектов
        stat_mode = ClockSet;
        changeHrs = RTC_time.h;
        changeMins = RTC_time.m;
        break;
      case 2:
        ef_flag = false;
        stat_mode = AlarmSet;
        break;
      case 3:
        ef_flag = false;
        stat_mode = DataSet;
        changedays = RTC_time.DD;
        changemnth = RTC_time.MM;
        break;
      case 4:
        ef_flag = false;
        stat_mode = DataSetY;
        changeyers = RTC_time.YY - 2000;
        break;
    }
  }

  if (btnSet.isClick()) {
    TimerFreeTone(PIEZO_PORT, 700, 50, 5);
    if (curMode == 1 || curMode == 2 || curMode == 3 || curMode == 4) currentDigit = !currentDigit;  // переключение индикаторов часов или минут
  } else {
    !SHOW_TR;
    !SHOW_TEST;
  }
}