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
 * Membrane Keypad Test Code
 * 
 * This code tests the 4x4 membrane keypad connection
 * and displays pressed keys on Serial Monitor
 */

#include <Keypad.h>

// Define keypad dimensions
const byte ROWS = 4; 
const byte COLS = 4; 

// Define the keypad layout
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Connect keypad ROW pins to these Arduino pins
byte rowPins[ROWS] = {22, 24, 26, 28}; 

// Connect keypad COLUMN pins to these Arduino pins
byte colPins[COLS] = {30, 32, 34, 36}; 

// Create the Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("=================================");
  Serial.println("  Membrane Keypad Test - MEGA-1");
  Serial.println("=================================");
  Serial.println("Press any key on the keypad...");
  Serial.println();
}

void loop() {
  // Check if a key is pressed
  char key = keypad.getKey();
  
  if (key) {
    // Display the pressed key
    Serial.print("Key Pressed: ");
    Serial.println(key);
    
    // Provide feedback for special keys
    if (key == '#') {
      Serial.println("  --> CONFIRM key detected!");
    }
    else if (key == '*') {
      Serial.println("  --> CLEAR key detected!");
    }
    else if (key >= '0' && key <= '9') {
      Serial.println("  --> Number key detected!");
    }
    else {
      Serial.println("  --> Letter key detected!");
    }
    Serial.println();
  }
}
