struct Melody
{
  uint16_t note[3];
  uint16_t timing[3];
  uint8_t repeat;
  String name;
};

class BUZZER {
  public:
    BUZZER(byte pin) {
      _pin = pin;
      pinMode(_pin, OUTPUT);
    }

    void play(Melody melody) {
      _melody = melody;
      _flag = true;
      _pos = 0;
      _tmr = 0;
      Serial.println(_melody.name);
    }

    void tick() {
      if (_flag) {
        if (millis() - _tmr >= _melody.timing[_pos]) {
          if (_melody.note[_pos] > 0) tone(_pin,
                                             _melody.note[_pos],
                                             _melody.timing[_pos]);
          if (++_pos == 3) {
            _pos = 0;
            if (--_melody.repeat == 0) _flag = false;
            else _tmr = millis();
          } else _tmr = millis();
        }
      }
    }

  private:
    byte _pin;
    uint32_t _tmr;
    Melody _melody;
    bool _flag = false;
    uint8_t _pos;
};
