// Звуковой сигнал

void buzzerON(uint8_t signal) {
  SIG = signal;
  buzzCounter = 1;
  buzTick.setPeriod(50);
}

void buzzerHandler(uint8_t signal) {
  if (buzTick.ready()) {
    switch (signal) { // Выбираем сигнал
      case NOT_NEED:
        return;

      case OPEN:
        switch (buzzCounter) {
          case 1:
            tone(BUZZER_PIN, 690, 150);
            buzzCounter++;
            buzTick.setPeriod(150);
            return;
          case 2:
            tone(BUZZER_PIN, 890, 150);
            buzTick.setPeriod(0);
            DEBUGLN("OPENING");
            return;
        }
        return;

      case CLOSED:
        switch (buzzCounter) {
          case 1:
            tone(BUZZER_PIN, 890, 150);
            buzzCounter++;
            buzTick.setPeriod(150);
            return;
          case 2:
            tone(BUZZER_PIN, 690, 150);
            buzTick.setPeriod(0);
            DEBUGLN("CLOSED");
            return;
        }
        return;

      case DECLINE:
        tone(BUZZER_PIN, 100, 500);
        buzTick.setPeriod(0);
        DEBUGLN("DECLINE");
        return;

      case FAULT:
        tone(BUZZER_PIN, 100, 100);
        buzTick.setPeriod(130);
        buzzCounter++;
        if (buzzCounter == 6) {
          buzTick.setPeriod(0);
          DEBUGLN("FAULT");
        }
        return;

      case PROG_ON:
        switch (buzzCounter) {
          case 1:
            tone(BUZZER_PIN, 490, 150);
            buzzCounter++;
            buzTick.setPeriod(150);
            return;
          case 2:
            tone(BUZZER_PIN, 690, 150);
            buzzCounter++;
            buzTick.setPeriod(150);
            return;
          case 3:
            tone(BUZZER_PIN, 890, 150);
            buzTick.setPeriod(0);
            DEBUGLN("PROG ON");
            return;
        }
        return;

      case PROG_OFF:
        switch (buzzCounter) {
          case 1:
            tone(BUZZER_PIN, 890, 150);
            buzzCounter++;
            buzTick.setPeriod(150);
            return;
          case 2:
            tone(BUZZER_PIN, 690, 150);
            buzzCounter++;
            buzTick.setPeriod(150);
            return;
          case 3:
            tone(BUZZER_PIN, 490, 150);
            buzTick.setPeriod(0);
            DEBUGLN("PROG OFF");
            return;
        }
        return;

      case MASTER:
        tone(BUZZER_PIN, 890, 100);
        buzTick.setPeriod(0);
        DEBUGLN("MASTER MODE");
        return;

      case SAVED:
        tone(BUZZER_PIN, 890, 100);
        buzTick.setPeriod(130);
        buzzCounter++;
        if (buzzCounter == 3) {
          buzTick.setPeriod(0);
          DEBUGLN("SAVED");
        }
        return;

      case DELETED:
        tone(BUZZER_PIN, 890, 100);
        buzTick.setPeriod(130);
        buzzCounter++;
        if (buzzCounter == 4) {
          buzTick.setPeriod(0);
          DEBUGLN("DELETED");
        }
        return;

      case BREAKTHROUGH:
        switch (buzzCounter) {
          case 1:
            tone(BUZZER_PIN, 690, 300);
            buzzCounter++;
            buzTick.setPeriod(300);
            DEBUGLN("BREAKTHROUGH");
            return;
          case 2: case 4:
            tone(BUZZER_PIN, 200, 300);
            buzzCounter++;
            buzTick.setPeriod(300);
            return;
          case 5: case 3:
            tone(BUZZER_PIN, 690, 300);
            buzzCounter++;
            buzTick.setPeriod(300);
            return;
          case 6:
            tone(BUZZER_PIN, 200, 300);
            buzTick.setPeriod(0);
            return;
        }
        return;
    }
  }
}





/*
  void buzz(uint8_t signal) {
  switch (signal) { // Выбираем сигнал
    case OPEN:
      tone(BUZZER_PIN, 690, 150);
      delay(150);
      tone(BUZZER_PIN, 890, 150);
      return;
    case CLOSED:
      DEBUGLN("CLOSED");
      tone(BUZZER_PIN, 890, 150);
      delay(150);
      tone(BUZZER_PIN, 690, 150);
      return;
    case DECLINE:
      DEBUGLN("DECLINE");
      tone(BUZZER_PIN, 100, 300);
      return;
    case FAULT:
      DEBUGLN("ERROR");
      for (uint8_t i = 0; i < 3; i++) {
        tone(BUZZER_PIN, 100, 100);
        delay(130);
      }
      return;
    case PROG:
      DEBUGLN("PROG");
      tone(BUZZER_PIN, 890, 300);
      return;
    case SAVED:
      DEBUGLN("SAVED");
      for (uint8_t i = 0; i < 2; i++) {
        tone(BUZZER_PIN, 890, 100);
        delay(130);
      }
      return;
    case DELETED:
      DEBUGLN("DELETED");
      for (uint8_t i = 0; i < 3; i++) {
        tone(BUZZER_PIN, 890, 100);
        delay(130);
      }
      return;
    case BREAKTHROUGH:
      DEBUGLN("BREAKTHROUGH");
      for (uint8_t i = 0; i < 2; i++) {
        tone(BUZZER_PIN, 690, 500);
        delay(500);
        tone(BUZZER_PIN, 200, 500);
        delay(500);
      }
      return;
  }
  }
*/
