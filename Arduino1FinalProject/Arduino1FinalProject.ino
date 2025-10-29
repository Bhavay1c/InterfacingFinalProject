// // // /*
// // // Starting the final project Arduino 1 

// // // Membrane switch keypad (passcodse & temperature input)
// // // LCD1602 display
// // // 4-digit 7-segment display
// // // Active buzzer
// // // Push button (PB1)
// // /*
// //  * CST8227 Final Project - MEGA-1 (Master Controller)
// //  * Multi-Sensor Monitoring and Control System
// //  * 
// //  * COMPONENTS:
// //  * - 4x4 Membrane Keypad (Pins 22,24,26,28,30,32,34,36)
// //  * - LCD1602 Display (Pins 2,3,4,5,11,12)
// //  * - 5641AS 7-Segment Display (Segments: 23,25,27,29,31,33,35,37 | Digits: 38,40,42,44)
// //  * - Active Buzzer (Pin 48)
// //  * - Push Button PB1 (Pin 46)
// //  * - I2C Communication (SDA=20, SCL=21)
// //  * 
// //  * FUNCTIONALITY:
// //  * 1. Passcode entry (double confirmation)
// //  * 2. Temperature preference setting
// //  * 3. Display passcode on 7-segment (flashing)
// //  * 4. Push button to start system
// //  * 5. Send temperature to MEGA-2 via I2C
// //  */

// // #include <Keypad.h>
// // #include <LiquidCrystal.h>
// // #include <Wire.h>

// // // ========== I2C CONFIGURATION ==========
// // #define MEGA2_I2C_ADDRESS 8  // MEGA-2 slave address

// // // ========== KEYPAD CONFIGURATION ==========
// // const byte ROWS = 4; 
// // const byte COLS = 4; 

// // char keys[ROWS][COLS] = {
// //   {'1','2','3','A'},
// //   {'4','5','6','B'},
// //   {'7','8','9','C'},
// //   {'*','0','#','D'}
// // };

// // byte rowPins[ROWS] = {22, 24, 26, 28}; 
// // byte colPins[COLS] = {30, 32, 34, 36}; 

// // Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// // // ========== LCD CONFIGURATION ==========
// // LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// // // ========== 7-SEGMENT DISPLAY CONFIGURATION ==========
// // #define SEG_A  23
// // #define SEG_B  25
// // #define SEG_C  27
// // #define SEG_D  29
// // #define SEG_E  31
// // #define SEG_F  33
// // #define SEG_G  35
// // #define SEG_DP 37

// // #define DIGIT_1 38
// // #define DIGIT_2 40
// // #define DIGIT_3 42
// // #define DIGIT_4 44

// // const byte SEGMENT_PINS[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
// // const byte DIGIT_PINS[] = {DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4};

// // // Segment patterns for digits 0-9: {A, B, C, D, E, F, G, DP}
// // const byte DIGIT_PATTERNS[10][8] = {
// //   {1,1,1,1,1,1,0,0},  // 0
// //   {0,1,1,0,0,0,0,0},  // 1
// //   {1,1,0,1,1,0,1,0},  // 2
// //   {1,1,1,1,0,0,1,0},  // 3
// //   {0,1,1,0,0,1,1,0},  // 4
// //   {1,0,1,1,0,1,1,0},  // 5
// //   {1,0,1,1,1,1,1,0},  // 6
// //   {1,1,1,0,0,0,0,0},  // 7
// //   {1,1,1,1,1,1,1,0},  // 8
// //   {1,1,1,1,0,1,1,0}   // 9
// // };

// // // ========== PUSH BUTTON & BUZZER ==========
// // #define PUSH_BUTTON_PIN 46
// // #define ACTIVE_BUZZER_PIN 48

// // // ========== GLOBAL VARIABLES ==========
// // String passcode1 = "";
// // String passcode2 = "";
// // String finalPasscode = "";
// // String tempInput = "";
// // float preferredTempC = 0.0;
// // float preferredTempF = 0.0;

// // enum SystemState {
// //   PASSCODE_ENTRY_1,
// //   PASSCODE_ENTRY_2,
// //   TEMP_SETTING,
// //   WAITING_FOR_START,
// //   SYSTEM_RUNNING
// // };

// // SystemState currentState = PASSCODE_ENTRY_1;

// // // ========== SETUP ==========
// // void setup() {
// //   Serial.begin(9600);
// //   Serial.println("========================================");
// //   Serial.println("   CST8227 Final Project - MEGA-1");
// //   Serial.println("   Master Controller");
// //   Serial.println("========================================");
// //   Serial.println();
  
// //   // Initialize I2C as Master
// //   Wire.begin();
// //   Serial.println("I2C Master initialized");
  
// //   // Initialize LCD
// //   lcd.begin(16, 2);
  
