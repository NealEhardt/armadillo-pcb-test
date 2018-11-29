// Armadillo test code

const int SHIFT_CLOCK_PIN = 26; // white
const int SHIFT_DATA_PIN = 28; // green
const int LATCH_PIN = 30; // yellow
const int MODE_PIN = 32; // blue

const int REGISTER_COUNT = 48;

void setup() {
  Serial.begin(9600);
  Serial.println("TESTING TIME ^_^");
  
  pinMode(SHIFT_CLOCK_PIN, OUTPUT);
  pinMode(SHIFT_DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(MODE_PIN, OUTPUT);
}

void loop() {
  const int UPDATE_INTERVAL = 1000; // milliseconds
  static unsigned long lastUpdate = millis();
  
  unsigned long t = millis();
  if (t - lastUpdate > UPDATE_INTERVAL) {
    byte brightness = (t / 37) % 0x80;
    
    setBrightness(brightness);
    lastUpdate += UPDATE_INTERVAL;
  }
  
  drawSnakeFrame(t / 15);
}

void setBrightness(byte level) {
  level &= 0x7F;
  Serial.print("brightness ");
  Serial.print(level * 100 / 0x7F);
  Serial.println("%");
    
  byte dc_bytes[14] = {0};
  for (int i = 0; i < sizeof(dc_bytes); i++) {
    for (int j = 0; j < 8; j++) {
      byte k = i * 8 + j;
      bool b = (level >> (6 - (k % 7))) & 1;
      dc_bytes[i] += b << (7 - j);
    }
  }
  
  digitalWrite(MODE_PIN, HIGH); // 112-bit Dot Correction Mode
  digitalWrite(LATCH_PIN, LOW);

  for (int i = 0; i < REGISTER_COUNT; i++) {
    for (int j = 0; j < sizeof(dc_bytes); j++) {
      shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, dc_bytes[j]);
    }
  }

  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(MODE_PIN, LOW); // 16-bit On/Off Mode
}

void drawSnakeFrame(unsigned long phase) {
  const int SNAKE_LENGTH = 12;
  const int GAP_LENGTH = 9;
  const int PERIOD = SNAKE_LENGTH + GAP_LENGTH;
  byte message[REGISTER_COUNT * 2] = {0};
  phase %= PERIOD;
  for (int i = 0; i < sizeof(message); i++) {
    byte b = 0;
    for (int j = 0; j < 8; j++) {
      b = (b << 1) | (phase < SNAKE_LENGTH);
      if (++phase >= PERIOD) { phase = 0; }
    }
    message[i] = b;
  }
  
  digitalWrite(LATCH_PIN, LOW);
  for (int i = 0; i < sizeof(message); i++) {
    shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, message[i]);
  }
  digitalWrite(LATCH_PIN, HIGH);
}
