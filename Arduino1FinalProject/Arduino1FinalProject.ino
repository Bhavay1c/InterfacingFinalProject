// /*
// Starting the final project Arduino 1 

// Membrane switch keypad (passcodse & temperature input)
// LCD1602 display
// 4-digit 7-segment display
// Active buzzer
// Push button (PB1)

// */
// /*
//  * CST8227 Final Project - MEGA-1
//  * Integrated Test: Keypad + LCD + Push Button + Active Buzzer
//  * 
//  * Components:
//  * - 4x4 Membrane Keypad
//  * - LCD1602 Display
//  * - Push Button (PB1)
//  * - Active Buzzer
//  * 
//  * Test Features:
//  * - Enter text with keypad, display on LCD
//  * - Press push button to confirm
//  * - Buzzer beeps when button pressed
//  */

// #include <Keypad.h>
// #include <LiquidCrystal.h>

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

// // ========== PUSH BUTTON & BUZZER ==========
// #define PUSH_BUTTON_PIN 46
// #define ACTIVE_BUZZER_PIN 48

// // ========== GLOBAL VARIABLES ==========
// String inputString = "";
// const int MAX_INPUT = 16;
// bool systemStarted = false;

// void setup() {
//   Serial.begin(9600);
//   Serial.println("========================================");
//   Serial.println("   MEGA-1: Full Component Test");
//   Serial.println("========================================");
//   Serial.println("Components: Keypad, LCD, Button, Buzzer");
//   Serial.println();
  
//   // Initialize LCD
//   lcd.begin(16, 2);
  
//   // Initialize Push Button (with internal pull-up)
//   pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  
//   // Initialize Active Buzzer
//   pinMode(ACTIVE_BUZZER_PIN, OUTPUT);
//   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
//   // Welcome message
//   displayWelcome();
  
//   // Test buzzer
//   Serial.println("Testing buzzer...");
//   testBuzzer();
  
//   delay(2000);
  
//   // Show initial prompt
//   showInputPrompt();
// }

// void loop() {
//   // Check for keypad input
//   char key = keypad.getKey();
//   if (key) {
//     handleKeyPress(key);
//   }
  
//   // Check for push button press
//   if (digitalRead(PUSH_BUTTON_PIN) == LOW) {  // Button pressed (active LOW)
//     handleButtonPress();
//     delay(300);  // Debounce delay
//   }
// }

// // ========== FUNCTION DEFINITIONS ==========

// void displayWelcome() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("  MEGA-1 Test");
//   lcd.setCursor(0, 1);
//   lcd.print("  All Systems!");
  
//   Serial.println("Welcome message displayed");
// }

// void showInputPrompt() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Enter Input:");
//   lcd.setCursor(0, 1);
//   // Line 2 is for user input
  
//   inputString = "";
//   Serial.println("Ready for input");
// }

// void handleKeyPress(char key) {
//   Serial.print("Key: ");
//   Serial.println(key);
  
//   // Short beep for feedback
//   digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
//   delay(50);
//   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
//   if (key == '#') {
//     // Confirm key
//     showConfirmation();
    
//   } else if (key == '*') {
//     // Backspace
//     handleBackspace();
    
//   } else if (inputString.length() < MAX_INPUT) {
//     // Add character
//     inputString += key;
//     lcd.setCursor(inputString.length() - 1, 1);
//     lcd.print(key);
    
//     Serial.print("Input: ");
//     Serial.println(inputString);
//   }
// }

// void handleBackspace() {
//   if (inputString.length() > 0) {
//     inputString.remove(inputString.length() - 1);
    
//     // Redraw line
//     lcd.setCursor(0, 1);
//     lcd.print("                ");  // Clear line
//     lcd.setCursor(0, 1);
//     lcd.print(inputString);
    
//     Serial.print("Backspace - Input: ");
//     Serial.println(inputString);
//   }
// }

// void showConfirmation() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("You entered:");
//   lcd.setCursor(0, 1);
  
//   if (inputString.length() > 0) {
//     lcd.print(inputString);
//   } else {
//     lcd.print("(nothing)");
//   }
  
//   // Long beep
//   digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
//   delay(500);
//   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
//   Serial.print("Confirmed: ");
//   Serial.println(inputString);
  
//   delay(2000);
//   showInputPrompt();
// }

// void handleButtonPress() {
//   Serial.println("PUSH BUTTON PRESSED!");
  
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Button Pressed!");
//   lcd.setCursor(0, 1);
//   lcd.print("System Started");
  
//   // Beep pattern (3 short beeps)
//   for (int i = 0; i < 3; i++) {
//     digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
//     delay(100);
//     digitalWrite(ACTIVE_BUZZER_PIN, LOW);
//     delay(100);
//   }
  
//   systemStarted = true;
  
//   delay(2000);
  
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("System: RUNNING");
//   lcd.setCursor(0, 1);
//   lcd.print("Press keys...");
  
//   Serial.println("System is now running!");
// }

// void testBuzzer() {
//   // Short beep to test buzzer
//   digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
//   delay(200);
//   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
//   delay(200);
//   digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
//   delay(200);
//   digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  
//   Serial.println("Buzzer test complete");
// }

/*
 * CST8227 Final Project - MEGA-1
 * 5641AS 4-Digit 7-Segment Display - Direct Connection
 * 
 * Hardware:
 * - 5641AS Common Cathode 4-digit display
 * - 8× 220Ω resistors for segments
 * 
 * Segment Pins: 6-10, 13-15 (through 220Ω resistors)
 * Digit Pins: 38, 40, 42, 44 (direct connection)
 */