// //   // Initialize 7-segment display
// //   for (int i = 0; i < 8; i++) {
// //     pinMode(SEGMENT_PINS[i], OUTPUT);
// //     digitalWrite(SEGMENT_PINS[i], LOW);
// //   }
// //   for (int i = 0; i < 4; i++) {
// //     pinMode(DIGIT_PINS[i], OUTPUT);
// //     digitalWrite(DIGIT_PINS[i], HIGH);
// //   }
  
// //   // Initialize Push Button
// //   pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  
// //   // Initialize Buzzer
// //   pinMode(ACTIVE_BUZZER_PIN, OUTPUT);
// //   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
// //   // Welcome message
// //   displayWelcome();
// //   delay(2000);
  
// //   // Start passcode entry
// //   promptPasscodeEntry1();
// // }

// // // ========== MAIN LOOP ==========
// // void loop() {
// //   // Handle different system states
// //   switch (currentState) {
// //     case PASSCODE_ENTRY_1:
// //       handlePasscodeEntry1();
// //       break;
      
// //     case PASSCODE_ENTRY_2:
// //       handlePasscodeEntry2();
// //       break;
      
// //     case TEMP_SETTING:
// //       handleTempSetting();
// //       break;
      
// //     case WAITING_FOR_START:
// //       handleWaitingForStart();
// //       break;
      
// //     case SYSTEM_RUNNING:
// //       handleSystemRunning();
// //       break;
// //   }
// // }

// // // ========== STATE HANDLERS ==========

// // void handlePasscodeEntry1() {
// //   char key = keypad.getKey();
  
// //   if (key) {
// //     beep(50);  // Short beep for feedback
    
// //     if (key == '#') {
// //       // Confirm first passcode
// //       if (passcode1.length() == 4) {
// //         Serial.print("First passcode entered: ");
// //         Serial.println(passcode1);
// //         currentState = PASSCODE_ENTRY_2;
// //         promptPasscodeEntry2();
// //       } else {
// //         lcd.clear();
// //         lcd.setCursor(0, 0);
// //         lcd.print("Error: Need 4");
// //         lcd.setCursor(0, 1);
// //         lcd.print("digits!");
// //         delay(2000);
// //         passcode1 = "";
// //         promptPasscodeEntry1();
// //       }
// //     } else if (key == '*') {
// //       // Backspace
// //       if (passcode1.length() > 0) {
// //         passcode1.remove(passcode1.length() - 1);
// //         updatePasscodeDisplay(passcode1);
// //       }
// //     } else if (key >= '0' && key <= '9') {
// //       // Add digit
// //       if (passcode1.length() < 4) {
// //         passcode1 += key;
// //         updatePasscodeDisplay(passcode1);
// //       }
// //     }
// //   }
// // }

// // void handlePasscodeEntry2() {
// //   char key = keypad.getKey();
  
// //   if (key) {
// //     beep(50);
    
// //     if (key == '#') {
// //       // Confirm second passcode
// //       if (passcode2.length() == 4) {
// //         Serial.print("Second passcode entered: ");
// //         Serial.println(passcode2);
        
// //         // Check if passcodes match
// //         if (passcode1 == passcode2) {
// //           finalPasscode = passcode1;
// //           Serial.println("Passcodes match!");
          
// //           // Flash passcode on 7-segment display
// //           flashPasscodeOn7Segment();
          
// //           // Move to temperature setting
// //           currentState = TEMP_SETTING;
// //           promptTempSetting();
// //         } else {
// //           // Passcodes don't match - restart
// //           lcd.clear();
// //           lcd.setCursor(0, 0);
// //           lcd.print("Passcodes");
// //           lcd.setCursor(0, 1);
// //           lcd.print("Don't Match!");
// //           beep(1000);  // Long error beep
// //           delay(2000);
          
// //           passcode1 = "";
// //           passcode2 = "";
// //           currentState = PASSCODE_ENTRY_1;
// //           promptPasscodeEntry1();
// //         }
// //       } else {
// //         lcd.clear();
// //         lcd.setCursor(0, 0);
// //         lcd.print("Error: Need 4");
// //         lcd.setCursor(0, 1);
// //         lcd.print("digits!");
// //         delay(2000);
// //         passcode2 = "";
// //         promptPasscodeEntry2();
// //       }
// //     } else if (key == '*') {
// //       // Backspace
// //       if (passcode2.length() > 0) {
// //         passcode2.remove(passcode2.length() - 1);
// //         updatePasscodeDisplay(passcode2);
// //       }
// //     } else if (key >= '0' && key <= '9') {
// //       // Add digit
// //       if (passcode2.length() < 4) {
// //         passcode2 += key;
// //         updatePasscodeDisplay(passcode2);
// //       }
// //     }
// //   }
// // }

// // void handleTempSetting() {
// //   char key = keypad.getKey();
  
