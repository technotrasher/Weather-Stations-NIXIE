//void Time() {  // синхронизация времени
  //getTime();   // синхронизация с RTC
  //RTC_time.s;  //секунды
  //RTC_time.m;  //минуты
  //RTC_time.h;  // часы
  //RTC_time.MM; // месяц
  //RTC_time.DD; // день
  //RTC_time.DW; //ДЕНЬ_НЕДЕЛИ - число от 0 (воскресение) до 6 (суббота)
  //RTC_time.YY - 2000; // год

  //Serial.println(String(RTC_time.s) + " sec");
  //Serial.println(String(RTC_time.m) + " min");
  //Serial.println(String(RTC_time.h) + " hr");
  //Serial.println(String(RTC_time.MM) + " manth");
  //Serial.println(String(RTC_time.DD) + " days");
  //Serial.println(String(RTC_time.DW) + " weekday");
  //Serial.println(String(RTC_time.YY) + " year");
  //Serial.println(String(changeyers) + " changeyers");
  //Serial.println(String(changeHrs) + "  =changeHrs");
  //Serial.println(String(changeMins) + "  =changeMins");
  //Serial.println(String(alm_hrs) + "  =alm_hrs");
  //Serial.println(String(alm_mins) + "  =alm_mins");
//}


// отправить на индикаторы
void sendTimeInd(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  indiDigits[0] = hours / 10;
  indiDigits[1] = hours % 10;

  indiDigits[2] = minutes / 10;
  indiDigits[3] = minutes % 10;

  indiDigits[4] = seconds / 10;
  indiDigits[5] = seconds % 10;
}

// отправить на индикаторы только секунды
void sendTimeSec(uint8_t seconds) {
  indiDigits[4] = seconds / 10;
  indiDigits[5] = seconds % 10;
}

// для эффектов
void setNewTime() {
  newTime[0] = RTC_time.h / 10;
  newTime[1] = RTC_time.h % 10;

  newTime[2] = RTC_time.m / 10;
  newTime[3] = RTC_time.m % 10;

  newTime[4] = RTC_time.s / 10;
  newTime[5] = RTC_time.s % 10;
}