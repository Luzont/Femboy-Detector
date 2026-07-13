//LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//RFID
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>

#define SS_PIN   10
#define RST_PIN  9

bool CanScan = true;

//LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

//RFID setup
MFRC522DriverPinSimple rst_pin(RST_PIN);
MFRC522DriverPinSimple ss_pin(SS_PIN);
MFRC522DriverSPI driver(ss_pin);
MFRC522 mfrc522(driver);

const byte CardUIDs[2][4] = {
  {0xEC, 0x12, 0xFA, 0x00}, //RFID tag (you will have to know what your RFID tag's UID is)
  {0x31, 0x73, 0x18, 0x02} //RFID tag
};

// Custom full block character
byte BlockFull[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

// Custom Checkerboard character
byte BlockPartial[8] = {
  B10101,
  B01010,
  B10101,
  B01010,
  B10101,
  B01010,
  B10101,
  B01010,
};

// Left part of my version of the sad emoji
byte SadLeft[8] = {
  B00000,
  B01100,
  B01100,
  B10000,
  B10111,
  B10100,
  B10011,
  B00000
};

// Right part of my version of the sad emoji
byte SadRight[8] = {
  B00000,
  B00110,
  B00110,
  B00000,
  B11100,
  B00100,
  B11000,
  B00000
};

// Left part of my version of the praying emoji
byte PrayLeft[8] = {
  B00001,
  B00011,
  B00011,
  B00011,
  B11110,
  B11110,
  B00100,
  B00000
};

// Right part of my version of the praying emoji
byte PrayRight[8] = {
  B10000,
  B11000,
  B11000,
  B11000,
  B01111,
  B01111,
  B00100,
  B00000
};

//tldr; this just checks if the card is one of the cards from the CardUIDs array
bool IsValidCard(byte *UID, byte Size) {
  if (Size != sizeof(CardUIDs[0])) {return false;}

  for (int i = 0; i < 2; i++) {
    bool IsValid = true;
    
    for (int ByteIndex = 0; ByteIndex < Size; ByteIndex++) {
      if (UID[ByteIndex] != CardUIDs[i][ByteIndex]) { IsValid = false; break; }
    }
    
    if (IsValid) {
      return true;
    }
  }
  return false;
}

void DrawLoadingBar(int percent) {
  int filled = percent / 10; // devides since you can have 10 characters on the screen, but not 100
  
  lcd.setCursor(0, 1);
  for (int i = 0; i < 10; i++) {
    if (i < filled) {
      lcd.write(byte(0));  // writes the full block
    } else {
      lcd.write(byte(1));  // writes the checkerboard block
    }
  }
}

void setup() {
  //LCD setup
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tap card to");
  lcd.setCursor(0, 1);
  lcd.print("reveal gender.");

  // Create custom characters (indices 0-4)
  lcd.createChar(0, BlockFull);
  lcd.createChar(1, BlockPartial);
  lcd.createChar(2, SadLeft);
  lcd.createChar(3, SadRight);
  lcd.createChar(4, PrayLeft);
  lcd.createChar(5, PrayRight);

  //RFID
  SPI.begin();
  mfrc522.PCD_Init();

  /*Serial Monitor
  Serial.begin(9600); */
}

void loop() {
  // checks if it's detecting an RFID tag
  if (!mfrc522.PICC_IsNewCardPresent()) {return;}
  // checks it it can actualy read the tag
  if (!mfrc522.PICC_ReadCardSerial()) {return;}

  // gets the values of the scanned UID and UID size
  byte* UID = mfrc522.uid.uidByte;
  byte UIDSize = mfrc522.uid.size;

  // checks if the card is valid and if you can scan
  if (IsValidCard(UID, UIDSize) && CanScan)
  {
    CanScan = false;
    //Fun part (basicaly just a bad loading bar)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Valid scan!");
    delay(1500);
    lcd.clear();
    lcd.print("Analysing:");
    DrawLoadingBar(0);
    delay(2000);
    DrawLoadingBar(10);
    lcd.setCursor(0, 1);
    delay(1500);
    DrawLoadingBar(20);
    delay(1000);
    DrawLoadingBar(30);
    delay(850);
    DrawLoadingBar(40);
    delay(750);
    DrawLoadingBar(50);
    //make it look like it takes time
    delay(2500);
    DrawLoadingBar(90);
    //really fake the loading bar
    delay(1000);
    DrawLoadingBar(100);
    //let it sink in
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Analysis:");
    lcd.setCursor(0, 1);
    lcd.print("Femboy ");
    //writes the emojis
    lcd.write(byte(2));
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));
  }
}