// //   if (key) {
// //     beep(50);
    
// //     if (key == '#') {
// //       // Confirm temperature
// //       if (tempInput.length() == 3) {
// //         // Convert "235" to 23.5
// //         int tempValue = tempInput.toInt();
// //         preferredTempC = tempValue / 10.0;
// //         preferredTempF = (preferredTempC * 9.0 / 5.0) + 32.0;
        
// //         Serial.print("Temperature set: ");
// //         Serial.print(preferredTempC);
// //         Serial.print("°C / ");
// //         Serial.print(preferredTempF);
// //         Serial.println("°F");
        
// //         displayTemperature();
// //         delay(3000);
        
// //         currentState = WAITING_FOR_START;
// //         promptPushButton();
// //       } else {
// //         lcd.clear();
// //         lcd.setCursor(0, 0);
// //         lcd.print("Error: Need 3");
// //         lcd.setCursor(0, 1);
// //         lcd.print("digits!");
// //         delay(2000);
// //         tempInput = "";
// //         promptTempSetting();
// //       }
// //     } else if (key == '*') {
// //       // Backspace
// //       if (tempInput.length() > 0) {
// //         tempInput.remove(tempInput.length() - 1);
// //         updateTempDisplay(tempInput);
// //       }
// //     } else if (key >= '0' && key <= '9') {
// //       // Add digit
// //       if (tempInput.length() < 3) {
// //         tempInput += key;
// //         updateTempDisplay(tempInput);
// //       }
// //     }
// //   }
// // }

// // void handleWaitingForStart() {
// //   // Check for push button press
// //   if (digitalRead(PUSH_BUTTON_PIN) == LOW) {
// //     Serial.println("Push button pressed - Starting system!");
    
// //     lcd.clear();
// //     lcd.setCursor(0, 0);
// //     lcd.print("System is ON");
    
// //     beep(100);
// //     delay(100);
// //     beep(100);
// //     delay(100);
// //     beep(100);
    
// //     // Send temperature to MEGA-2 via I2C
// //     sendTemperatureToMega2();
    
// //     delay(2000);
// //     currentState = SYSTEM_RUNNING;
    
// //     lcd.clear();
// //     lcd.setCursor(0, 0);
// //     lcd.print("System Running");
// //     lcd.setCursor(0, 1);
// //     lcd.print("Temp:");
// //     lcd.print(preferredTempC, 1);
// //     lcd.print("C");
    
// //     Serial.println("System is now running!");
// //   }
// // }

// // void handleSystemRunning() {
// //   // System is running - maintain display
// //   // In full project, this will coordinate with MEGA-2
  
// //   // Keep refreshing 7-segment with passcode
// //   static unsigned long lastRefresh = 0;
// //   if (millis() - lastRefresh > 100) {
// //     display7SegmentNumber(finalPasscode.toInt());
// //     lastRefresh = millis();
// //   }
// // }

// // // ========== DISPLAY FUNCTIONS ==========

// // void displayWelcome() {
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   lcd.print("CST8227 Project");
// //   lcd.setCursor(0, 0);
// //   lcd.print("  MEGA-1 Ready");
// //   Serial.println("Welcome message displayed");
// // }

// // void promptPasscodeEntry1() {
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   lcd.print("Enter Passcode:");
// //   lcd.setCursor(0, 1);
// //   lcd.print("(4 digits)");
// //   Serial.println("Waiting for first passcode entry...");
// // }

// // void promptPasscodeEntry2() {
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   lcd.print("Re-enter Code:");
// //   lcd.setCursor(0, 1);
// //   lcd.print("(confirm)");
// //   Serial.println("Waiting for second passcode entry...");
// // }

