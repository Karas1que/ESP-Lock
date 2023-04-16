// Функция проверки на открытую дверь по концевику
bool isOpen(void) {
  return digitalRead(IS_OPEN_PIN);  // Если дверь открыта - концевик размокнут, на пине HIGH
}
//------------------------------------------------------------------------------------
void unlockHandler(void) {
  if (unlockTick.ready() or (unlockTick.enabled() and isOpen())) {
    switch (needUnlock) {

      case 1:
        needUnlock = false;
        digitalWrite(LOCK_PIN, HIGH);       // Дёргаем замок на время
        unlockTick.setPeriod(5000);
        return;

      case 0:
        digitalWrite(LOCK_PIN, LOW);
        if (isOpen()) {
          locked = false;                   // Замок открыт
          String str = makeStr();
          sendUDP(str);
        }
        else {                              // Если концевик показывает что замок до сих пор закрыт - даём сигнал ошибки
          buzzerON(FAULT);

        }
        unlockTick.setPeriod(0);
        return;
    }
  }
}


// Функция открывания замка
void unlock(void) {
  unlockTick.setPeriod(50);
  needUnlock = true;
}
