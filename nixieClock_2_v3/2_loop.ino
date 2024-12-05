void loop() {
  if (dotTimer.tick() && (stat_mode == Clock || stat_mode == ClockSet || stat_mode == AlarmSet || stat_mode == DataSet || stat_mode == DataSetY)) calculateTime();                  // каждые 500 мс пересчёт и отправка времени
  if (newTimeFlag && stat_mode == Clock) flipTick(); //включение эффектов при переходе через минуту и час
  if (newTimeFlagsec && stat_mode == Clock) Ticksec(); // эффект секунды                                                                                                                                // перелистывание цифр
  dotBrightTick();                                                                                                                                                                  // плавное мигание точки
  if ((SHOW_GL && stat_mode == Clock) || stat_mode == Temperature || stat_mode == Pressure || stat_mode == CO2) glitchTick();                                                       // глюки
  buttonsTick();                                                                                                                                                                    // кнопки
  settingsTick();                                                                                                                                                                   // настройки
  pwm();                                                                                                                                                                            // данные с датчика СО2 MHZ19
  if (SHOW_ESP && !alm_flag && ef_flag && ESPTimer.tick()) realtime();                                                                                                              // синхронизация времени через WIFI
  if (stat_mode > 8 || curMode > 4) resetFunc();                                                                                                                                    // перезагрузка при глюке ака костыль  4 режим
  if ((SHOW_TEMP_HUM || SHOW_TEMP_DS) && !alm_flag && ef_flag && stat_mode != AlarmSet && stat_mode != ClockSet && stat_mode != DataSet && stat_mode != DataSetY) stat_modeTick();  //температура влажность
  if (SHOW_CO2 && !alm_flag && ef_flag && stat_mode != AlarmSet && stat_mode != ClockSet && stat_mode != DataSet && stat_mode != DataSetY) stat_modeTickCO();                       // СО2
  if (SHOW_PRES && !alm_flag && ef_flag && stat_mode != AlarmSet && stat_mode != ClockSet && stat_mode != DataSet && stat_mode != DataSetY) stat_modeTickP();                       // отображать давление
  if (SHOW_TR && !alm_flag && ef_flag && stat_mode != AlarmSet && stat_mode != ClockSet && stat_mode != DataSet && stat_mode != DataSetY) train_Event();                            //поезд.
  if (SHOW_TEST && !alm_flag && ef_flag && stat_mode != AlarmSet && stat_mode != ClockSet && stat_mode != DataSet && stat_mode != DataSetY) TestTimerI();                           //перебор индикаторов. тут можно отключить
  if (stat_mode == Clock && !alm_flag) {
    LEDsFromColors();  // установить подсветку
  }
}