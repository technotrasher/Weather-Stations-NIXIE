проект Часы на ГРИ - погодная станция.
Плата универсальна. под все лампы. платы две. основная и плата для ламп.
платы держатели ламп разные. по ИН12, ИН14 и тд.
основная плата выполненна без АРДУИНО! теперь голая атмега. нужно прошить загрузчик. я рекомендую optiboot.
https://github.com/Optiboot/optiboot/releases
на плате присутствуют пины для прошивки через программатор.
  Скетч, корпуса и платы к проекту "Часы на ГРИ версия 2.3"
  платы тут https://easyeda.com/technotrasher/nixie_clock_pcb_in14


                       При прошивке скетча отключать модуль ESP!!!!!!!!!!!!!!!!!!!!!!
  Управление:
  - Настройка времени:
    - Левая кнопка - выбор, остальные "больше" и "меньше"
    - Удержать "выбор" первый раз btnSet - настройка времени
    - Клик по "выбору" btnSet - смена настройки часы/минуты
    - Клик "больше" и "меньше" - изменение времени
    - Удержать "выбор" второй раз - настройка будильника
    - Левая кнопка - выбор, остальные "больше" и "меньше"
    - Удержать "выбор" третий раз - установка даты и месяца
    - Удержать "выбор" четвертый раз - установка года
    - Удержать "выбор" btnSet - настройка времени
    - Клик по "выбору" btnSet - смена настройки часы/минуты
    - Клик "больше" и "меньше" - изменение времени
    - Удержать "выбор" - возврат к показу времени
    - Удержание "больше" - отображение даты. вид - день. месяц. день недели. для выхода нажать и удержать повторно

    В модуле ESP в строке 58 указать свои настройки. имя точки вайфай и пароль к точке доступа. кавычки не трогать.
    В библиотеке wire - если на плате нет подтяжки пинов A4 A5 к vcc через резисторы 4.7кОм, то заменить строку
  PORTC &= 0xCF; //отключаем подтяжку SDA и SCL
  на строку
  PORTC |= 0x30; //включаем подтяжку SDA и SCL

  Подсветка
  1. режим отображения СО2 - ppm больше 1000, то цвет ярко красный
  2. отображение давления - синий цвет всех светодиодов
  3. Влажность - ниже 32% цвет зеленовато-желтый, 32-40 - желтый, больше 40 - изумрудный
  4. температура ниже нуля - цвет первых двух светодиодов синий
  5. температура с датчика выносного DS - первый светодиод голубой
 

  - Управление эффектами В РЕЖИМЕ ЧАСОВ:
    - Клик по кнопке btnUp переключает режимы подсветки ламп
    - выключена
    - выключена, но данные с датчиков подсвечиваются
    - постоянный свет
    - палитра PartyColors
    - палитра RainbowStripeColors
    - палитра CloudColors
    - Клик по кнопке btnDwn переключает режимы перелистывания цифр
      - Без эффекта
      - Плавное угасание
      - Перемотка по порядку числа
      - Перемотка по катодам
      - Поезд
      - Резинка
      - Блики
      - Перемотка со смещением

  Удержание кнопки btnUp
  - показ даты.
  для возврата удержать кнопку повторно

  Удержание кнопки btnDown
  - отключает запрос времени из интернета через модуль ESP (изначально включено) 0 = SHOW_ESP - выкл. 1 = SHOW_ESP - вкл
 
  Двойной клик по btnUP - смена часового пояса. работает только при работе модуля ESP
  после двойного нажатия - сигнал длиннее прозвучит (1 = esptime - лето 0 = esptime - зима)
  Одинарный клик по btnSet - включение/отключение эффекта секунд без эффекта, угасание, перемотка
