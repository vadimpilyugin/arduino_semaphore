class LedCounter {
  static const int N_PINS = 8;
  static const int N_DIGITS = 10;
  static const int START_PIN = 2;
public:
  static const boolean ALL[N_PINS];
  static const boolean NONE[N_PINS];
  static const boolean MINUS[N_PINS];
  static const boolean DIGITS[N_DIGITS][N_PINS];
  static void printSymbol(const boolean sym[N_PINS], int t = 0) {
    for (int i = 0; i < N_PINS; i++) {
      digitalWrite(i+START_PIN, sym[i]);
    }
    if (t > 0) {
      delay(t);
      printSymbol(NONE);
    }
  }
  static void printDigit(const int d, int t = 0) {
    printSymbol(DIGITS[d], t);
  }
  static void init() {
    for (int i = 0; i < N_DIGITS; i++) {
      pinMode(i+START_PIN, OUTPUT);
    }
    for (int i = 0; i < N_DIGITS; i++) {
      printDigit(i, 150);
    }
    printDigit(9, 300);
    printSymbol(NONE);
  }
};

const boolean LedCounter::ALL[N_PINS] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
const boolean LedCounter::NONE[N_PINS] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};
const boolean LedCounter::MINUS[N_PINS] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,HIGH};
const boolean LedCounter::DIGITS[N_DIGITS][N_PINS] = {
  {HIGH,HIGH,HIGH,LOW,HIGH,HIGH,HIGH,LOW},
  {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW},
  {HIGH,HIGH,LOW,LOW,HIGH,HIGH,LOW,HIGH},
  {LOW,HIGH,HIGH,LOW,HIGH,HIGH,LOW,HIGH},
  {LOW,LOW,HIGH,LOW,HIGH,LOW,HIGH,HIGH},
  {LOW,HIGH,HIGH,LOW,LOW,HIGH,HIGH,HIGH},
  {HIGH,HIGH,HIGH,LOW,LOW,HIGH,HIGH,HIGH},
  {LOW,LOW,HIGH,LOW,HIGH,HIGH,HIGH,LOW},
  {HIGH,HIGH,HIGH,LOW,HIGH,HIGH,HIGH,HIGH},
  {LOW,HIGH,HIGH,LOW,HIGH,HIGH,HIGH,HIGH},
};

class Semaphore {
  static const int N_LEDS = 3;
  static const int LED_PINS[N_LEDS];
  static void lightLeds(const boolean leds[N_LEDS]) {
    for (int i = 0; i < N_LEDS; i++) {
      digitalWrite(LED_PINS[i], leds[i]);
    }
  }
public:
  static void redLight() {
    const boolean leds[N_LEDS] = {HIGH,LOW,LOW};
    lightLeds(leds);
  }
  static void yellowLight() {
    const boolean leds[N_LEDS] = {LOW,HIGH,LOW};
    lightLeds(leds);
  }
  static void greenLight() {
    const boolean leds[N_LEDS] = {LOW,LOW,HIGH};
    lightLeds(leds);
  }
  static void firstTwo() {
    const boolean leds[N_LEDS] = {HIGH,HIGH,LOW};
    lightLeds(leds);
  }
  static void noLight() {
    const boolean leds[N_LEDS] = {LOW,LOW,LOW};
    lightLeds(leds);
  }
  static void allLight() {
    const boolean leds[N_LEDS] = {HIGH,HIGH,HIGH};
    lightLeds(leds);
  }
  static void init() {
    for (int i = 0; i < N_LEDS; i++) {
      pinMode(LED_PINS[i],OUTPUT);
    }
    int delay_time = 500;
    redLight();
    delay(delay_time);
    yellowLight();
    delay(delay_time);
    greenLight();
    delay(delay_time);
    allLight();
    delay(delay_time);
    noLight();
  }
};

const int Semaphore::LED_PINS[N_LEDS] = {11,12,13};
const int BUZZER_PIN = 10;
const int FREQ = 3500;
const int SOUND_TIME = 100;

