/// Внимание - модуль памяти в ESP расчитан на 10000 перезаписей. Используйте _CUR !!!!!
/// Версия прошивки модуля 1.7.4 рекомендую обновиться до нее. на старых прошивках работа не гарантированна
/// Измените в строке "доступ к домашнему роутеру" - имя точки и пароль!!!!!!!!!!!!!!!!
////ESPport.println("AT+UART_DEF=9600,8,1,0,0"); // установка скорости модуля в 9600  - не трогать!!!!
///иначе восстановить только сбросом к заводским AT+RESTORE
#define BUFFER_SIZE 256
//EEPROM_UpdateByte(3, 0); // esptime = false первый запуск
bool esptime = EEPROM_ReadByte(3);
char response[BUFFER_SIZE];
//boolean connectTime;  // флаг получения или ошибки получения времени
//HardwareSerial & ESPport = Serial; // например ATmega328P - arduino nano работа через serial. при программировании отключить модуль esp.
// если у вас MEGA (arduino mega 2560 pro, например), то закомментируйте верхнюю строчку и раскомментируйте нижеследующую.
// ESP подключите к Serial1 (18 - TX, 19 - RX) HardwareSerial & ESPport = Serial1;
//HardwareSerial & ESPport = Serial1;

/////////////////////отправка АТ-команд/////////////////////
boolean GetResponse(const char *AT_Command, const char *expected_answer, uint16_t wait) {  // wait - milliseconds
  uint8_t x = 0;
  boolean answer = false;  // флаг ошибки обена работы с AT командами
  unsigned long previous;
  memset(response, '\0', BUFFER_SIZE);  // инициализация строки
  _delay_ms(100);
  while (ESPport.available() > 0) ESPport.read();  // очистить буфер данных
  ESPport.println(AT_Command);                     // записываем в порт АТ команды
  previous = millis();
  // ждем ответ
  do {
    if (ESPport.available() != 0) {
      response[x] = ESPport.read();
      x++;
      // сравнение на совпадение
      if (strstr(response, expected_answer) != NULL) {
        answer = true;
      }
    }
  }
  // контроль времени ожидания. нашли "OK" - продолжаем
  while ((answer == false) && ((millis() - previous) < wait));  // пока ищем ОК задержка
  //Serial.println(response);  //то что выводится ответом на АТ. для тестирования
  return answer;
}

