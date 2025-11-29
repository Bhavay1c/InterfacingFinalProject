// ============================================
// CST8227 FINAL PROJECT - MEGA-2
// Multi-Sensor Monitoring and Control System
// ============================================
// This is the complete MEGA-2 implementation for the final project
// Features:
// - PIR motion detection with yellow LEDs
// - DHT11 temperature/humidity sensing
// - Fan control based on temperature
// - Ultrasonic safety system with buzzer
// - Night mode with photoresistor
// - 7-segment display for fan runtime
// - I2C communication with MEGA-1
// - Node-RED compatible serial output

#include <LiquidCrystal.h>
#include <DHT.h>
#include <Wire.h>

// ---------------------------
// PIN DEFINITIONS
// ---------------------------
#define PIR_PIN 2
#define DHT_PIN 3
#define TRIG_PIN 4
#define ECHO_PIN 5
#define FAN_ENABLE 6   // L293D Enable 1,2 pin
#define FAN_INPUT1 7   // L293D Input 1 pin
#define FAN_INPUT2 9   // L293D Input 2 pin
#define BUZZER_PIN 34
#define PHOTO_PIN A0

// I2C Configuration
#define I2C_ADDRESS 8  // MEGA-2 I2C address
#define I2C_CMD_READY 100
#define I2C_CMD_SET_TEMP 101

// Shift register (LEDs)
int dataPin  = 8;
int latchPin = 11;
int clockPin = 12;

// LCD MEGA-2
LiquidCrystal lcd(42, 40, 44, 46, 48, 50);

// 7-segment display pins (using working configuration)
const int segPins[8] = {22, 23, 24, 25, 26, 27, 28, 29}; // A–G, DP
const int digitPins[4] = {30, 31, 32, 33};               // D1–D4 (left→right)

DHT dht(DHT_PIN, DHT11);

// ---------------------------
// GLOBAL VARIABLES
// ---------------------------
float preferredTemp = 22.0;  // Default, will be set via I2C from MEGA-1
float tempC = 0;
float hum = 0;
long distCM = 999;

bool nightMode = false;
bool motionDetected = false;
bool fanOn = false;
bool lastMotionState = false;
bool systemActive = false;  // System starts inactive until MEGA-1 activates
bool objectTooClose = false;
bool lastObjectState = false;

unsigned long fanSeconds = 0;
unsigned long lastSecond = 0;
unsigned long fanStartTime = 0;

unsigned long lastDHT = 0;
unsigned long lastLCD = 0;
unsigned long lastUS = 0;
unsigned long lastI2C = 0;

// Buzzer song variables (Happy Birthday)
bool buzzerPlaying = false;
unsigned long buzzerStartTime = 0;
int songNoteIndex = 0;
unsigned long lastNoteTime = 0;

// Musical note frequencies (in Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_Bb4 466
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_REST 0

// Happy Birthday melody - {frequency, duration in ms}
int birthdaySong[][2] = {
  {NOTE_C4, 250}, {NOTE_C4, 125}, {NOTE_D4, 500}, {NOTE_C4, 500}, {NOTE_F4, 500}, {NOTE_E4, 1000},
  {NOTE_C4, 250}, {NOTE_C4, 125}, {NOTE_D4, 500}, {NOTE_C4, 500}, {NOTE_G4, 500}, {NOTE_F4, 1000},
  {NOTE_C4, 250}, {NOTE_C4, 125}, {NOTE_C5, 500}, {NOTE_A4, 500}, {NOTE_F4, 500}, {NOTE_E4, 500}, {NOTE_D4, 1000},
  {NOTE_Bb4, 250}, {NOTE_Bb4, 125}, {NOTE_A4, 500}, {NOTE_F4, 500}, {NOTE_G4, 500}, {NOTE_F4, 1500}
};
int songLength = sizeof(birthdaySong) / sizeof(birthdaySong[0]);

// 7-segment digit patterns (GFEDCBA + DP) - USING WORKING PATTERN
byte patterns[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

// ---------------------------
// SHIFT REGISTER LED CONTROL
// ---------------------------
void updateShiftRegister(byte data) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}

