/*
Starting the final project Arduino 1 

Membrane switch keypad (passcodse & temperature input)
LCD1602 display
4-digit 7-segment display
Active buzzer
Push button (PB1)

*/
/*
 * CST8227 Final Project - MEGA-1
 * LCD1602 + Keypad Integration Test
 * 
 * Components:
 * - 4x4 Membrane Keypad (Pins 22,24,26,28,30,32,34,36)
 * - LCD1602 Display (Pins 2,3,4,5,11,12)
 * 
 * Functionality:
 * - Display messages on LCD
 * - Show keypad input on LCD
 * - Test backspace (*) and confirm (#) functions
 */

#include <Keypad.h>
#include <LiquidCrystal.h>

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
// LiquidCrystal(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ========== GLOBAL VARIABLES ==========
String inputString = "";      // Store user input
const int MAX_INPUT = 16;     // Maximum characters (LCD width)

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("   MEGA-1: LCD + Keypad Test");
  Serial.println("========================================");
  Serial.println();
  
  // Initialize LCD (16 columns, 2 rows)
  lcd.begin(16, 2);
  
  // Display welcome message
  displayWelcomeMessage();
  
  // Wait 3 seconds
  delay(3000);
  
  // Clear and show input prompt
  clearAndShowPrompt();
}

void loop() {
  // Get key press
  char key = keypad.getKey();
  
  if (key) {
    handleKeyPress(key);
  }
}

// ========== FUNCTION DEFINITIONS ==========

void displayWelcomeMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  CST8227 2025");
  lcd.setCursor(0, 1);
  lcd.print(" MEGA-1 Ready! ");
  
  Serial.println("Welcome message displayed");
}

void clearAndShowPrompt() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Input:");
  lcd.setCursor(0, 1);
  // Line 2 is for user input
  
  Serial.println("Ready for input");
}

void handleKeyPress(char key) {
  Serial.print("Key pressed: ");
  Serial.println(key);
  
  if (key == '#') {
    // CONFIRM key - show what was entered
    handleConfirm();
    
  } else if (key == '*') {
    // BACKSPACE key - delete last character
    handleBackspace();
    
  } else if (key >= '0' && key <= '9') {
    // NUMBER keys (0-9)
    handleNumberInput(key);
    
  } else {
    // LETTER keys (A, B, C, D)
    handleLetterInput(key);
  }
}

void handleConfirm() {
  Serial.print("CONFIRM - Input entered: ");
  Serial.println(inputString);
  
  // Show confirmation screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You entered:");
  lcd.setCursor(0, 1);
  
  if (inputString.length() > 0) {
    lcd.print(inputString);
  } else {
    lcd.print("(nothing)");
  }
  
  // Wait 2 seconds
  delay(2000);
  
  // Reset for next input
  inputString = "";
  clearAndShowPrompt();
}

void handleBackspace() {
  if (inputString.length() > 0) {
    // Remove last character
    inputString.remove(inputString.length() - 1);
    
    Serial.print("BACKSPACE - Current input: ");
    Serial.println(inputString);
    
    // Redraw the input line
    lcd.setCursor(0, 1);
    lcd.print("                ");  // Clear line 2
    lcd.setCursor(0, 1);
    lcd.print(inputString);
    
  } else {
    Serial.println("BACKSPACE - Nothing to delete");
  }
}

void handleNumberInput(char key) {
  if (inputString.length() < MAX_INPUT) {
    inputString += key;
    
    // Display on LCD at current position
    lcd.setCursor(inputString.length() - 1, 1);
    lcd.print(key);
    
    Serial.print("Number added - Current input: ");
    Serial.println(inputString);
  } else {
    Serial.println("Input full! (max 16 characters)");
    
    // Flash the display briefly to indicate full
    lcd.noDisplay();
    delay(100);
    lcd.display();
  }
}

void handleLetterInput(char key) {
  if (inputString.length() < MAX_INPUT) {
    inputString += key;
    
    // Display on LCD
    lcd.setCursor(inputString.length() - 1, 1);
    lcd.print(key);
    
    Serial.print("Letter added - Current input: ");
    Serial.println(inputString);
  } else {
    Serial.println("Input full! (max 16 characters)");
  }
}