//----------------------------------------------------------------------------
// чтение реального времени из ESP возвращает
// 1-если операция прошла корректно
// 0-если ошибка
void realtime() {

  boolean errorconnect = false;  // флаг ошибки обмена работы с ESP
  const char *ucazat;            // указатель на данные
  char mas[4];

  if (GetResponse("AT", "OK", 300)) {           // проверка ESP
    GetResponse("AT+RST", "OK", 2000);          // перезагрузка ESP AT+RST
    //GetResponse("AT+RESTORE", "OK", 2000);    // перезагрузка заводские настройки
    GetResponse("AT+CWMODE_CUR=1", "OK", 500);  // режим клиента 300
    //GetResponse("AT+CSYSWDTENABLE", "OK", 300); // сторожевой таймер AT+CSYSWDTDISABLE — деактивировать.
    //GetResponse("AT+CIPMUX=0", "OK", 300);                                    // режим одного соединения.
    if (GetResponse("AT+CWJAP_CUR=\"name_wi_fi\",\"pass\"", "OK", 6500)) {  // подключаемся к домашнему роутеру!!!!!! изменить на свои 6500
      //GetResponse("AT+PING=\"google.com\"", "OK", 500);                                                            // для теста.
      TimerFreeTone(PIEZO_PORT, 1000, 150, 5);                                                                     // звук подключения
      if (GetResponse("AT+CIPSNTPCFG=1,2,\"0.pool.ntp.org\",\"1.pool.ntp.org\",\"2.pool.ntp.org\"", "OK", 500)) {  // подключиться к серверам
        _delay_ms(500);                                                                                            // задержка обязательна если тупит провайдер.
        //Serial.println("TIME SERVERS SET "); // для отладки обязательно
        if (GetResponse("AT+CIPSNTPTIME?", "OK", 500)) {  // - запрос времени!
          //Serial.println("WiFi - Module is ready GET TIME "); // для отладки обязательно
          ucazat = strstr(response, ":");  // ищем первый ":" (13 байт) получаем указатель на начало данных
          //Serial.println(*ucazat - 0x30); // "наполнение" байта
          //Serial.println(uint8_t(ucazat)); // байт в строке

          // день недели
          ucazat++;  // :
          mas[0] = *ucazat++;
          mas[1] = *ucazat++;
          mas[2] = *ucazat++;
          mas[3] = 0;

          // преобразование имени месяца в цифру
          if (!strcmp(mas, "Mon")) mas[0] = 1;
          else if (!strcmp(mas, "Tue")) mas[0] = 2;
          else if (!strcmp(mas, "Wed")) mas[0] = 3;
          else if (!strcmp(mas, "Thu")) mas[0] = 4;
          else if (!strcmp(mas, "Fri")) mas[0] = 5;
          else if (!strcmp(mas, "Sat")) mas[0] = 6;
          else if (!strcmp(mas, "Sun")) mas[0] = 7;

          // загрузить день недели
          //RTC_time.DW = mas[0];  // не нужно. расчитывается через getWeekDay. закомментировать при использовании!!!!!!!!!

          // месяц
          ucazat++;  // пробел
          mas[0] = *ucazat++;
          mas[1] = *ucazat++;
          mas[2] = *ucazat++;
          mas[3] = 0;

          // преобразование имени месяца в цифру
          if (!strcmp(mas, "Jan")) mas[0] = 1;
          else if (!strcmp(mas, "Feb")) mas[0] = 2;
          else if (!strcmp(mas, "Mar")) mas[0] = 3;
          else if (!strcmp(mas, "Apr")) mas[0] = 4;
          else if (!strcmp(mas, "May")) mas[0] = 5;
          else if (!strcmp(mas, "Jun")) mas[0] = 6;
          else if (!strcmp(mas, "Jul")) mas[0] = 7;
          else if (!strcmp(mas, "Aug")) mas[0] = 8;
          else if (!strcmp(mas, "Sep")) mas[0] = 9;
          else if (!strcmp(mas, "Oct")) mas[0] = 10;
          else if (!strcmp(mas, "Nov")) mas[0] = 11;
          else if (!strcmp(mas, "Dec")) mas[0] = 12;
          RTC_time.MM = mas[0];  // загрузить значение месяца

          // день - число месяца
          ucazat++;                              // пробел
          mas[0] = (uint8_t)(*ucazat++ - 0x30);  // десятки
          mas[1] = (uint8_t)(*ucazat++ - 0x30);  // единицы
          RTC_time.DD = mas[0] * 10 + mas[1];    // загрузить день - число месяца

          // часы
          ucazat++;                              // пробел
          mas[0] = (uint8_t)(*ucazat++ - 0x30);  // десятки
          mas[1] = (uint8_t)(*ucazat++ - 0x30);  // единицы " - 0x30 Перевод числа с типа string в int"
          RTC_time.h = mas[0] * 10 + mas[1];     // записать часы

          // коррекция летнее зимнее время по умолчанию 2 пояс зимнее время

          if (RTC_time.MM == 3 && RTC_time.DD >= 25 && RTC_time.DW == 7 && RTC_time.h == 4 && RTC_time.m == 0) {  // с марта месяца добавляем час
            esptime = true;
            EEPROM_UpdateByte(3, esptime);
          }
          if (RTC_time.MM == 10 && RTC_time.DD >= 25 && RTC_time.DW == 7 && RTC_time.h == 4 && RTC_time.m == 0) {  // ноябрь флаг не добавляем час
            esptime = false;
            EEPROM_UpdateByte(3, esptime);
          }

          if (esptime) {
            RTC_time.h = RTC_time.h + 1;
            if (RTC_time.h > 23) RTC_time.h = 0;
          }

          // минуты
          ucazat++;                              // :
          mas[0] = (uint8_t)(*ucazat++ - 0x30);  // десятки
          mas[1] = (uint8_t)(*ucazat++ - 0x30);  // единицы
          RTC_time.m = mas[0] * 10 + mas[1];     // записать минуты

          // секунды
          ucazat++;                               // :
          mas[0] = (uint8_t)(*ucazat++ - 0x30);   // десятки
          mas[1] = (uint8_t)(*ucazat++ - 0x30);   // единицы
          RTC_time.s = mas[0] * 10 + mas[1] + 6;  // можно записать секунд +6 коррекции

          // год
          ucazat++;  // пробел
          mas[0] = (*ucazat++ - 0x30);
          mas[1] = (*ucazat++ - 0x30);
          mas[2] = (*ucazat++ - 0x30);
          mas[3] = (*ucazat++ - 0x30);
          RTC_time.YY = mas[0] * 1000 + mas[1] * 100 + mas[2] * 10 + mas[3];  // записать год
        } else {
          //Serial.println("NO CONNECT TIME SERVERS"); // для отладки обязательно
          errorconnect = true;                        // нет связи
          GetResponse("AT+CIPSNTPCFG=0", "OK", 300);  // отключиться от серверов
          GetResponse("AT+CWQAP", "OK", 300);         // отключиться от роутера
        }
      } else {
        //Serial.println("NO SET TIME SERVERS"); // для отладки обязательно
        errorconnect = true;                        // нет связи
        GetResponse("AT+CIPSNTPCFG=0", "OK", 300);  // отключиться от серверов
        GetResponse("AT+CWQAP", "OK", 300);         // отключиться от роутера
      }
      //------------------------------------------------------------------------------
      if (!errorconnect) {
        //Serial.println(String(RTC_time.DW) + " weekday");
        //Serial.println(String(RTC_time.MM) + " manth");
        //Serial.println(String(RTC_time.DD) + " days");
        //Serial.println(String(RTC_time.h) + " hr");
        //Serial.println(String(RTC_time.m) + " min");
        //Serial.println(String(RTC_time.s) + " sec");
        Serial.println(String(RTC_time.YY) + " year");
        GetResponse("AT+CIPSNTPCFG=0", "OK", 300);  // отключиться от серверов
        GetResponse("AT+CWQAP", "OK", 300);         // отключиться от роутера
        // 1970 если не полученно данных но подключение есть: +CIPSNTPTIME:Thu Jan 01 00:00:00 1970
        //connectTime = true;
        if (RTC_time.YY != 1970) {
          sendTimeInd(RTC_time.h, RTC_time.m, RTC_time.s);
          sendTime();  // отправка точного времени
        } else {
          getTime();  // если не получено время запросим время из RTC
        }
      } else {
        //Serial.println("data error ");  // для отладки обязательно
        //connectTime = false;
      }

    } else {
      //Serial.println("WiFi router - connect error ");  // для отладки обязательно
      //connectTime = false;
    }
  } else {
    //Serial.println("WiFi - Module not found ");  // для отладки обязательно
    //connectTime = false;
  }
  //------------------------------------------------------------------------------
}