void setYellowLEDs(bool on) {
  if (on) {
    updateShiftRegister(0b00001111);  // Lower 4 bits = Yellow LEDs
  } else {
    // Only turn off if not in night mode (preserve red LEDs)
    if (!nightMode) {
      updateShiftRegister(0);
    }
  }
}

void setRedLEDs(bool on) {
  if (on) {
    updateShiftRegister(0b11110000);  // Upper 4 bits = Red LEDs
  } else {
    // Only turn off if no motion (preserve yellow LEDs)
    if (!motionDetected) {
      updateShiftRegister(0);
    }
  }
}

// ---------------------------
// FAN CONTROL (L293D)
// ---------------------------
void setFanState(bool on) {
  if (on) {
    digitalWrite(FAN_INPUT1, HIGH);
    digitalWrite(FAN_INPUT2, LOW);
    digitalWrite(FAN_ENABLE, HIGH);
    if (fanStartTime == 0) {
      fanStartTime = millis();
    }
  } else {
    digitalWrite(FAN_INPUT1, LOW);
    digitalWrite(FAN_INPUT2, LOW);
    digitalWrite(FAN_ENABLE, LOW);
    fanStartTime = 0;
  }
}

// ---------------------------
// ULTRASONIC DISTANCE SENSOR
// ---------------------------
long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;

  return (duration * 0.034) / 2;
}

// ---------------------------
// 7-SEGMENT DISPLAY (using working method)
// ---------------------------
void showSeconds(unsigned long number) {
  // Ensure number is in valid range (0-9999)
  if (number > 9999) number = 9999;

  // Display all 4 digits
  for (int d = 0; d < 4; d++) {
    // Clear all segments
    for (int s = 0; s < 8; s++) digitalWrite(segPins[s], LOW);

    // Enable current digit (LOW = ON for common cathode)
    digitalWrite(digitPins[d], LOW);

    // Get the digit value for this position
    int digit = getDigit(number, d);
    byte pattern = patterns[digit];

    // Set segments based on pattern
    for (int s = 0; s < 8; s++)
      digitalWrite(segPins[s], bitRead(pattern, s));

    delayMicroseconds(2500);
    
    // Turn off digit (HIGH = OFF)
    digitalWrite(digitPins[d], HIGH);
  }
}

// Get one digit from number (0=thousands, 1=hundreds, 2=tens, 3=ones)
int getDigit(unsigned long number, int position) {
  int divisor = 1;
  for (int i = 0; i < (3 - position); i++) divisor *= 10;
  return (number / divisor) % 10;
}

void clear7Segment() {
  // Turn off all digits
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], HIGH);
  }
  // Clear all segments
  for (int i = 0; i < 8; i++) {
    digitalWrite(segPins[i], LOW);
  }
}

// ---------------------------
// BUZZER SONG CONTROL
// ---------------------------
void playBuzzerSong() {
  if (!buzzerPlaying) {
    buzzerPlaying = true;
    buzzerStartTime = millis();
    songNoteIndex = 0;
    lastNoteTime = millis();
  }

  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - lastNoteTime;

  if (songNoteIndex < songLength) {
    if (elapsed >= birthdaySong[songNoteIndex][1]) {
      // Move to next note
      songNoteIndex++;
      lastNoteTime = currentTime;
      
      if (songNoteIndex < songLength) {
        if (birthdaySong[songNoteIndex][0] == NOTE_REST) {
          noTone(BUZZER_PIN);
        } else {
          tone(BUZZER_PIN, birthdaySong[songNoteIndex][0]);
        }
      } else {
        // Song finished
        noTone(BUZZER_PIN);
        buzzerPlaying = false;
      }
    }
  }
}

void stopBuzzerSong() {
  noTone(BUZZER_PIN);
  buzzerPlaying = false;
  songNoteIndex = 0;
}

