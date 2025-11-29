// ============================================
// CST8227 FINAL PROJECT - MEGA-1 (MASTER)
// ============================================
// Real implementation - NO SIMULATION
// Features:
// 1. Passcode entry via keypad (4x4 membrane)
// 2. Preferred temperature setting
// 3. I2C communication with MEGA-2
// 4. ESP32 communication (optional)
// 5. 7-segment display for passcode flashing
// 6. LCD display for user feedback

#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Wire.h>

// ========== I2C CONFIGURATION ==========
#define MEGA2_I2C_ADDRESS 8
#define I2C_CMD_READY 100
#define I2C_CMD_SET_TEMP 101

// ========== KEYPAD CONFIGURATION ==========
const byte ROWS = 4; 
const byte COLS = 4; 

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {23, 25, 27, 29}; 
byte colPins[COLS] = {31, 33, 35, 37}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ========== LCD CONFIGURATION ==========
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ========== 7-SEGMENT DISPLAY CONFIGURATION ==========
#define SEG_A  22
#define SEG_B  24
#define SEG_C  26
#define SEG_D  28
#define SEG_E  30 
#define SEG_F  32
#define SEG_G  34
#define SEG_DP 36

#define DIGIT_1 38
#define DIGIT_2 40
#define DIGIT_3 42
#define DIGIT_4 44

const byte SEGMENT_PINS[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
const byte DIGIT_PINS[] = {DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4};

const byte DIGIT_PATTERNS[10][8] = {
  {1,1,1,1,1,1,0,0},  // 0
  {0,1,1,0,0,0,0,0},  // 1
  {1,1,0,1,1,0,1,0},  // 2
  {1,1,1,1,0,0,1,0},  // 3
  {0,1,1,0,0,1,1,0},  // 4
  {1,0,1,1,0,1,1,0},  // 5
  {1,0,1,1,1,1,1,0},  // 6
  {1,1,1,0,0,0,0,0},  // 7
  {1,1,1,1,1,1,1,0},  // 8
  {1,1,1,1,0,1,1,0}   // 9
};

// ========== PUSH BUTTON & BUZZER ==========
#define PUSH_BUTTON_PIN 46
#define ACTIVE_BUZZER_PIN 48

// ========== GLOBAL VARIABLES ==========
String passcode1 = "";
String passcode2 = "";
String finalPasscode = "";
String tempInput = "";
float preferredTempC = 0.0;
float preferredTempF = 0.0;

bool display7SegmentActive = false;

// Data received from MEGA-2
float receivedTempC = 0.0;
float receivedHumidity = 0.0;
bool fanStatus = false;
bool nightModeStatus = false;
bool motionStatus = false;

enum SystemState {
  PASSCODE_ENTRY_1,
  PASSCODE_ENTRY_2,
  TEMP_SETTING,
  WAITING_FOR_START,
  SYSTEM_RUNNING
};

SystemState currentState = PASSCODE_ENTRY_1;

// ========== SETUP ==========
void setup() {
  Serial.begin(9600);
  Serial.println("CST8227 Final Project - MEGA-1");
  Serial.println("=====================================");
  
  // Initialize Serial1 for ESP32 (optional)
  Serial1.begin(9600);  // TX1=Pin 18, RX1=Pin 19
  Serial.println("Serial1 ready for ESP32");
  
  // Initialize I2C as Master
  Wire.begin();
  Serial.println("I2C Master initialized");
  
  // Initialize LCD
  lcd.begin(16, 2);
  
  // Initialize 7-segment display
  for (int i = 0; i < 8; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);
    digitalWrite(SEGMENT_PINS[i], LOW);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(DIGIT_PINS[i], OUTPUT);
    digitalWrite(DIGIT_PINS[i], HIGH);
  }
  
  // Initialize Push Button
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize Buzzer
  pinMode(ACTIVE_BUZZER_PIN, OUTPUT);
  digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
  // Welcome message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  MEGA-1 Ready");
  lcd.setCursor(0, 1);
  lcd.print(" CST8227 Project");
  delay(2000);
  
  // Start passcode entry
  promptPasscodeEntry1();
}

