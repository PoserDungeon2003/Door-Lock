
// Include the LCD_I2C library
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 4;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Connections to Arduino
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };

// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int n = 0;

const int lock = 10; //lock signal
String input = ""; //user input
String pass = ""; //to store password
int check = 0; // flag

void setup() {
  // Setup serial monitor

  Serial.begin(9600);
  pinMode(lock, OUTPUT);
  lcd.init();
  lcd.backlight();

  digitalWrite(lock, LOW);
  lcd.setCursor(3, 0);
  lcd.print("Welcome");
  delay(2000);
  lcd.clear();
  del("INPUT PIN: ");

  pass = getPIN(0);
}

void loop() {
  // Get key value if pressed
  char customKey = customKeypad.getKey();
  if (customKey) {
    // Print key value to serial monitor
    lcd.setCursor(n, 1);
    lcd.print(customKey);
    input.concat(customKey);
    n++;
  }

  switch (customKey) {
    case 'A':
      switch (check){
        case 0:
          if(input==pass){
            del("OK");
            digitalWrite(lock, HIGH);
            delay(10000);
            digitalWrite(lock, LOW);
          }
          else{
            center("TRY AGAIN");
          }
          break;

        case 1:
          if(input==pass){
            del("NEW PIN:");
            check = 2;
          }
          else{
            center("TRY AGAIN");
          }
          break;
        case 2:
          pass=input;
          storePIN(0, pass);
          del("PIN Changed");
          check=0;
          break;
      }
      break;

    case 'B':
      del("OLD PIN:");
      check = 1;
      break;

    case 'C':
      digitalWrite(lock, LOW);
      switch (check){
        case 0:
          del("INPUT PIN:");
          break;
        case 1:
          del("OLD PIN:");
          break;
        case 2:
          del("NEW PIN:");
          break;
      }
      break;
      case '#':
        
        Serial.println(pass);
  }
}

void del(String text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  input = "";
  n = 0;
}
void center(String text) {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(text);
  input = "";
  n = 0;
}
void storePIN(int addr, String pass){
  byte l = pass.length();
  EEPROM.write(addr,l);
  for(int i=0; i<l; i++){
    EEPROM.write(addr+i+1, pass[i]);
  }
}
String getPIN(int addr){
  int l1 = EEPROM.read(addr);
  char code[l1+1];
  for(int i=0; i<l1; i++){
    code[i] = EEPROM.read(addr+i+1);
  }
  code[l1] = '\0';
  return String(code);
}