// ---------------------------
// I2C COMMUNICATION
// ---------------------------
void receiveEvent(int howMany) {
  if (howMany >= 2) {
    byte cmd = Wire.read();
    byte data = Wire.read();

    if (cmd == I2C_CMD_SET_TEMP) {
      // Convert received byte to temperature (0-99 range, divided by 10)
      preferredTemp = data / 10.0;
      Serial.print("Received preferred temp: ");
      Serial.println(preferredTemp, 1);
    } else if (cmd == I2C_CMD_READY) {
      systemActive = (data == 1);
      Serial.print("System active: ");
      Serial.println(systemActive);
    }
  }
}

void requestEvent() {
  // Send current sensor data to MEGA-1 when requested
  // Send 5 bytes: Temperature, Humidity, Fan Status, Night Mode, Motion
  Wire.write((byte)(tempC * 10));        // Byte 0: Temperature (x10)
  Wire.write((byte)hum);                 // Byte 1: Humidity (0-100)
  Wire.write((byte)(fanOn ? 1 : 0));     // Byte 2: Fan status (0 or 1)
  Wire.write((byte)(nightMode ? 1 : 0)); // Byte 3: Night mode (0 or 1)
  Wire.write((byte)(motionDetected ? 1 : 0)); // Byte 4: Motion (0 or 1)
  
  Serial.print("→ MEGA-1: Sent data - T=");
  Serial.print(tempC, 1);
  Serial.print("C, H=");
  Serial.print(hum);
  Serial.print(", Fan=");
  Serial.print(fanOn ? "ON" : "OFF");
  Serial.println();
}

// ---------------------------
// SETUP
// ---------------------------
void setup() {
  Serial.begin(9600);

  // Initialize I2C as slave
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // Initialize pins
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(FAN_ENABLE, OUTPUT);
  pinMode(FAN_INPUT1, OUTPUT);
  pinMode(FAN_INPUT2, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PHOTO_PIN, INPUT);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Initialize 7-segment pins (using working configuration)
  for (int i = 0; i < 8; i++) pinMode(segPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH); // all digits off initially
  }

  // Initialize all outputs
  setFanState(false);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(TRIG_PIN, LOW);
  updateShiftRegister(0);

  lcd.begin(16, 2);
  lcd.print("MEGA-2 Ready");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");

  dht.begin();

  Serial.println("=====================================");
  Serial.println("CST8227 FINAL PROJECT - MEGA-2");
  Serial.println("=====================================");
  Serial.println("Waiting for MEGA-1 to activate...");
  Serial.println();

  delay(1000);
}

