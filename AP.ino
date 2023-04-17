#ifdef UDP_USE
void connectUDP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed");
    while (1) {
      delay(1000);
    }
  }
  if (udp.listen(port)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet) {
      
      Serial.print(", Length: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();

      uint8_t *req = packet.data();
      uint8_t cmnd = req[0];
      DEBUGLINE("Command = ", cmnd);
      if (cmnd == 117) unlock();
      if (cmnd ==100) {
        uint8_t substr = req + 1;
        int  n;
        n = atoi(substr);
        DEBUGLINE("Delete tag #", req[3]);
        
      }
      
      //reply to the client
      String str = makeStr();
      const char *str2 = str.c_str();
      packet.printf(str2);
    });
  }
}

void sendUDP(String str) {
  int str_len = str.length() + 1;
  char  udpString[str_len];
  str.toCharArray(udpString, str_len);
  AsyncUDPMessage msg( str_len );
  msg.write((uint8_t*)udpString, str_len );
  udp.sendTo(msg, addr, port);
  DEBUGLN(str);

}

String makeStr(void) {
  String str;
  str += (locked ? "1" : "0");
  str += ",";
  str += savedTags;
  for (uint8_t i = 0; i < savedTags; i++) {
    uint32_t tag;
    EEPROM.get(EE_FIRST_TAG_ADDR + i * EE_TAG_LENGTH, tag);
    str += ",";
    str += tag;
  }
  return str;
}

#endif