class StateMachine {
  enum class State {
    RED_LIGHT,
    YELLOW_LIGHT,
    GREEN_LIGHT,
  };
  static const int SECOND = 1000;
  static const int RED_TIME = 9*SECOND;
  static const int YELLOW_TIME = 2*SECOND;
  static const int GREEN_TIME = 9*SECOND;
  static const int BLINK_TIME = 5*SECOND;
  static int t;
  static State state;
public:
  static void init() {
    t = RED_TIME;
    Semaphore::redLight();
    state = State::RED_LIGHT;
  }
  static void loop() {
    switch(state) {
      case State::RED_LIGHT:
        if (t > 0) {
          LedCounter::printDigit(t/SECOND, SECOND);
          t -= SECOND;
        } else if (t == 0) {
          t = YELLOW_TIME;
          Semaphore::firstTwo();
          state = State::YELLOW_LIGHT;
        } else {
          Semaphore::allLight();
          exit(1);
        }
        break;
      case State::YELLOW_LIGHT:
        if (t == 1) {
          t = RED_TIME;
          Semaphore::redLight();
          state = State::RED_LIGHT;
        } else if (t > 0) {
          LedCounter::printSymbol(LedCounter::NONE);
          delay(YELLOW_TIME);
          t -= YELLOW_TIME; // при переходе от зеленого останется единица
        } else if (t == 0) {
          t = GREEN_TIME;
          Semaphore::greenLight();
          state = State::GREEN_LIGHT;
        } else {
          Semaphore::allLight();
          exit(1);
        }
        break;
      case State::GREEN_LIGHT:
        if (t == BLINK_TIME) {
          LedCounter::printDigit(t/SECOND);
          tone(BUZZER_PIN, FREQ, SOUND_TIME);
          delay(SECOND/4-SOUND_TIME);
          LedCounter::printSymbol(LedCounter::NONE);
          tone(BUZZER_PIN, FREQ, SOUND_TIME);
          delay(SECOND/4-SOUND_TIME);
          LedCounter::printDigit(t/SECOND);
          tone(BUZZER_PIN, FREQ, SOUND_TIME);
          delay(SECOND/4-SOUND_TIME);
          LedCounter::printSymbol(LedCounter::NONE);
          tone(BUZZER_PIN, FREQ, SOUND_TIME);
          delay(SECOND/4-SOUND_TIME);
          delay(SECOND/4); // чтобы дать время между этим tone и следующим 
          t -= SECOND;
        }
        else if (t > 0 && t < BLINK_TIME) {
          LedCounter::printDigit(t/SECOND);
          tone(BUZZER_PIN, FREQ, SOUND_TIME);
          delay(SECOND/4-SOUND_TIME);
          LedCounter::printSymbol(LedCounter::NONE, SECOND/4);
          LedCounter::printDigit(t/SECOND, SECOND/4);
          LedCounter::printSymbol(LedCounter::NONE, SECOND/4);
          t -= SECOND;
        } else if (t > 0) {
          LedCounter::printDigit(t/SECOND);
          tone(BUZZER_PIN, FREQ, SOUND_TIME);
          delay(SECOND-SOUND_TIME);
          t -= SECOND;
        } else if (t == 0) {
          t = YELLOW_TIME+1;
          Semaphore::yellowLight();
          state = State::YELLOW_LIGHT;
        } else {
          Semaphore::allLight();
          exit(1);
        }
        break;
    }
  }
};

int StateMachine::t;
StateMachine::State StateMachine::state;

void setup() {
  // put your setup code here, to run once:
  LedCounter::init();
  Semaphore::init();
  StateMachine::init();
  pinMode(BUZZER_PIN, OUTPUT);
  tone(BUZZER_PIN, FREQ, SOUND_TIME);
}

void loop() {
  // put your main code here, to run repeatedly:
  StateMachine::loop();
}