// // void updatePasscodeDisplay(String passcode) {
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   if (currentState == PASSCODE_ENTRY_1) {
// //     lcd.print("Enter Passcode:");
// //   } else {
// //     lcd.print("Re-enter Code:");
// //   }
// //   lcd.setCursor(0, 1);
  
// //   // Show asterisks for security
// //   for (int i = 0; i < passcode.length(); i++) {
// //     lcd.print("*");
// //   }
// // }

// // void promptTempSetting() {
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   lcd.print("Set Temp (3dig):");
// //   lcd.setCursor(0, 1);
// //   lcd.print("e.g. 235=23.5C");
// //   Serial.println("Waiting for temperature setting...");
// //   delay(2000);
  
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   lcd.print("Enter Temp:");
// //   lcd.setCursor(0, 1);
// // }

// // void updateTempDisplay(String temp) {
// //   lcd.setCursor(0, 1);
// //   lcd.print("                ");  // Clear line
// //   lcd.setCursor(0, 1);
// //   lcd.print(temp);
  
// //   // Show preview with decimal
// //   if (temp.length() >= 2) {
// //     lcd.print(" (");
// //     lcd.print(temp.substring(0, 2));
// //     lcd.print(".");
// //     if (temp.length() == 3) {
// //       lcd.print(temp.substring(2));
// //     } else {
// //       lcd.print("_");
// //     }
// //     lcd.print("C)");
// //   }
// // }

// // void displayTemperature() {
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   lcd.print("Temp Set:");
// //   lcd.setCursor(0, 1);
// //   lcd.print(preferredTempC, 1);
// //   lcd.print("C / ");
// //   lcd.print(preferredTempF, 1);
// //   lcd.print("F");
// // }

// // void promptPushButton() {
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   lcd.print("Press PB1 to");
// //   lcd.setCursor(0, 1);
// //   lcd.print("Start System");
// //   Serial.println("Waiting for push button press...");
// // }

// // // ========== 7-SEGMENT DISPLAY FUNCTIONS ==========

// // void displayDigit(byte digitPosition, byte number) {
// //   // Turn off all digits
// //   for (int i = 0; i < 4; i++) {
// //     digitalWrite(DIGIT_PINS[i], HIGH);
// //   }
  
// //   // Set segment pattern
// //   for (int seg = 0; seg < 8; seg++) {
// //     digitalWrite(SEGMENT_PINS[seg], DIGIT_PATTERNS[number][seg]);
// //   }
  
// //   // Turn on selected digit
// //   digitalWrite(DIGIT_PINS[digitPosition], LOW);
// // }

// // void display7SegmentNumber(int number) {
// //   byte digits[4];
// //   digits[0] = (number / 1000) % 10;
// //   digits[1] = (number / 100) % 10;
// //   digits[2] = (number / 10) % 10;
// //   digits[3] = number % 10;
  
// //   for (int d = 0; d < 4; d++) {
// //     displayDigit(d, digits[d]);
// //     delay(2);
// //   }
// // }

// // void flashPasscodeOn7Segment() {
// //   lcd.clear();
// //   lcd.setCursor(0, 0);
// //   lcd.print("Passcode OK!");
  
// //   Serial.println("Flashing passcode on 7-segment display...");
  
// //   int passcodeNum = finalPasscode.toInt();
// //   unsigned long startTime;
  
// //   // Flash 5 times with 1 second interval
// //   for (int flash = 0; flash < 5; flash++) {
// //     // Display ON for 500ms
// //     startTime = millis();
// //     while (millis() - startTime < 500) {
// //       display7SegmentNumber(passcodeNum);
// //     }
    
// //     // Display OFF for 500ms
// //     clearDisplay();
// //     delay(500);
// //   }
  
// //   // Final display for 1 second
// //   startTime = millis();
// //   while (millis() - startTime < 1000) {
// //     display7SegmentNumber(passcodeNum);
// //   }
  
// //   // Activate buzzer for 1 second
// //   digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
// //   delay(1000);
// //   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
// //   Serial.println("7-segment flash complete");
// // }

// // void clearDisplay() {
// //   for (int i = 0; i < 4; i++) {
// //     digitalWrite(DIGIT_PINS[i], HIGH);
// //   }
// //   for (int i = 0; i < 8; i++) {
// //     digitalWrite(SEGMENT_PINS[i], LOW);
// //   }
// // }

// // // ========== I2C COMMUNICATION ==========

// // void sendTemperatureToMega2() {
// //   Serial.print("Sending temperature to MEGA-2: ");
// //   Serial.print(preferredTempC);
// //   Serial.println("°C");
  
// //   Wire.beginTransmission(MEGA2_I2C_ADDRESS);
  
// //   // Send temperature as integer (multiply by 10 to preserve decimal)
// //   int tempValue = (int)(preferredTempC * 10);
// //   Wire.write(highByte(tempValue));
// //   Wire.write(lowByte(tempValue));
  
// //   byte error = Wire.endTransmission();
  
// //   if (error == 0) {
// //     Serial.println("Temperature sent successfully!");
// //   } else {
// //     Serial.print("I2C transmission error: ");
// //     Serial.println(error);
// //   }
// // }

// // // ========== BUZZER FUNCTIONS ==========

// // void beep(int duration) {
// //   digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
// //   delay(duration);
// //   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
// // }


// /*
//  * CST8227 Final Project - MEGA-1 (Master Controller)
//  * IMPROVED VERSION with better 7-segment handling
//  */

// #include <Keypad.h>
// #include <LiquidCrystal.h>
// #include <Wire.h>

// // ========== I2C CONFIGURATION ==========
// #define MEGA2_I2C_ADDRESS 8

// // ========== KEYPAD CONFIGURATION ==========
// const byte ROWS = 4; 
// const byte COLS = 4; 

// char keys[ROWS][COLS] = {
//   {'1','2','3','A'},
//   {'4','5','6','B'},
//   {'7','8','9','C'},
//   {'*','0','#','D'}
// };

// byte rowPins[ROWS] = {22, 24, 26, 28}; 
// byte colPins[COLS] = {30, 32, 34, 36}; 

// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// // ========== LCD CONFIGURATION ==========
// LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// // ========== 7-SEGMENT DISPLAY CONFIGURATION ==========
// #define SEG_A  23
// #define SEG_B  25
// #define SEG_C  27
// #define SEG_D  29
// #define SEG_E  31
// #define SEG_F  33
// #define SEG_G  35
// #define SEG_DP 37

// #define DIGIT_1 38
// #define DIGIT_2 40
// #define DIGIT_3 42
// #define DIGIT_4 44

// const byte SEGMENT_PINS[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
// const byte DIGIT_PINS[] = {DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4};

// // Segment patterns for digits 0-9: {A, B, C, D, E, F, G, DP}
// const byte DIGIT_PATTERNS[10][8] = {
//   {1,1,1,1,1,1,0,0},  // 0
//   {0,1,1,0,0,0,0,0},  // 1
//   {1,1,0,1,1,0,1,0},  // 2
//   {1,1,1,1,0,0,1,0},  // 3
//   {0,1,1,0,0,1,1,0},  // 4
//   {1,0,1,1,0,1,1,0},  // 5
//   {1,0,1,1,1,1,1,0},  // 6
//   {1,1,1,0,0,0,0,0},  // 7
//   {1,1,1,1,1,1,1,0},  // 8
//   {1,1,1,1,0,1,1,0}   // 9
// };

// // ========== PUSH BUTTON & BUZZER ==========
// #define PUSH_BUTTON_PIN 46
// #define ACTIVE_BUZZER_PIN 48

// // ========== GLOBAL VARIABLES ==========
// String passcode1 = "";
// String passcode2 = "";
// String finalPasscode = "";
// String tempInput = "";
// float preferredTempC = 0.0;
// float preferredTempF = 0.0;

// bool display7SegmentActive = false;  // Control when 7-segment should display

// enum SystemState {
//   PASSCODE_ENTRY_1,
//   PASSCODE_ENTRY_2,
//   TEMP_SETTING,
//   WAITING_FOR_START,
//   SYSTEM_RUNNING
// };

// SystemState currentState = PASSCODE_ENTRY_1;

// // ========== SETUP ==========
// void setup() {
//   Serial.begin(9600);
//   Serial.println("========================================");
//   Serial.println("   CST8227 Final Project - MEGA-1");
//   Serial.println("   Master Controller - IMPROVED");
//   Serial.println("========================================");
//   Serial.println();
  
//   // Initialize I2C as Master
//   Wire.begin();
//   Serial.println("I2C Master initialized");
  
//   // Initialize LCD
//   lcd.begin(16, 2);
  
//   // Initialize 7-segment display
//   for (int i = 0; i < 8; i++) {
//     pinMode(SEGMENT_PINS[i], OUTPUT);
//     digitalWrite(SEGMENT_PINS[i], LOW);
//   }
//   for (int i = 0; i < 4; i++) {
//     pinMode(DIGIT_PINS[i], OUTPUT);
//     digitalWrite(DIGIT_PINS[i], HIGH);
//   }
  
//   Serial.println("Testing all 4 digits on 7-segment...");
//   test7SegmentAllDigits();  // Test to verify all digits work
  
//   // Initialize Push Button
//   pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  
//   // Initialize Buzzer
//   pinMode(ACTIVE_BUZZER_PIN, OUTPUT);
//   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
//   // Welcome message
//   displayWelcome();
//   delay(2000);
  
//   // Start passcode entry
//   promptPasscodeEntry1();
// }

// // ========== MAIN LOOP ==========
// void loop() {
//   // Handle different system states
//   switch (currentState) {
//     case PASSCODE_ENTRY_1:
//       handlePasscodeEntry1();
//       break;
      
//     case PASSCODE_ENTRY_2:
//       handlePasscodeEntry2();
//       break;
      
//     case TEMP_SETTING:
//       handleTempSetting();
//       break;
      
//     case WAITING_FOR_START:
//       handleWaitingForStart();
//       break;
      
//     case SYSTEM_RUNNING:
//       handleSystemRunning();
//       break;
//   }
  
//   // Only update 7-segment if it should be active
//   if (display7SegmentActive) {
//     display7SegmentNumber(finalPasscode.toInt());
//   } else {
//     clearDisplay();
//   }
// }

// // ========== STATE HANDLERS ==========

// void handlePasscodeEntry1() {
//   char key = keypad.getKey();
  
//   if (key) {
//     beep(50);
    
//     if (key == '#') {
//       if (passcode1.length() == 4) {
//         Serial.print("First passcode entered: ");
//         Serial.println(passcode1);
//         currentState = PASSCODE_ENTRY_2;
//         promptPasscodeEntry2();
//       } else {
//         showError("Error: Need 4 digits!");
//         passcode1 = "";
//         promptPasscodeEntry1();
//       }
//     } else if (key == '*') {
//       if (passcode1.length() > 0) {
//         passcode1.remove(passcode1.length() - 1);
//         updatePasscodeDisplay(passcode1);
//       }
//     } else if (key >= '0' && key <= '9') {
//       if (passcode1.length() < 4) {
//         passcode1 += key;
//         updatePasscodeDisplay(passcode1);
//       }
//     }
//   }
// }

// void handlePasscodeEntry2() {
//   char key = keypad.getKey();
  
//   if (key) {
//     beep(50);
    
//     if (key == '#') {
//       if (passcode2.length() == 4) {
//         Serial.print("Second passcode entered: ");
//         Serial.println(passcode2);
        
//         if (passcode1 == passcode2) {
//           finalPasscode = passcode1;
//           Serial.println("Passcodes match!");
//           Serial.print("Final passcode: ");
//           Serial.println(finalPasscode);
          
//           // Flash passcode on 7-segment display
//           flashPasscodeOn7Segment();
          
//           // Move to temperature setting
//           currentState = TEMP_SETTING;
//           promptTempSetting();
//         } else {
//           showError("Passcodes Don't Match!");
//           beep(1000);
//           passcode1 = "";
//           passcode2 = "";
//           currentState = PASSCODE_ENTRY_1;
//           promptPasscodeEntry1();
//         }
//       } else {
//         showError("Error: Need 4 digits!");
//         passcode2 = "";
//         promptPasscodeEntry2();
//       }
//     } else if (key == '*') {
//       if (passcode2.length() > 0) {
//         passcode2.remove(passcode2.length() - 1);
//         updatePasscodeDisplay(passcode2);
//       }
//     } else if (key >= '0' && key <= '9') {
//       if (passcode2.length() < 4) {
//         passcode2 += key;
//         updatePasscodeDisplay(passcode2);
//       }
//     }
//   }
// }

// void handleTempSetting() {
//   char key = keypad.getKey();
  
//   if (key) {
//     beep(50);
    
//     if (key == '#') {
//       if (tempInput.length() == 3) {
//         int tempValue = tempInput.toInt();
//         preferredTempC = tempValue / 10.0;
//         preferredTempF = (preferredTempC * 9.0 / 5.0) + 32.0;
        
//         Serial.print("Temperature set: ");
//         Serial.print(preferredTempC);
//         Serial.print("°C / ");
//         Serial.print(preferredTempF);
//         Serial.println("°F");
        
//         displayTemperature();
//         delay(3000);
        
//         currentState = WAITING_FOR_START;
//         promptPushButton();
//       } else {
//         showError("Error: Need 3 digits!");
//         tempInput = "";
//         promptTempSetting();
//       }
//     } else if (key == '*') {
//       if (tempInput.length() > 0) {
//         tempInput.remove(tempInput.length() - 1);
//         updateTempDisplay(tempInput);
//       }
//     } else if (key >= '0' && key <= '9') {
//       if (tempInput.length() < 3) {
//         tempInput += key;
//         updateTempDisplay(tempInput);
//       }
//     }
//   }
// }

// void handleWaitingForStart() {
//   if (digitalRead(PUSH_BUTTON_PIN) == LOW) {
//     Serial.println("Push button pressed - Starting system!");
    
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("System is ON");
    
//     // 3 beeps
//     for (int i = 0; i < 3; i++) {
//       beep(100);
//       delay(100);
//     }
    
//     // Send temperature to MEGA-2 via I2C
//     sendTemperatureToMega2();
    
//     delay(2000);
//     currentState = SYSTEM_RUNNING;
//     display7SegmentActive = true;  // NOW turn on 7-segment
    
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("System Running");
//     lcd.setCursor(0, 1);
//     lcd.print("Temp:");
//     lcd.print(preferredTempC, 1);
//     lcd.print("C");
    
//     Serial.println("System is now running!");
//     Serial.println("7-segment display is now ON");
//   }
// }

// void handleSystemRunning() {
//   // System running - 7-segment controlled by main loop
//   // Could add more functionality here
// }

// // ========== DISPLAY FUNCTIONS ==========

// void displayWelcome() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("  MEGA-1 Ready");
//   lcd.setCursor(0, 1);
//   lcd.print(" CST8227 Project");
//   Serial.println("Welcome message displayed");
// }

// void promptPasscodeEntry1() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Enter Passcode:");
//   lcd.setCursor(0, 1);
//   lcd.print("(4 digits)");
//   Serial.println("Waiting for first passcode...");
// }

// void promptPasscodeEntry2() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Re-enter Code:");
//   lcd.setCursor(0, 1);
//   lcd.print("(confirm)");
//   Serial.println("Waiting for confirmation...");
// }

// void updatePasscodeDisplay(String passcode) {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   if (currentState == PASSCODE_ENTRY_1) {
//     lcd.print("Enter Passcode:");
//   } else {
//     lcd.print("Re-enter Code:");
//   }
//   lcd.setCursor(0, 1);
//   for (int i = 0; i < passcode.length(); i++) {
//     lcd.print("*");
//   }
// }

// void promptTempSetting() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Set Temp (3dig):");
//   lcd.setCursor(0, 1);
//   lcd.print("e.g. 235=23.5C");
//   Serial.println("Waiting for temperature...");
//   delay(2000);
  
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Enter Temp:");
//   lcd.setCursor(0, 1);
// }

// void updateTempDisplay(String temp) {
//   lcd.setCursor(0, 1);
//   lcd.print("                ");
//   lcd.setCursor(0, 1);
//   lcd.print(temp);
  
//   if (temp.length() >= 2) {
//     lcd.print(" (");
//     lcd.print(temp.substring(0, 2));
//     lcd.print(".");
//     if (temp.length() == 3) {
//       lcd.print(temp.substring(2));
//     } else {
//       lcd.print("_");
//     }
//     lcd.print("C)");
//   }
// }

// void displayTemperature() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Temp Set:");
//   lcd.setCursor(0, 1);
//   lcd.print(preferredTempC, 1);
//   lcd.print("C / ");
//   lcd.print(preferredTempF, 1);
//   lcd.print("F");
// }

// void promptPushButton() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Press PB1 to");
//   lcd.setCursor(0, 1);
//   lcd.print("Start System");
//   Serial.println("Waiting for button...");
// }

// void showError(String message) {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print(message.substring(0, 16));
//   if (message.length() > 16) {
//     lcd.setCursor(0, 1);
//     lcd.print(message.substring(16));
//   }
//   Serial.println(message);
//   delay(2000);
// }

// // ========== 7-SEGMENT FUNCTIONS ==========

// void displayDigit(byte digitPosition, byte number) {
//   // Turn off all digits first
//   for (int i = 0; i < 4; i++) {
//     digitalWrite(DIGIT_PINS[i], HIGH);
//   }
  
//   // Set segment pattern
//   for (int seg = 0; seg < 8; seg++) {
//     digitalWrite(SEGMENT_PINS[seg], DIGIT_PATTERNS[number][seg]);
//   }
  
//   // Turn on selected digit
//   digitalWrite(DIGIT_PINS[digitPosition], LOW);
// }

// void display7SegmentNumber(int number) {
//   byte digits[4];
//   digits[0] = (number / 1000) % 10;
//   digits[1] = (number / 100) % 10;
//   digits[2] = (number / 10) % 10;
//   digits[3] = number % 10;
  
//   for (int d = 0; d < 4; d++) {
//     displayDigit(d, digits[d]);
//     delayMicroseconds(2000);  // 2ms delay
//   }
// }

// void flashPasscodeOn7Segment() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Passcode OK!");
  
//   Serial.println("Flashing passcode...");
//   Serial.print("Passcode value: ");
//   Serial.println(finalPasscode);
  
//   int passcodeNum = finalPasscode.toInt();
  
//   // Flash 5 times
//   for (int flash = 0; flash < 5; flash++) {
//     Serial.print("Flash ");
//     Serial.println(flash + 1);
    
//     // ON for 500ms
//     unsigned long startTime = millis();
//     while (millis() - startTime < 500) {
//       display7SegmentNumber(passcodeNum);
//     }
    
//     // OFF for 500ms (actually 1 second interval as required)
//     clearDisplay();
//     delay(1000);
//   }
  
//   // Final display for 1 second
//   Serial.println("Final display...");
//   unsigned long startTime = millis();
//   while (millis() - startTime < 1000) {
//     display7SegmentNumber(passcodeNum);
//   }
  
//   // Buzzer for 1 second
//   Serial.println("Buzzer ON");
//   digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
//   delay(1000);
//   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
//   clearDisplay();
//   Serial.println("Flash complete!");
// }

// void clearDisplay() {
//   for (int i = 0; i < 4; i++) {
//     digitalWrite(DIGIT_PINS[i], HIGH);
//   }
//   for (int i = 0; i < 8; i++) {
//     digitalWrite(SEGMENT_PINS[i], LOW);
//   }
// }

// void test7SegmentAllDigits() {
//   // Test each digit individually
//   Serial.println("Testing Digit 1...");
//   for (int i = 0; i < 100; i++) {
//     displayDigit(0, 1);
//     delay(5);
//   }
  
//   Serial.println("Testing Digit 2...");
//   for (int i = 0; i < 100; i++) {
//     displayDigit(1, 2);
//     delay(5);
//   }
  
//   Serial.println("Testing Digit 3...");
//   for (int i = 0; i < 100; i++) {
//     displayDigit(2, 3);
//     delay(5);
//   }
  
//   Serial.println("Testing Digit 4...");
//   for (int i = 0; i < 100; i++) {
//     displayDigit(3, 4);
//     delay(5);
//   }
  
//   Serial.println("Testing all together: 1234");
//   for (int i = 0; i < 200; i++) {
//     display7SegmentNumber(1234);
//   }
  
//   clearDisplay();
//   Serial.println("7-segment test complete!");
// }

// // ========== I2C & BUZZER ==========

// void sendTemperatureToMega2() {
//   Serial.print("Sending to MEGA-2: ");
//   Serial.print(preferredTempC);
//   Serial.println("°C");
  
//   Wire.beginTransmission(MEGA2_I2C_ADDRESS);
//   int tempValue = (int)(preferredTempC * 10);
//   Wire.write(highByte(tempValue));
//   Wire.write(lowByte(tempValue));
//   byte error = Wire.endTransmission();
  
//   if (error == 0) {
//     Serial.println("Sent successfully!");
//   } else {
//     Serial.print("I2C error: ");
//     Serial.println(error);
//   }
// }

// void beep(int duration) {
//   digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
//   delay(duration);
//   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
// }
/*
 * CST8227 Final Project - MEGA-1 (Master Controller)
 * Multi-Sensor Monitoring and Control System
 * 
 * COMPONENTS:
 * - 4x4 Membrane Keypad (Pins 22,24,26,28,30,32,34,36)
 * - LCD1602 Display (Pins 2,3,4,5,11,12)
 * - 5641AS 7-Segment Display (Segments: 23,25,27,29,31,33,35,37 | Digits: 38,40,42,44)
 * - Active Buzzer (Pin 48)
 * - Push Button PB1 (Pin 46)
 * - I2C Communication (SDA=20, SCL=21)
 */

#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Wire.h>

// ========== I2C CONFIGURATION ==========
#define MEGA2_I2C_ADDRESS 8

// ========== KEYPAD CONFIGURATION ==========
const byte ROWS = 4; 
const byte COLS = 4; 

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {22, 24, 26, 28}; 
byte colPins[COLS] = {30, 32, 34, 36}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ========== LCD CONFIGURATION ==========
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ========== 7-SEGMENT DISPLAY CONFIGURATION ==========
#define SEG_A  23
#define SEG_B  25
#define SEG_C  27
#define SEG_D  29
#define SEG_E  31
#define SEG_F  33
#define SEG_G  35
#define SEG_DP 37

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
  
  // Initialize I2C as Master
  Wire.begin();
  
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
  
  // Only update 7-segment if active
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
          
          // Flash passcode on 7-segment
          flashPasscodeOn7Segment();
          
          // Move to temperature setting
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
        Serial.print(preferredTempC);
        Serial.print("C / ");
        Serial.print(preferredTempF);
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
    Serial.println("System starting!");
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System is ON");
    
    // 3 beeps
    for (int i = 0; i < 3; i++) {
      beep(100);
      delay(100);
    }
    
    // Send temperature to MEGA-2
    sendTemperatureToMega2();
    
    delay(2000);
    currentState = SYSTEM_RUNNING;
    display7SegmentActive = true;
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Running");
    lcd.setCursor(0, 1);
    lcd.print("Temp:");
    lcd.print(preferredTempC, 1);
    lcd.print("C");
    
    Serial.println("System running!");
  }
}

