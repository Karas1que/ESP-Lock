
Timer rfidTimeout(1000);                                                        // Таймаут рфид
Timer prog;

bool progMode = false;
bool masterFlag = false;

void progModeHandler(void) {
  switch (progMode) {
    case 0:
      if (!locked and btn.held()) {          // Если откыта дверь, режим программирования отключен и нажата кнопка
        buz.play(PROG_ON);
        progMode = true;                                      //
        prog.setPeriod(20 * 1000);                            // Включаем режим программирования
        DEBUGLN("Waiting for tag...");                        //
      }
      break;
    case 1:
      if (prog.ready() or !prog.enabled() or locked)       // Отключаем режим программирования если таймер или закрыли дверь
      {
        buz.play(PROG_OFF);
        progMode = false;
        masterFlag = false;
        prog.setPeriod(0);
      }
      break;
  }
}

// Поднесение метки
void rfHandler(void)  {
  if (rfidTimeout.ready()) {
    switch (progMode) {
      case 0: {                                                                             // Режим открытия
          if (locked) {
            if (NEWTAG > 0) {                                                               // Поднесена карта
              uint32_t tag = CURRTAG;                                                       // Буфер метки
              CLRSERIAL;
              int8_t tagpos = foundTag(tag);                                                // Ищем метку в базе
              DEBUGLINE("Readed tag = ", tag);
              DEBUGLINE("Position in storage = ", tagpos);
              if (tagpos > 0) {                                                             // И если метка есть в базе
                buz.play(OPEN);                                                             // Подаем сигнал успеха
                unlock();                                                                   // Разблокируем
              }
              else buz.play(DECLINE);                                                       // Если метки в базе нет - выдаем отказ
            }
          }
        }
        break;
      case 1: {
          if (NEWTAG > 0) {                                                                 // Если появилась новая метка
            uint32_t tag = CURRTAG;                                                         // Записываем её в буфер
            CLRSERIAL;
            int8_t tagpos = foundTag(tag);                                                  // Ищем метку в базе
            if (tagpos == 1) {
              masterFlag = true;                                             // Если поднесена мастер-метка ждем новую для записи
              buz.play(MASTER);
            }
            if (tagpos == -1) {
              if (masterFlag) {
                saveTag(tag);                         // Если поднесена мастер-метка, а потом новая - записываем
                readTags();
                prog.setPeriod(20 * 1000);
              }
              else {
                buz.play(DECLINE);
              }
            }
            if (tagpos > 1) {
              deleteTag(tagpos);
              readTags();
              prog.setPeriod(20 * 1000);
            }
          }
        }
        break;
    }
  }
}
