

#include <LiquidCrystal_I2C.h> // Include the LCD_I2C library for LCD screen
#include <Keypad.h> // Include the Keypad library for keypad
#include <EEPROM.h> // Include the EEPROM library for accessing ROM

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
  pinMode(lock, OUTPUT); // set the lock output
  lcd.init();
  lcd.backlight();

  digitalWrite(lock, LOW); //default the lock will be locked
  lcd.setCursor(3, 0);
  lcd.print("WELCOME"); //Welcome screen
  delay(2000);
  lcd.clear();
  del("INPUT PIN: "); //input PIN code

  pass = getPIN(0); // get PIN from ROM
}

void loop() {
  // Get key value if pressed
  char customKey = customKeypad.getKey(); //get input from keypad
  if (customKey) {
    lcd.setCursor(n, 1);
    lcd.print(customKey); //print all the desired output to screen
    input.concat(customKey); //passing parameter from "customKey" value to "input" value
    n++;
  }

  switch (customKey) {
    case 'A': //Enter pass mode button
      switch (check){
        //check = 0; ==> default mode (Enter PIN for authorization)
        //check = 1; ==> Enter old PIN (For change pass mode)
        //check = 2; ==> Enter new PIN (FOr change pass mode)
        case 0:
          if(input==pass){
            del("OK");
            digitalWrite(lock, HIGH);
            delay(2000);
            digitalWrite(lock, LOW);
            del("INPUT PIN:");
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

    case 'B': // Change PIN mode
      del("OLD PIN:");
      check = 1;
      break;

    case 'C': //Clear the screen
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
      case 'D':
        del("INPUT PIN:");
        break;
      case '#': //show PIN value on Serial Plotter that was saved in ROM
        
        Serial.println(pass);
        break;
  }
}

void del(String text) { //function to clear and set cursor to the right position
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  input = "";
  n = 0;
}
void center(String text) { //function to clear and set cursor to center
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print(text);
  input = "";
  n = 0;
}
void storePIN(int addr, String pass){ //store PIN into EEPROM
  byte l = pass.length();
  EEPROM.write(addr,l);
  for(int i=0; i<l; i++){
    EEPROM.write(addr+i+1, pass[i]);
  }
}
String getPIN(int addr){ //get PIN from EEPROM
  int l1 = EEPROM.read(addr);
  char code[l1+1];
  for(int i=0; i<l1; i++){
    code[i] = EEPROM.read(addr+i+1);
  }
  code[l1] = '\0';
  return String(code);
}