void handleSystemRunning() {
  // System running - 7-segment shows passcode continuously
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
  
  // Flash 5 times with 1 second interval
  for (int flash = 0; flash < 5; flash++) {
    // ON for 500ms
    unsigned long startTime = millis();
    while (millis() - startTime < 500) {
      display7SegmentNumber(passcodeNum);
    }
    
    // OFF for 1000ms
    clearDisplay();
    delay(1000);
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
}

void clearDisplay() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(DIGIT_PINS[i], HIGH);
  }
  for (int i = 0; i < 8; i++) {
    digitalWrite(SEGMENT_PINS[i], LOW);
  }
}

// ========== I2C COMMUNICATION ==========

void sendTemperatureToMega2() {
  Serial.print("Sending to MEGA-2: ");
  Serial.print(preferredTempC);
  Serial.println("C");
  
  Wire.beginTransmission(MEGA2_I2C_ADDRESS);
  int tempValue = (int)(preferredTempC * 10);
  Wire.write(highByte(tempValue));
  Wire.write(lowByte(tempValue));
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("Sent successfully!");
  } else {
    Serial.print("I2C error: ");
    Serial.println(error);
  }
}

// ========== BUZZER ==========

void beep(int duration) {
  digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(ACTIVE_BUZZER_PIN, LOW);
}
