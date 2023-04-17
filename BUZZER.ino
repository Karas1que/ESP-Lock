struct Melody 
{
    uint16_t note[3];
    uint16_t timing[3];
    uint8_t repeat;
    String name;
};

Melody         OPEN = {{690,890,  0},{150,150,  0},1,"OPEN"};
Melody       CLOSED = {{890,690,  0},{150,150,  0},1,"CLOSED"};
Melody      DECLINE = {{100,  0,  0},{500,  0,  0},1,"DECLINE"};
Melody        FAULT = {{100,  0,  0},{100, 30,  0},4,"FAULT"};
Melody      PROG_ON = {{490,690,890},{150,150,150},1,"PROG_ON"};
Melody     PROG_OFF = {{890,490,690},{150,150,150},1,"PROG_OFF"};
Melody       MASTER = {{890,  0,  0},{200,  0,  0},1,"MASTER"};
Melody        SAVED = {{890,  0,  0},{100, 30,  0},2,"SAVED"};
Melody      DELETED = {{890,  0,  0},{100, 30,  0},3,"DELETED"};
Melody BREAKTHROUGH = {{690,200,  0},{300,300,  0},3,"BREAKTHROUGH"};

class BUZZER {
    public:
    BUZZER(byte pin) {
        _pin = pin;
        pinMode(_pin, OUTPUT);
    }

    void play(Melody melody) {
        _melody = melody;
        _flag = true;
        _pos = 1;
        _tmr = 0;
        Serial.println(_melody.name);
    }

    void tick() {
        if (_flag) {
            if (millis() - _tmr >= _melody.timing[_pos]) {
                tone(_pin,
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
    bool _flag;
    uint8_t _pos;
};

BUZZER buz(4);

void setup() {

}
void loop() {
buz.tick();
if (false) buz.play(OPEN);
}