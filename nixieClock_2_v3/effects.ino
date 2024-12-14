//*
void Ticksec() {  // плавная смена секунда

  // без эффектов =======================================================
  if (SHOW_SEC == 0) {
    newTimeFlagsec = false;
  }

  // затухание цифры =============================================
  else if (SHOW_SEC == 1) {
    if (flipTimersec.tick()) {
      if (!indiBrightDirections) {
        indiBrightCounters--;           // уменьшаем яркость
        if (indiBrightCounters <= 5) {  // если яроксть меньше
          indiBrightDirections = true;  // меняем направление изменения
          indiBrightCounters = 0;       // обнуляем яркость
        }
      } else {
        indiBrightCounters++;                       // увеличиваем яркость
        if (indiBrightCounters >= indiMaxBright) {  // достигли предела
          indiBrightDirections = false;             // меняем направление
          indiBrightCounters = indiMaxBright;       // устанавливаем максимум
          newTimeFlagsec = false;
        }
      }
      for (uint8_t i = 4; i < 6; i++) {
        if (flipIndics[i]) indiDimm[i] = indiBrightCounters;  // применяем яркость
      }
      if (indiMaxBright == 5) {
        flipTimersec.setTime(150);  // при  яркости 5 ночной
      } else {
        flipTimersec.setTime(15);  // при яркости 23 дневной
      }
    }
  }

  // перемотка цифры =============================================
  else if (SHOW_SEC == 2) {
    if (flipTimersec.tick()) {
      for (uint8_t i = 4; i < 6; i++) {
        if (flipIndics[i]) {
          if (--indiDigits[i] < 0) indiDigits[i] = 9;
          if (indiDigits[i] == newTime[i]) newTimeFlagsec = false;
        }
      }
      if (indiMaxBright == 5) {
        flipTimersec.setTime(70);  // при  яркости 5 ночной
      } else {
        flipTimersec.setTime(25);  // при яркости 23 дневной
      }
    }
  }
}