// ========== MAIN LOOP ==========
void loop() {
  switch (currentState) {
    case PASSCODE_ENTRY_1:
      handlePasscodeEntry1();
      break;
      
    case PASSCODE_ENTRY_2:
      handlePasscodeEntry2();
      break;
      
    case TEMP_SETTING:
      handleTempSetting();
      break;
      
    case WAITING_FOR_START:
      handleWaitingForStart();
      break;
      
    case SYSTEM_RUNNING:
      handleSystemRunning();
      break;
  }
  
  // Only update 7-segment if active (during passcode flashing)
  if (display7SegmentActive) {
    display7SegmentNumber(finalPasscode.toInt());
  } else {
    clearDisplay();
  }
}

// ========== STATE HANDLERS ==========

void handlePasscodeEntry1() {
  char key = keypad.getKey();
  
  if (key) {
    beep(50);
    
    if (key == '#') {
      if (passcode1.length() == 4) {
        Serial.print("First passcode: ");
        Serial.println(passcode1);
        currentState = PASSCODE_ENTRY_2;
        promptPasscodeEntry2();
      } else {
        showError("Need 4 digits!");
        passcode1 = "";
        promptPasscodeEntry1();
      }
    } else if (key == '*') {
      if (passcode1.length() > 0) {
        passcode1.remove(passcode1.length() - 1);
        updatePasscodeDisplay(passcode1);
      }
    } else if (key >= '0' && key <= '9') {
      if (passcode1.length() < 4) {
        passcode1 += key;
        updatePasscodeDisplay(passcode1);
      }
    }
  }
}

void handlePasscodeEntry2() {
  char key = keypad.getKey();
  
  if (key) {
    beep(50);
    
    if (key == '#') {
      if (passcode2.length() == 4) {
        Serial.print("Second passcode: ");
        Serial.println(passcode2);
        
        if (passcode1 == passcode2) {
          finalPasscode = passcode1;
          Serial.println("Passcodes match!");
          
          flashPasscodeOn7Segment();
          
          currentState = TEMP_SETTING;
          promptTempSetting();
        } else {
          showError("Don't Match!");
          beep(1000);
          passcode1 = "";
          passcode2 = "";
          currentState = PASSCODE_ENTRY_1;
          promptPasscodeEntry1();
        }
      } else {
        showError("Need 4 digits!");
        passcode2 = "";
        promptPasscodeEntry2();
      }
    } else if (key == '*') {
      if (passcode2.length() > 0) {
        passcode2.remove(passcode2.length() - 1);
        updatePasscodeDisplay(passcode2);
      }
    } else if (key >= '0' && key <= '9') {
      if (passcode2.length() < 4) {
        passcode2 += key;
        updatePasscodeDisplay(passcode2);
      }
    }
  }
}

void handleTempSetting() {
  char key = keypad.getKey();
  
  if (key) {
    beep(50);
    
    if (key == '#') {
      if (tempInput.length() == 3) {
        int tempValue = tempInput.toInt();
        preferredTempC = tempValue / 10.0;
        preferredTempF = (preferredTempC * 9.0 / 5.0) + 32.0;
        
        Serial.print("Temp set: ");
        Serial.print(preferredTempC, 1);
        Serial.print("C / ");
        Serial.print(preferredTempF, 1);
        Serial.println("F");
        
        displayTemperature();
        delay(3000);
        
        currentState = WAITING_FOR_START;
        promptPushButton();
      } else {
        showError("Need 3 digits!");
        tempInput = "";
        promptTempSetting();
      }
    } else if (key == '*') {
      if (tempInput.length() > 0) {
        tempInput.remove(tempInput.length() - 1);
        updateTempDisplay(tempInput);
      }
    } else if (key >= '0' && key <= '9') {
      if (tempInput.length() < 3) {
        tempInput += key;
        updateTempDisplay(tempInput);
      }
    }
  }
}

void handleWaitingForStart() {
  if (digitalRead(PUSH_BUTTON_PIN) == LOW) {
    delay(50);  // Debounce
    if (digitalRead(PUSH_BUTTON_PIN) == LOW) {
      Serial.println("System starting!");
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("System is ON");
      
      for (int i = 0; i < 3; i++) {
        beep(100);
        delay(100);
      }
      
      // Send to MEGA-2 via I2C
      sendTemperatureToMega2();
      
      delay(2000);
      currentState = SYSTEM_RUNNING;
      display7SegmentActive = false;  // Don't display passcode when running
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("System Running");
      lcd.setCursor(0, 1);
      lcd.print("Temp:");
      lcd.print(preferredTempC, 1);
      lcd.print("C");
      
      Serial.println("System running!");
      
      // Send initial status to ESP32
      sendToESP32();
    }
  }
}

