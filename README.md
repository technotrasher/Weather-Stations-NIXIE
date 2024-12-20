Проект Часы на ГРИ - погодная станция.

Одна плата управления. универсальна под разные лампы. и платы для ламп.

платы тут https://easyeda.com/technotrasher/nixie_clock_pcb_in14

Функции: показ времени :), отображение температуры, влажности, давления и СО2. Возможность подключения выносного датчика DS18B20. основной - BME280.

С помощью ESP-M2 есть возможность синхронизации времени через WI-FI.

Будильник с мерзким звуком. звук нажатия кнопок. звук можно настроить в самом скетче. буззер ставить БЕЗ ГЕНЕРАТОРА на 5В! (отличные результаты с LD-BZPN-3510)

Посветка из 6 программируемых свветодиодов ws2812. устанавливаются на платы для ламп. своих посадочных мест не имеют. можно размещать по своему усмотрению. RGB не подходят!!!!

7 вариантов работы:

Клик по кнопке btnUp переключает режимы подсветки ламп

    - выключена
    
    - выключена, но данные с датчиков подсвечиваются
    
    - постоянный свет
    
    - палитра PartyColors
    
    - палитра RainbowStripeColors
    
    - палитра CloudColors
    
Так же в режимах отображения информации своя подсветка:

    - режим отображения СО2 - ppm больше 1000, то цвет ярко красный
    
    - отображение давления - синий цвет всех светодиодов
    
    - влажность - ниже 32% цвет зеленовато-желтый, 32-40 - желтый, больше 40 - изумрудный
    
    - температура ниже нуля - цвет первых двух светодиодов синий
    
    - температура с датчика выносного DS - первый светодиод голубой.
      
   Есть эффекты переключения цифр:
   
Клик по кнопке btnDwn переключает режимы перелистывания цифр отображения часов и минут во время их смены:

      - Без эффекта
      
      - Плавное угасание
      
      - Перемотка по порядку числа
      
      - Перемотка по катодам
      
      - Поезд
      
      - Резинка
      
      - Блики
      
      - Перемотка со смещением
      
 Клик по кнопке btnSet переключает режимы перелистывания цифр секунд:
 
      - Без эффекта
      
      - Плавное угасание
      
      - Перемотка по порядку числа
      
Удержание кнопки btnUp:

      - показ даты.
      
для возврата удержать кнопку повторно.

Так же присутствуют глюки и эффекты. не отключаются. проявляются рандомно.

    - поезд
    
    - перепад напряжения
    
    - перебор катодов
    
    - неизвестно что

Основная плата выполненна на  AT328p. B неё нужно прошить загрузчик. я рекомендую optiboot.

В папке 328P_optiboot программа, сам загрузчик и скрин с настройками.
Загружать через программатор и разъем ISP на плате. после чего загружать скетчи можно через arduino ide и разъем USB.
Как вариант загружать можно скетч и через программатор. как подсоединять программатор, как загружать скетч описывать не буду.
все есть в интернете

При прошивке скетча отключать модуль ESP с помощью джамперов. !!!!!!!!!!!!!!!!!!!!!

ESP-2M обязательно обновить версию ПО. 

https://wiki.sipeed.com/soft/maixpy/en/get_started/upgrade_esp8285_firmware.html

в папке programm_esp есть программа, скрин с настройками и само ПО.

для прошивки нужен конвертер USB-TTL CH340G. или любой другой подходящий.
после загрузки ПО обязательно перевести модуль в режим точки клиента WI-Fi командой AT+CWMODE_DEF=1
о работе с модулями ESP так же есть вся информация в интернете.