void flipTick() {
  //Serial.println(String(FLIP_EFFECT) + " FLIP_EFFECT");
  // без эффектов =======================================================
  if (FLIP_EFFECT == 0) {
    sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
    newTimeFlag = false;
  }

  // плавное угасание цифры =============================================
  else if (FLIP_EFFECT == 1) {
    if (!flipInit) {
      flipInit = true;
      // запоминаем, какие цифры поменялись и будем менять их яркость
      for (uint8_t i = 0; i < 4; i++) {
        if (indiDigits[i] != newTime[i]) flipIndics[i] = true;
        else flipIndics[i] = false;
      }
    }
    if (flipTimer.tick()) {
      if (!indiBrightDirection) {
        indiBrightCounter--;                                // уменьшаем яркость
        if (indiBrightCounter <= 0) {                       // если яроксть меньше нуля
          indiBrightDirection = true;                       // меняем направление изменения
          indiBrightCounter = 0;                            // обнуляем яркость
          sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);  // меняем цифры
        }
      } else {
        indiBrightCounter++;                       // увеличиваем яркость
        if (indiBrightCounter >= indiMaxBright) {  // достигли предела
          indiBrightDirection = false;             // меняем направление
          indiBrightCounter = indiMaxBright;       // устанавливаем максимум
          // выходим из цикла изменения
          flipInit = false;
          newTimeFlag = false;
        }
      }
      for (uint8_t i = 0; i < 4; i++) {
        if (flipIndics[i]) indiDimm[i] = indiBrightCounter;  // применяем яркость
      }
    }
  }

  // Перемотка по порядку числа =====================================================?
  else if (FLIP_EFFECT == 2) {
    if (!flipInit) {
      flipInit = true;
      // запоминаем, какие цифры поменялись и будем менять их
      for (uint8_t i = 0; i < 4; i++) {
        if (indiDigits[i] != newTime[i]) flipIndics[i] = true;  // ищем индикатор на котором произошла смена цифры
        else flipIndics[i] = false;                             // блокируем индикатор на котором ничего не менялось. [0,0,0,1] например если менялась минута
      }
    }

    if (flipTimer.tick()) {
      flipCounter = 0;
      for (uint8_t i = 0; i < 4; i++) {
        if (flipIndics[i]) {  // находим индикатор на котором только сменилась цифра и стали на него
          indiDigits[i]--;
          if (indiDigits[i] < 0) indiDigits[i] = 9;                // если ушли в 0 то стали на 9
          if (indiDigits[i] == newTime[i]) flipIndics[i] = false;  // как только новое число стало равно старому - стоп
          //Serial.println(String(i) + " i");
          //Serial.println(String(indiDigits[i]) + " indiDigits");
          //Serial.println(String(newTime[i]) + " newTime");
        } else {
          flipCounter++;  // счётчик цифр, которые не надо менять
        }
        //Serial.println(String(flipCounter) + " flipCounter");
      }
      if (flipCounter == 4) {  // если ни одну из 4 цифр менять не нужно
        // выходим из цикла изменения
        //Serial.println(String(" выходим из цикла изменения"));
        flipInit = false;
        newTimeFlag = false;
      }
    }
  }

  // Перемотка по катодам ===========================================================
  else if (FLIP_EFFECT == 3) {
    if (!flipInit) {
      flipInit = true;
      // запоминаем, какие цифры поменялись и будем менять их
      for (uint8_t i = 0; i < 4; i++) {
        if (indiDigits[i] != newTime[i]) {
          flipIndics[i] = true;
          for (uint8_t c = 0; c < 10; c++) {
            if (cathodeMask[c] == indiDigits[i]) startCathode[i] = c;
            if (cathodeMask[c] == newTime[i]) endCathode[i] = c;
          }
        } else flipIndics[i] = false;
      }
    }

    if (flipTimer.tick()) {
      flipCounter = 0;
      for (uint8_t i = 0; i < 4; i++) {
        if (flipIndics[i]) {
          if (startCathode[i] > endCathode[i]) {
            startCathode[i]--;
            indiDigits[i] = cathodeMask[startCathode[i]];
          } else if (startCathode[i] < endCathode[i]) {
            startCathode[i]++;
            indiDigits[i] = cathodeMask[startCathode[i]];
          } else {
            flipIndics[i] = false;
          }
        } else {
          flipCounter++;  // счётчик цифр, которые не надо менять
        }
      }
      if (flipCounter == 4) {  // если ни одну из 4 цифр менять не нужно
        // выходим из цикла изменения
        flipInit = false;
        newTimeFlag = false;
      }
    }
  }

  // --- train --- поезд=====================================================
  else if (FLIP_EFFECT == 4) {
    if (!flipInit) {
      flipInit = true;
      currentLamp = 0;
      trainLeaving = true;
    }
    if (flipTimer.tick()) {
      if (trainLeaving) {
        for (uint8_t i = 3; i > currentLamp; i--) {
          indiDigits[i] = indiDigits[i - 1];
        }
        anodeStates[currentLamp] = 0;
        currentLamp++;
        if (currentLamp > 3) {
          trainLeaving = false;  //coming ;)
          currentLamp = 0;
        }
      } else {
        for (uint8_t i = currentLamp; i > 0; i--) {
          indiDigits[i] = indiDigits[i - 1];
        }
        indiDigits[0] = newTime[3 - currentLamp];
        anodeStates[currentLamp] = 1;
        currentLamp++;
        if (currentLamp > 3) {
          flipInit = false;
          newTimeFlag = false;
        }
      }
    }
  }

  // --- elastic band резинка--- =================================================
  else if (FLIP_EFFECT == 5) {
    if (!flipInit) {
      flipInit = true;
      flipEffectStages = 0;
    }
    if (flipTimer.tick()) {
      switch (flipEffectStages++) {
        case 1:
          anodeStates[3] = 0;
          break;
        case 2:
          anodeStates[2] = 0;
          indiDigits[3] = indiDigits[2];
          anodeStates[3] = 1;
          break;
        case 3:
          anodeStates[3] = 0;
          break;
        case 4:
          anodeStates[1] = 0;
          indiDigits[2] = indiDigits[1];
          anodeStates[2] = 1;
          break;
        case 5:
          anodeStates[2] = 0;
          indiDigits[3] = indiDigits[1];
          anodeStates[3] = 1;
          break;
        case 6:
          anodeStates[3] = 0;
          break;
        case 7:
          anodeStates[0] = 0;
          indiDigits[1] = indiDigits[0];
          anodeStates[1] = 1;
          break;
        case 8:
          anodeStates[1] = 0;
          indiDigits[2] = indiDigits[0];
          anodeStates[2] = 1;
          break;
        case 9:
          anodeStates[2] = 0;
          indiDigits[3] = indiDigits[0];
          anodeStates[3] = 1;
          break;
        case 10:
          anodeStates[3] = 0;
          break;
        case 11:
          indiDigits[0] = newTime[3];
          anodeStates[0] = 1;
          break;
        case 12:
          anodeStates[0] = 0;
          indiDigits[1] = newTime[3];
          anodeStates[1] = 1;
          break;
        case 13:
          anodeStates[1] = 0;
          indiDigits[2] = newTime[3];
          anodeStates[2] = 1;
          break;
        case 14:
          anodeStates[2] = 0;
          indiDigits[3] = newTime[3];
          anodeStates[3] = 1;
          break;
        case 15:
          indiDigits[0] = newTime[2];
          anodeStates[0] = 1;
          break;
        case 16:
          anodeStates[0] = 0;
          indiDigits[1] = newTime[2];
          anodeStates[1] = 1;
          break;
        case 17:
          anodeStates[1] = 0;
          indiDigits[2] = newTime[2];
          anodeStates[2] = 1;
          break;
        case 18:
          indiDigits[0] = newTime[1];
          anodeStates[0] = 1;
          break;
        case 19:
          anodeStates[0] = 0;
          indiDigits[1] = newTime[1];
          anodeStates[1] = 1;
          break;
        case 20:
          indiDigits[0] = newTime[0];
          anodeStates[0] = 1;
          break;
        case 21:
          flipInit = false;
          newTimeFlag = false;
      }
    }
  }

  //================== блики ========================
  else if (FLIP_EFFECT == 6) {
    if (!flipInit) {
      flipInit = true;
    }
    for (uint8_t i = 0; i < 6;) {  // колличество ламп учавствующих в эффекте
      curIndi = random(0, 6);
      for (uint8_t c = 0; c < 2;) {
        if (flipTimer.tick()) {  //если таймер истек
          switch (c) {
            case 0: anodeStates[curIndi] = 0; break;  //очистка индикатора
            case 1:
              for (uint8_t s = 0; s < 6; s++) {
                if (anodeStates[s] == 0) anodeStates[s] = 1;
              }
              i++;  //прибавляем цикл
              sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
              break;  //вывод часов
          }
          c++;  //прибавляем цикл
          flipInit = false;
          newTimeFlag = false;
        }
      }
    }
  }

  //================== движение с перемоткой ========================
  else if (FLIP_EFFECT == 7) {
    if (!flipInit) {
      flipInit = true;
      flipCounter = 0;
      currentLamp = 0;
      flipEffectStages = 0;
    }
    if (flipTimer.tick()) {
      if (currentLamp < 4) {
        switch (flipCounter) {
          case 0:
            for (uint8_t lamp = currentLamp + 1; lamp > 0; lamp--) {
              if ((lamp - 1) == (currentLamp - flipEffectStages)) {
                indiDigits[4 - lamp] = indiDigits[3 - currentLamp];  //вывод часов
                anodeStates[4 - lamp] = 1;                           //включаем индикатор
              } else anodeStates[4 - lamp] = 0;                      //очистка индикатора
            }
            if (flipEffectStages++ >= currentLamp) {
              flipEffectStages = 0;  //сбрасываем позицию индикатора
              currentLamp++;         //прибавляем цикл
            }
            break;
          case 1:
            for (uint8_t lamp = 0; lamp < (4 - currentLamp); lamp++) {
              if (lamp == flipEffectStages) {
                indiDigits[lamp] = newTime[3 - currentLamp];  //вывод часов
                anodeStates[lamp] = 1;                        //включаем индикатор
              } else anodeStates[lamp] = 0;                   //очистка индикатора
            }
            if (flipEffectStages++ >= (3 - currentLamp)) {
              flipEffectStages = 0;  //сбрасываем позицию индикатора
              flipCounter = 2;
              currentLamp++;  //прибавляем цикл
              if (currentLamp < 4) indiDigits[0] = newTime[3 - currentLamp];
            }
            break;
          case 2:
            if (--indiDigits[0] < 0) indiDigits[0] = 9;                      //если переполнение
            anodeStates[0] = 1;                                              //включаем индикатор
            if (indiDigits[0] == newTime[3 - currentLamp]) flipCounter = 1;  //если начало анимации
            break;
          default:
            flipInit = false;
            newTimeFlag = false;
            break;
        }
      } else {
        if (!flipCounter) {
          anodeStates[3] = 0;  //выключаем индикатор
          indiDigits[0] = newTime[3];
        }
        currentLamp = 0;
        flipCounter += 2;
      }
    }
  }
}

