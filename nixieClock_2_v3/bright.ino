void dotBrightTick() {
  if (dotBrightFlag && dotBrightTimer.tick()) {
    if (dotBrightDirection) {
      dotBrightCounter += dotBrightStep;
      if (dotBrightCounter >= dotMaxBright) {
        dotBrightDirection = false;
        dotBrightCounter = dotMaxBright;
      }
    } else {
      dotBrightCounter -= dotBrightStep;
      if (dotBrightCounter <= 0) {
        dotBrightDirection = true;
        dotBrightFlag = false;
        dotBrightCounter = 0;
      }
    }
    //Serial.println("dotBrightCounter = " + String(dotBrightCounter));
    OCR1B = getPWM_CRT(dotBrightCounter);  // задаем значение регистра сравнения на D10. достигло - сбросилось в 0
    //if (dotBrightCounter) TCCR1A |= (0x01 << COM1B1);  //подключаем D10
    //else TCCR1A &= ~(0x01 << COM1B1);                  //отключаем D10
  }
}

void setBackLight() {

  //Serial.println("setBackLight = " + String(stat_mode));
  setLedBright(backlMaxBright);  // устанавливаем яркость светодиодов

  if (stat_mode == Temperature) {
    //uint8_t Hum = tempSens.hum;
    if (tempSens.hum <= 32) {
      setLedHue(15, WHITE_ON);  //установили цвет всех диодов поносный
    }
    if (tempSens.hum > 32 && tempSens.hum <= 40) {
      setLedHue(35, WHITE_ON);  //установили цвет всех диодов желтый
    }
    if (tempSens.hum > 40) {
      setLedHue(100, WHITE_ON);  //установили цвет всех диодов изумрудный
    }
    if (tempSens.temp < 0) {
      setLedHue(0, 210, WHITE_ON);  //установили цвет первых двух светодиодов синий, если температура < 0
      setLedHue(1, 210, WHITE_ON);
    }
    if (!tempFlag) {
      setLedHue(0, 210, WHITE_ON);  // если показывает данные с датчика DS голубой первый светодиод
    }
  }

  if (stat_mode == CO2) {  //индикация превышения СО2
    if (ppm >= 1000) {
      setLedHue(0, WHITE_ON);         //установили цвет всех диодов красный
    } else setLedHue(130, WHITE_ON);  //установили цвет всех диодов  //backLightLeds[i] = CHSV(100, 100, backlMaxBright);
  }

  if (stat_mode == Pressure) {
    setLedHue(160, WHITE_ON);  //установили цвет всех диодов синий
  }
  //Serial.println("test1");
  showLeds();  //отрисовка светодиодов
}

