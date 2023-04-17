class Timer {
  public:
    Timer(uint32_t nprd = 0) {
      setPeriod(nprd);
    }
    void setPeriod(uint32_t nprd) {
      prd = nprd;
    }
    bool enabled() {
      return (prd > 0) ? 1 : 0;
    }
    bool ready() {
      return (prd && millis() - tmr >= prd) ? (tmr = millis(), 1) : 0;
    }
  private:
    uint32_t tmr = 0, prd = 0;
};