void TestTimer_Event() {  // тест индикаторов
  for (uint8_t i = 0; i < 10; i++) {
    indiDigits[0] = i;
    indiDigits[1] = i;
    indiDigits[2] = i;
    indiDigits[3] = i;
    indiDigits[4] = i;
    indiDigits[5] = i;
    _delay_ms(300);
  }
}

void train_Event() {  //убегание цифр по индикаторам, ака поезд!
  if (trainTimer.tick()) {
    if (TrainFlag) currentLamp = 6;
    TrainFlag = false;
    currentLamp--;
    for (uint8_t i = 0; i < currentLamp; i++) {
      indiDigits[i] = indiDigits[i + 1];
    }
    anodeStates[currentLamp] = 0;
    trainTimer.setTime(350);
    if (currentLamp == 0) {
      for (uint8_t i = 0; i < 6; i++) {
        anodeStates[i] = 1;
      }
      sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
      trainTimer.setTime(random(train_MIN * 1000L, train_MAX * 1000L));
      TrainFlag = true;
    }
  }
}

void TestTimerI() {  //тест индикаторов
  if (testtimer.tick()) {
    if (TestFlag) testIndicator = 10;
    TestFlag = false;
    testIndicator--;
    indiDigits[0] = testIndicator;
    indiDigits[1] = testIndicator;
    indiDigits[2] = testIndicator;
    indiDigits[3] = testIndicator;
    indiDigits[4] = testIndicator;
    indiDigits[5] = testIndicator;
    testtimer.setTime(200);
    if (testIndicator == 0) {
      sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
      testtimer.setTime(random(test_MIN * 1000L, test_MAX * 1000L));
      TestFlag = true;
    }
  }
}