void LEDsFromColors() {
  static uint8_t color_steps;  //номер цвета
  static boolean backl_drv;    //направление огня
  static uint8_t backl_pos;    //положение огня
  static uint8_t backl_steps;  //шаги затухания

  if (effectTimer.tick()) {

    if (BACKL_MODE == 0) {  // BACKL_MODE = 0 пусто
      //setLedBright(0);            // погасил подсветку
      clrLeds();
    }

    if (BACKL_MODE == 1) {  // BACKL_MODE = 1 пусто, но подсветка с сенсоров работает
      clrLeds();            // погасил подсветку
    }

    if (BACKL_MODE == 2) {           // BACKL_MODE = 2 постоянный
      setLedBright(backlMaxBright);  // задали уровень подсветки
      setLedHue(190, WHITE_ON);      // фиолетовый
    }

    if (BACKL_MODE == 3) {               // BACKL_MODE = 3 радуга
      setLedBright(backlMaxBright);      // задали уровень подсветки
      color_steps += BACKL_MODE_3_STEP;  // прибавили шаг
      for (uint8_t f = 0; f < NUM_LEDS; f++) {
        setLedHue(f, color_steps + (f * BACKL_MODE_3_STEP), WHITE_OFF);  //установили цвет
      }
      effectTimer.setTime(BACKL_MODE_3_TIME);
    }

    if (BACKL_MODE == 4) {           //  BACKL_MODE = 4 градиент статика
      setLedBright(backlMaxBright);  //  задали уровень подсветки
      for (uint8_t f = 0; f < NUM_LEDS; f++) {
        setLedHue(f, color_steps + (f * BACKL_MODE_4_STEP), WHITE_ON);  //установили цвет
      }
    }

    if (BACKL_MODE == 5) {                                        //  BACKL_MODE = 5 рандомный цвет
      setLedBright(backlMaxBright);                               //  задали уровень подсветки
      setLedHue(random(0, NUM_LEDS), random(0, 256), WHITE_OFF);  //  установили цвет
      effectTimer.setTime(BACKL_MODE_5_TIME);
    }

    if (BACKL_MODE == 6) {  // BACKL_MODE = 6 волна
      if (backl_drv) {
        if (incLedBright(backl_pos, BACKL_MODE_6_STEP, backlMaxBright)) {  //прибавили шаг яркости
          if (backl_pos < (NUM_LEDS - 1)) backl_pos++;                     //сменили позицию
          else {
            backl_pos = 0;  //сбросили позицию
            backl_drv = 0;  //перешли в затухание
          }
        }
      } else {
        if (decLedBright(backl_pos, BACKL_MODE_6_STEP, 20)) {  //иначе убавляем яркость
          if (backl_pos < (NUM_LEDS - 1)) backl_pos++;         //сменили позицию
          else {
            backl_pos = 0;  //сбросили позицию
            backl_drv = 1;  //перешли в разгорание
          }
        }
      }
      //setLedHue(random(0, NUM_LEDS), random(0, 256));  //установили цвет
      color_steps += BACKL_MODE_6_COLOR;   //прибавили шаг
      setLedHue(color_steps, WHITE_OFF);  //установили цвет
      if (color_steps >= 255) {
        color_steps = 0;
      }
      effectTimer.setTime(BACKL_MODE_6_TIME);
    }

    if (BACKL_MODE == 7) {                                                   // BACKL_MODE = 7 дыхание
      if (backl_drv) {                                                       //если светодиоды в режиме разгорания
        if (incLedBright(BACKL_MODE_7_STEP, backlMaxBright)) backl_drv = 0;  //прибавили шаг яркости
      } else {                                                               //иначе светодиоды в режиме затухания
        if (decLedBright(BACKL_MODE_7_STEP, 5)) {                            //уменьшаем яркость
          backl_drv = 1;
          color_steps += BACKL_MODE_7_COLOR;  //меняем цвет
          setLedHue(color_steps, WHITE_OFF);  //установили цвет
        }
      }
      effectTimer.setTime(BACKL_MODE_7_TIME);
    }

    if (BACKL_MODE == 8) {                // BACKL_MODE = 8 бегущий огонь
      if (backl_steps) {                  //если есть шаги затухания
        decLedsBright(backl_pos - 1, 5);  //уменьшаем яркость
        backl_steps--;                    //уменьшаем шаги затухания
      } else {                            //иначе двигаем голову
        if (backl_drv) {                  //если направление вправо
          if (backl_pos < NUM_LEDS + 1) backl_pos++;
          else backl_drv = 0;  //едем вправо
        } else {               //иначе напрвление влево
          if (backl_pos > 0) backl_pos--;
          else backl_drv = 1;  //едем влево
        }
        setLedBright(backl_pos - 1, backlMaxBright);  //установили яркость
        backl_steps = BACKL_MODE_8_FADING;            //установили шаги затухания
      }
      color_steps += BACKL_MODE_8_COLOR;  //меняем цвет
      setLedHue(color_steps, WHITE_ON);   //установили цвет

      effectTimer.setTime(BACKL_MODE_8_TIME);
    }
    showLeds();  //отрисовка светодиодов
  }
}

void changeBright() {
  // установка яркости всех светилок от времени суток
  if ((RTC_time.h >= NIGHT_START && RTC_time.h <= 23)
      || (RTC_time.h >= 0 && RTC_time.h < NIGHT_END)) {
    indiMaxBright = INDI_BRIGHT_N;
    dotMaxBright = DOT_BRIGHT_N;
    backlMaxBright = BACKL_BRIGHT_N;
  } else {
    indiMaxBright = INDI_BRIGHT;
    dotMaxBright = DOT_BRIGHT;
    backlMaxBright = BACKL_BRIGHT;
  }
  for (uint8_t i = 0; i < 6; i++) {
    indiDimm[i] = indiMaxBright;
  }
  //Serial.println(String(backlMaxBright) + " backlMaxBright");
  dotBrightStep = ceil((float)dotMaxBright * 2 / DOT_TIME * DOT_TIMER);  ////расчет шага яркости точки
  if (dotBrightStep == 0) dotBrightStep = 1;
}