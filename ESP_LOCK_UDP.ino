/*
   Электронный замок с бесконтактным доступом по технологии RFID
   Использован RFID модуль RDM6300
   Подача звукового сигнала при помощи баззера
   Остальные функции реализуются при помощи кнопки на внутренней стороне двери
   Запись нового ключа: поднесите мастер-метку при открытой двери и зажатой кнопке до сигнала (2 писка)
   Удаление записанного ключа: аналогично поднесите метку при зажатой кнопке до сигнала (3 писка)
   Удаление всех ключей: зажать кнопку на 3 секунды после подачи питания до сигнала
   При отсутствии записанных ключей дверь блокироваться не будет.
*/
#include "Timer.h"                // Класс таймера
#include "Constants.h"            // Настройки
#include "BUZZER.h"               // Класс и структура баззера
#include <EEPROM.h>               // Библиотека EEPROM для хранения ключей
#include <EncButton.h>

//#define UDP_USE
#ifdef UDP_USE
#include <ESP8266WiFi.h>
#include "ESPAsyncUDP.h"
const char * ssid = "Persik";
const char * password = "y5durtbn";
AsyncUDP udp;
ip_addr_t addr;
uint16_t port = 1234;
#endif


#define DEBUG_EN
#define RDM_SERIAL


#ifdef RDM_SERIAL
#define NEWTAG    Serial.available()
#define CURRTAG   Serial.parseInt()
#define CLRSERIAL while (Serial.available() > 0) Serial.read()
#else
#define NEWTAG    rdm6300.get_new_tag_id()
#define CURRTAG   rdm6300.get_tag_id()
#define CLRSERIAL
#endif


/*
    Схема использования EEPROM памяти:
    Используются адреса:
    Начало    Длина    Описание
    0        1         Признак первого запуска
    1        1         Число меток
   ******************* массив меток
    2-9      8         Мастер-метка
    10-17    8         метка №2
    18-25    8         метка №3
    ...
*/

#ifdef DEBUG_EN
#define DEBUG(x) Serial.print(x)
#define DEBUGLN(x) Serial.println(x)
#define DEBUGLINE(s,x) Serial.print(s); Serial.print(" = "); Serial.println(x)
#else
#define DEBUG(x)
#define DEBUGLN(x)
#endif

#ifndef RDM_SERIAL
#include <rdm6300.h>              // Библиотека RFID модуля RDM6300
Rdm6300 rdm6300;                  // Обьект RDM6300
#endif

#define MAX_TAGS             10   // Максимальное количество хранимых меток - ключей
//------------------------------------------------------------------------------------
#define BUZZER_PIN           D7   // Пин баззера
#define RDM_PIN              D5   // Пин SDA MFRC522
#define BTN_PIN              D6   // Пин кнопки
#define IS_OPEN_PIN          D2   // Пин концевика двери, подтянут к VCC
#define LOCK_PIN             D8   // Пин замка
//------------------------------------------------------------------------------------
#define EE_TAG_LENGTH         4   // Число байт на одну метку
#define EE_START_ADDR         0   // Адрес метки первого запуска в EEPROM
#define EE_KEY              100   // Ключ метки запуска в EEPROM
#define EE_TAGS_NUM_ADDR      1   // Адрес числа меток в  EEPROM
#define EE_FIRST_LABEL_ADDR  44   // Адрес названий меток в EEPROM
#define EE_FIRST_TAG_ADDR     4   // Адрес мастер-метки в EEPROM
//------------------------------------------------------------------------------------
BUZZER buz(BUZZER_PIN);

Melody         OPEN = {{690, 890,   0}, {150, 150,   0}, 1, "OPEN"};
Melody       CLOSED1 = {{890, 690,   0}, {150, 150,   0}, 1, "CLOSED"};
Melody      DECLINE = {{100,   0,   0}, {500,   0,   0}, 1, "DECLINE"};
Melody        FAULT = {{100,   0,   0}, {100,  30,   0}, 4, "FAULT"};
Melody      PROG_ON = {{490, 690, 890}, {150, 150, 150}, 1, "PROG_ON"};
Melody     PROG_OFF = {{890, 490, 690}, {150, 150, 150}, 1, "PROG_OFF"};
Melody       MASTER = {{890,   0,   0}, {200,   0,   0}, 1, "MASTER"};
Melody        SAVED = {{890,   0,   0}, {100,  30,   0}, 2, "SAVED"};
Melody      DELETED = {{890,   0,   0}, {100,  30,   0}, 3, "DELETED"};
Melody BREAKTHROUGH = {{690, 200,   0}, {300, 300,   0}, 3, "BREAKTHROUGH"};
//------------------------------------------------------------------------------------
EncButton<EB_TICK, BTN_PIN> btn;
const uint32_t EE_MASTER_TAG = 0x11121314;  // Мастер-метка
//------------------------------------------------------------------------------------
bool locked = true;       // Флаг состояния замка
uint8_t savedTags = 0;    // кол-во записанных меток
//------------------------------------------------------------------------------------
Timer unlockTick;
bool needUnlock = true;
//------------------------------------------------------------------------------------
void setup() {
#ifdef DEBUG_EN | RDM_SERIAL
  Serial.begin(9600);
#endif

  // Настраиваем пины
  //pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(IS_OPEN_PIN, INPUT_PULLUP);
  pinMode(LOCK_PIN, OUTPUT);
  InitEEPROM();           // Инициализация и (при необходимости) форматирование EEPROM

#ifndef RDM_SERIAL
  rdm6300.begin(RDM_PIN);
#endif

  // Начальное состояние замка
  if (isOpen()) {         // И дверь сейчас открыта
    unlock();             // На всякий случай дернем замок
    delay(100);
    buz.play(OPEN);
  }
  else {                  // Метки есть, но дверь закрыта
    locked = true;        // Замок закрыт
//    String str = makeStr();
//    sendUDP(str);
    buz.play(CLOSED1);
  }
#ifdef UDP_USE
  connectUDP();
#endif
}
//------------------------------------------------------------------------------------
void loop() {
  btn.tick();
  buz.tick();
  // Если дверь открыли "незаконно"
  if (locked and isOpen() and !unlockTick.enabled()) {
    buz.play(BREAKTHROUGH);
    unlock();                            // Дёрнем на всякий случай, вдруг концевик сломался

  }
  //---------------------------------------
  // Проверка на закрытие двери по концевику
  if (!locked and !isOpen()) {              // Если дверь закрыли
    locked = true;                          // Замок закрыт
//    String str = makeStr();
//    sendUDP(str);
    buz.play(CLOSED1);
  }
  //---------------------------------------
  // Открытие по нажатию кнопки изнутри
  if (locked and btn.click() and !unlockTick.enabled()) {          // Если дверь закрыта и нажали кнопку
    unlock();                                                       // Разблокируем замок
    buz.play(OPEN);
  }
  //---------------------------------------
  progModeHandler();      // Обработчик режима программирования
  unlockHandler();        // Обработчик замка
  rfHandler();            // Обработчик RFID



  delay(1);
}