// ---------------------------
// MAIN LOOP
// ---------------------------
void loop() {
  if (!systemActive) {
    // Wait for system activation from MEGA-1
    lcd.setCursor(0, 0);
    lcd.print("Waiting for     ");
    lcd.setCursor(0, 1);
    lcd.print("MEGA-1...      ");
    delay(100);
    return;
  }

  // -------------------------
  // NIGHT MODE CHECK
  // -------------------------
  bool newNightMode = (analogRead(PHOTO_PIN) < 400);
  if (newNightMode != nightMode) {
    nightMode = newNightMode;
    if (nightMode) {
      // Night mode: use 22°C default
      preferredTemp = 22.0;
    }
    setRedLEDs(nightMode);
  }

  // -------------------------
  // PIR MOTION DETECTION
  // -------------------------
  motionDetected = digitalRead(PIR_PIN);

  if (motionDetected && !lastMotionState) {
    // Motion just detected
    setYellowLEDs(true);
  } else if (!motionDetected && lastMotionState) {
    // Motion stopped
    setYellowLEDs(false);
  }
  lastMotionState = motionDetected;

  // -------------------------
  // DHT11 TEMPERATURE/HUMIDITY READ
  // -------------------------
  if (millis() - lastDHT >= 2000) {
    lastDHT = millis();
    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();

    if (!isnan(newTemp)) {
      tempC = newTemp;
    }
    if (!isnan(newHum)) {
      hum = newHum;
    }
  }

  // -------------------------
  // ULTRASONIC SAFETY CHECK
  // -------------------------
  if (millis() - lastUS >= 100) {
    lastUS = millis();
    distCM = readDistance();
    objectTooClose = (distCM < 20);
  }

  // -------------------------
  // FAN CONTROL LOGIC
  // -------------------------
  // Fan should be on if:
  // 1. Motion was detected (once motion detected, fan can run)
  // 2. Temperature is above preferred temp
  // 3. Object is not too close (safety)
  // 4. System is active
  static bool motionTriggeredFan = false;
  
  if (motionDetected) {
    motionTriggeredFan = true;
  }
  
  // Determine if fan should be on
  bool shouldFanBeOn = false;
  if (motionTriggeredFan && tempC > preferredTemp && !objectTooClose && systemActive) {
    shouldFanBeOn = true;
  } else if (tempC <= preferredTemp) {
    // Fan stops when temperature reaches preferred temp
    motionTriggeredFan = false;
    shouldFanBeOn = false;
  }

  // Handle object proximity safety
  if (objectTooClose && !lastObjectState) {
    // Object just got too close - emergency stop
    setFanState(false);
    playBuzzerSong();
  } else if (!objectTooClose && lastObjectState) {
    // Object moved away - can resume fan
    stopBuzzerSong();
    setFanState(shouldFanBeOn);
  } else if (objectTooClose) {
    // Object still too close - keep fan off and play song
    setFanState(false);
    playBuzzerSong();
  } else {
    // Safe distance - control fan normally
    setFanState(shouldFanBeOn);
  }

  lastObjectState = objectTooClose;
  fanOn = shouldFanBeOn && !objectTooClose;

  // -------------------------
  // COUNT FAN ON TIME
  // -------------------------
  if (fanOn) {
    if (millis() - lastSecond >= 1000) {
      lastSecond = millis();
      fanSeconds++;
      // Prevent overflow (reset after 9999 seconds)
      if (fanSeconds > 9999) {
        fanSeconds = 0;
      }
    }
  }

  // -------------------------
  // LCD DISPLAY UPDATE
  // -------------------------
  if (millis() - lastLCD >= 500) {
    lastLCD = millis();

    lcd.clear();
    lcd.setCursor(0, 0);

    if (nightMode) {
      lcd.print("Night Mode");
      lcd.setCursor(0, 1);
      lcd.print("T:22.0C");
      lcd.print(" H:");
      lcd.print(hum, 0);
    } else {
      lcd.print("T:");
      lcd.print(tempC, 1);
      lcd.print("C H:");
      lcd.print(hum, 0);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print("Pref:");
      lcd.print(preferredTemp, 1);
      lcd.print("C Fan:");
      lcd.print(fanOn ? "ON" : "OFF");
    }
  }

  // -------------------------
  // SERIAL OUTPUT FOR NODE-RED
  // -------------------------
  static unsigned long lastSerial = 0;
  if (millis() - lastSerial >= 1000) {
    lastSerial = millis();

    // Format: JSON-like output for Node-RED
    Serial.print("{\"temp\":");
    Serial.print(tempC, 1);
    Serial.print(",\"humidity\":");
    Serial.print(hum, 0);
    Serial.print(",\"fan\":\"");
    Serial.print(fanOn ? "ON" : "OFF");
    Serial.print("\",\"nightMode\":\"");
    Serial.print(nightMode ? "ACTIVE" : "INACTIVE");
    Serial.print("\",\"motion\":\"");
    Serial.print(motionDetected ? "DETECTED" : "NONE");
    Serial.print("\",\"distance\":");
    Serial.print(distCM);
    Serial.print(",\"fanTime\":");
    Serial.print(fanSeconds);
    Serial.println("}");
  }

  // -------------------------
  // 7-SEGMENT DISPLAY UPDATE (only when fan is ON)
  // -------------------------
  if (fanOn) {
    // Only display when fan is running - shows seconds fan has been on
    showSeconds(fanSeconds);
    delay(10); // stabilize display
  } else {
    // Turn off 7-segment display when fan is off
    clear7Segment();
  }
}