void handleSystemRunning() {
  // System is running
  // Request data from MEGA-2 every 2 seconds
  static unsigned long lastDataRequest = 0;
  if (millis() - lastDataRequest >= 2000) {
    requestDataFromMega2();
    lastDataRequest = millis();
  }
  
  // Send status to ESP32 every 2 seconds
  static unsigned long lastESP32Send = 0;
  if (millis() - lastESP32Send >= 2000) {
    sendToESP32();
    lastESP32Send = millis();
  }
  
  // Update LCD with received data
  static unsigned long lastLCDUpdate = 0;
  if (millis() - lastLCDUpdate >= 5000) {
    updateSystemStatusLCD();
    lastLCDUpdate = millis();
  }
}

// ========== DISPLAY FUNCTIONS ==========

void promptPasscodeEntry1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Passcode:");
  lcd.setCursor(0, 1);
  lcd.print("(4 digits)");
}

void promptPasscodeEntry2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Re-enter Code:");
  lcd.setCursor(0, 1);
  lcd.print("(confirm)");
}

void updatePasscodeDisplay(String passcode) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (currentState == PASSCODE_ENTRY_1) {
    lcd.print("Enter Passcode:");
  } else {
    lcd.print("Re-enter Code:");
  }
  lcd.setCursor(0, 1);
  for (int i = 0; i < passcode.length(); i++) {
    lcd.print("*");
  }
}

void promptTempSetting() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Temp (3dig):");
  lcd.setCursor(0, 1);
  lcd.print("e.g. 235=23.5C");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Temp:");
  lcd.setCursor(0, 1);
}

void updateTempDisplay(String temp) {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(temp);
  
  if (temp.length() >= 2) {
    lcd.print(" (");
    lcd.print(temp.substring(0, 2));
    lcd.print(".");
    if (temp.length() == 3) {
      lcd.print(temp.substring(2));
    } else {
      lcd.print("_");
    }
    lcd.print("C)");
  }
}

void displayTemperature() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp Set:");
  lcd.setCursor(0, 1);
  lcd.print(preferredTempC, 1);
  lcd.print("C / ");
  lcd.print(preferredTempF, 1);
  lcd.print("F");
}

void promptPushButton() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press PB1 to");
  lcd.setCursor(0, 1);
  lcd.print("Start System");
}

void showError(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(2000);
}

// ========== 7-SEGMENT FUNCTIONS ==========

void displayDigit(byte digitPosition, byte number) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(DIGIT_PINS[i], HIGH);
  }
  
  for (int seg = 0; seg < 8; seg++) {
    digitalWrite(SEGMENT_PINS[seg], DIGIT_PATTERNS[number][seg]);
  }
  
  digitalWrite(DIGIT_PINS[digitPosition], LOW);
}

void display7SegmentNumber(int number) {
  byte digits[4];
  digits[0] = (number / 1000) % 10;
  digits[1] = (number / 100) % 10;
  digits[2] = (number / 10) % 10;
  digits[3] = number % 10;
  
  for (int d = 0; d < 4; d++) {
    displayDigit(d, digits[d]);
    delayMicroseconds(2000);
  }
}

void flashPasscodeOn7Segment() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Passcode OK!");
  
  int passcodeNum = finalPasscode.toInt();
  
  // Flash 5 times (1 second on, 1 second off)
  display7SegmentActive = true;
  
  for (int flash = 0; flash < 5; flash++) {
    unsigned long startTime = millis();
    while (millis() - startTime < 500) {
      display7SegmentNumber(passcodeNum);
    }
    
    clearDisplay();
    delay(500);
    
    startTime = millis();
    while (millis() - startTime < 500) {
      display7SegmentNumber(passcodeNum);
    }
    clearDisplay();
    delay(500);
  }
  
  // Final display for 1 second
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    display7SegmentNumber(passcodeNum);
  }
  
  // Activate buzzer for 1 second
  digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
  clearDisplay();
  display7SegmentActive = false;
}