// ========== PIN DEFINITIONS ==========
// Segment pins (A-G + DP) - connect through 220Ω resistors
#define SEG_A  23
#define SEG_B  25
#define SEG_C  27
#define SEG_D  29
#define SEG_E  31
#define SEG_F  33
#define SEG_G  35
#define SEG_DP 37

// Digit control pins (D1-D4) - direct connection, no resistors
#define DIGIT_1 38
#define DIGIT_2 40
#define DIGIT_3 42
#define DIGIT_4 44

// ========== SEGMENT PATTERNS ==========
// Common Cathode: HIGH = ON, LOW = OFF
// Array index = digit to display (0-9)
// Bit order: DP G F E D C B A

const byte SEGMENT_PINS[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
const byte DIGIT_PINS[] = {DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4};

// Segment patterns for digits 0-9
// Format: {A, B, C, D, E, F, G, DP}
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

// ========== GLOBAL VARIABLES ==========
int displayNumber = 0;
unsigned long lastUpdate = 0;
const int UPDATE_INTERVAL = 1000;  // Update every 1 second

// Current digits to display
byte digit[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  5641AS 7-Segment Display Test");
  Serial.println("  Direct Connection Method");
  Serial.println("========================================");
  Serial.println();
  
  // Initialize segment pins as outputs
  for (int i = 0; i < 8; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);
    digitalWrite(SEGMENT_PINS[i], LOW);  // All segments OFF
  }
  
  // Initialize digit pins as outputs
  for (int i = 0; i < 4; i++) {
    pinMode(DIGIT_PINS[i], OUTPUT);
    digitalWrite(DIGIT_PINS[i], HIGH);  // All digits OFF (HIGH = inactive for common cathode)
  }
  
  Serial.println("Hardware initialized!");
  Serial.println();
  
  // Test: Display "8888" for 2 seconds
  Serial.println("Test 1: Displaying 8888...");
  testAllSegments();
  
  Serial.println("Test 2: Counting from 0000 to 9999");
  Serial.println("Starting count...");
  Serial.println();
}

void loop() {
  // Update the displayed number every second
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    displayNumber++;
    if (displayNumber > 9999) {
      displayNumber = 0;
    }
    
    // Break number into individual digits
    digit[0] = (displayNumber / 1000) % 10;  // Thousands
    digit[1] = (displayNumber / 100) % 10;   // Hundreds
    digit[2] = (displayNumber / 10) % 10;    // Tens
    digit[3] = displayNumber % 10;           // Ones
    
    Serial.print("Displaying: ");
    Serial.println(displayNumber);
    
    lastUpdate = millis();
  }
  
  // Multiplexing: rapidly cycle through all 4 digits
  // This happens continuously and very fast!
  for (int d = 0; d < 4; d++) {
    displayDigit(d, digit[d]);
    delay(2);  // 2ms per digit = 8ms total cycle = 125 Hz refresh rate
  }
}

// ========== FUNCTION DEFINITIONS ==========

/*
 * Display a specific digit at a specific position
 * digitPosition: 0-3 (which digit to light up)
 * number: 0-9 (what number to show)
 */
void displayDigit(byte digitPosition, byte number) {
  // Turn off all digits first
  for (int i = 0; i < 4; i++) {
    digitalWrite(DIGIT_PINS[i], HIGH);  // HIGH = OFF for common cathode
  }
  
  // Set the segment pattern for this number
  for (int seg = 0; seg < 8; seg++) {
    digitalWrite(SEGMENT_PINS[seg], DIGIT_PATTERNS[number][seg]);
  }
  
  // Turn on the selected digit
  digitalWrite(DIGIT_PINS[digitPosition], LOW);  // LOW = ON for common cathode
}

/*
 * Test function: Display "8888" for 2 seconds
 */
void testAllSegments() {
  unsigned long startTime = millis();
  
  while (millis() - startTime < 2000) {  // Run for 2 seconds
    for (int d = 0; d < 4; d++) {
      displayDigit(d, 8);  // Display "8" on each digit
      delay(2);
    }
  }
  
  Serial.println("All segments test complete!");
  Serial.println();
}

/*
 * Display a 4-digit number (with leading zeros)
 * number: 0-9999
 * duration: how long to display in milliseconds
 */
void displayNumberFor(int number, int duration) {
  // Break into digits
  byte d1 = (number / 1000) % 10;
  byte d2 = (number / 100) % 10;
  byte d3 = (number / 10) % 10;
  byte d4 = number % 10;
  
  unsigned long startTime = millis();
  
  while (millis() - startTime < duration) {
    displayDigit(0, d1);
    delay(2);
    displayDigit(1, d2);
    delay(2);
    displayDigit(2, d3);
    delay(2);
    displayDigit(3, d4);
    delay(2);
  }
}

/*
 * Clear the display (all segments off)
 */
void clearDisplay() {
  // Turn off all digits
  for (int i = 0; i < 4; i++) {
    digitalWrite(DIGIT_PINS[i], HIGH);
  }
  
  // Turn off all segments
  for (int i = 0; i < 8; i++) {
    digitalWrite(SEGMENT_PINS[i], LOW);
  }
}

/*
 * Flash the display on and off
 * times: number of flashes
 * onTime: milliseconds display is ON
 * offTime: milliseconds display is OFF
 */
void flashDisplay(int number, int times, int onTime, int offTime) {
  for (int i = 0; i < times; i++) {
    // Display ON
    displayNumberFor(number, onTime);
    
    // Display OFF
    clearDisplay();
    delay(offTime);
  }
}
