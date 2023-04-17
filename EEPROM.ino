void InitEEPROM(void) {
  EEPROM.begin(MAX_TAGS * EE_TAG_LENGTH + EE_FIRST_TAG_ADDR);
  delay(100);
  DEBUGLN("\n");
  DEBUGLN("POWER ON");
  DEBUGLN("Reading EEPROM...");
  DEBUG("First launch key ");
  DEBUGLN(EEPROM.read(EE_START_ADDR) == EE_KEY ? "valid" : "invalid");
  if (EEPROM.read(EE_START_ADDR) != EE_KEY) { // при первом включении или необходимости очистки ключей
    DEBUGLN("Formatting EEPROM...");
    for (uint8_t i = 0; i < EEPROM.length(); i++) EEPROM.write(i, 0x00); // Чистим всю EEPROM
    EEPROM.write(EE_START_ADDR, EE_KEY);                   // Пишем байт-ключ
    EEPROM.write(EE_TAGS_NUM_ADDR, 1);                     // Пишем число ключей (1 мастер-ключ)
    EEPROM.put(EE_FIRST_TAG_ADDR, EE_MASTER_TAG);          // Пишем мастер-ключ
    EEPROM.commit();
    DEBUGLN("EEPROM formatted!");
    DEBUGLN("Rebooting...");
    delay(100);
    ESP.restart();
  } else readTags(); // Читаем память
}

void readTags(void) {
  savedTags = EEPROM.read(EE_TAGS_NUM_ADDR);             // Читаем кол-во меток в памяти
  DEBUG("Tags in EEPROM: ");
  DEBUGLN(savedTags);
  for (uint8_t i = 0; i < savedTags; i++) {              // Выводим метки в serial
    uint32_t tag;
    EEPROM.get(EE_FIRST_TAG_ADDR + i * EE_TAG_LENGTH, tag);
    DEBUG("Tag #");
    DEBUG(i + 1);
    DEBUG("=");
    DEBUGLN(tag);
  }
}

// Поиск метки в EEPROM
int8_t foundTag(uint32_t tag) {
  uint32_t buf;   // Буфер метки
  uint8_t address; // Адрес
  for (uint8_t i = 0; i < savedTags; i++) {                   // проходим по всем меткам
    address = EE_FIRST_TAG_ADDR + (i * EE_TAG_LENGTH);        // Считаем адрес текущей метки
    EEPROM.get(address, buf);                                 // Читаем метку из памяти
    if (tag == buf) return (i + 1);                           // Сравниваем - если нашли возвращаем номер метки
  }
  return -1;                                                  // Если не нашли - вернем минус 1
}

void saveTag(uint32_t tag) {
  uint8_t newTagAddr = EE_FIRST_TAG_ADDR + savedTags * EE_TAG_LENGTH;           // Адрес для новой метки в EEPROM
  if (savedTags < MAX_TAGS) {                                                   // Если лимит не достигнут
    EEPROM.put(newTagAddr, tag);                                                // Пишем новую метку
    EEPROM.write(EE_TAGS_NUM_ADDR, ++savedTags);                                // Увеличиваем кол-во меток и пишем в EEPROM
    EEPROM.commit();
    buz.play(SAVED);                                                                // Подаем сигнал успеха
//    String str = makeStr();
//    sendUDP(str);
  }
  else {                                                                        // Лимит меток при попытке записи новой
    buz.play(DECLINE);                                                              // Выдаем отказ
  }
}

void deleteTag(uint8_t tagpos) {
  if (tagpos < savedTags) {                                                    // Если удаляемая метка не в конце
    uint32_t buf;                                                               // Буфер метки
    uint8_t lastTagAddr = EE_FIRST_TAG_ADDR + EE_TAG_LENGTH * (savedTags - 1);  // Адрес начала крайней метки в EEPROM
    EEPROM.get(lastTagAddr, buf);                                               // Читаем крайнюю метку из памяти
    EEPROM.put(EE_FIRST_TAG_ADDR + EE_TAG_LENGTH * (tagpos - 1), buf);          // Переписываем её на место удаляемой
  }
  EEPROM.write(EE_TAGS_NUM_ADDR, --savedTags);                                  // Уменьшаем кол-во меток и пишем в EEPROM
  EEPROM.commit();
  buz.play(DELETED);                                                            // Подаем сигнал
//  String str = makeStr();
//  sendUDP(str);
}