void clearDisplay() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(DIGIT_PINS[i], HIGH);
  }
  for (int i = 0; i < 8; i++) {
    digitalWrite(SEGMENT_PINS[i], LOW);
  }
}

// ========== ESP32 COMMUNICATION ==========

void sendToESP32() {
  // Create JSON string with system data (including data from MEGA-2)
  String json = "{";
  json += "\"passcode\":\"" + finalPasscode + "\",";
  json += "\"prefTemp\":" + String(preferredTempC, 1) + ",";
  json += "\"currentTemp\":" + String(receivedTempC, 1) + ",";
  json += "\"humidity\":" + String(receivedHumidity) + ",";
  json += "\"fanStatus\":\"" + String(fanStatus ? "ON" : "OFF") + "\",";
  json += "\"nightMode\":" + String(nightModeStatus ? "true" : "false") + ",";
  json += "\"motion\":" + String(motionStatus ? "true" : "false");
  json += "}";
  
  // Send to ESP32 via Serial1
  Serial1.println(json);
  
  // Debug to Serial Monitor
  Serial.print("→ ESP32: ");
  Serial.println(json);
}

// ========== I2C COMMUNICATION ==========
void sendTemperatureToMega2() {
  Serial.println("\n========================================");
  Serial.println("Sending to MEGA-2 via I2C...");
  Serial.print("  Temperature: ");
  Serial.print(preferredTempC, 1);
  Serial.println("°C");
  
  // Step 1: Send ACTIVATION command
  Wire.beginTransmission(MEGA2_I2C_ADDRESS);
  Wire.write(I2C_CMD_READY);
  Wire.write(1);  // 1 = activate system
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("  ✓ Activation command sent");
  } else {
    Serial.print("  ✗ Activation error: ");
    Serial.println(error);
  }
  delay(50);  // Small delay between transmissions
  
  // Step 2: Send TEMPERATURE command
  byte tempByte = (byte)(preferredTempC * 10);  // Convert to byte (e.g., 25.0 -> 250)
  
  Wire.beginTransmission(MEGA2_I2C_ADDRESS);
  Wire.write(I2C_CMD_SET_TEMP);
  Wire.write(tempByte);
  error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.print("  ✓ Temperature sent: ");
    Serial.print(preferredTempC, 1);
    Serial.println("°C");
    Serial.println("========================================\n");
  } else {
    Serial.print("  ✗ Temperature error: ");
    Serial.println(error);
    Serial.println("========================================\n");
  }
}

// Request sensor data from MEGA-2
void requestDataFromMega2() {
  // Request 5 bytes from MEGA-2:
  // Byte 0: Temperature (x10)
  // Byte 1: Humidity
  // Byte 2: Fan status (0/1)
  // Byte 3: Night mode (0/1)
  // Byte 4: Motion (0/1)
  
  Wire.requestFrom(MEGA2_I2C_ADDRESS, 5);
  
  if (Wire.available() >= 5) {
    byte tempByte = Wire.read();
    byte humByte = Wire.read();
    byte fanByte = Wire.read();
    byte nightByte = Wire.read();
    byte motionByte = Wire.read();
    
    receivedTempC = tempByte / 10.0;
    receivedHumidity = humByte;
    fanStatus = (fanByte == 1);
    nightModeStatus = (nightByte == 1);
    motionStatus = (motionByte == 1);
    
    Serial.print("← MEGA-2: T=");
    Serial.print(receivedTempC, 1);
    Serial.print("C, H=");
    Serial.print(receivedHumidity);
    Serial.print("%, Fan=");
    Serial.print(fanStatus ? "ON" : "OFF");
    Serial.print(", Night=");
    Serial.print(nightModeStatus ? "YES" : "NO");
    Serial.print(", Motion=");
    Serial.println(motionStatus ? "YES" : "NO");
  } else {
    Serial.println("← MEGA-2: No data available");
  }
}

void updateSystemStatusLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(receivedTempC, 1);
  lcd.print("C H:");
  lcd.print(receivedHumidity, 0);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Fan:");
  lcd.print(fanStatus ? "ON " : "OFF");
  if (nightModeStatus) {
    lcd.print("Night");
  } else if (motionStatus) {
    lcd.print("Motion");
  }
}

// ========== BUZZER ==========

void beep(int duration) {
  digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(ACTIVE_BUZZER_PIN, LOW);
}