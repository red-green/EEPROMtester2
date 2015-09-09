#include <Wire.h>
#include <SPI.h>

#include <Adafruit_RGBLCDShield.h>
#include <Adafruit_MCP23017.h>
#include <SD.h>
#include <EEPROM24LC256_512.h>

//define eeprom chip (1 per chip)
EEPROM256_512 mem1;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();


byte dataout[64];
byte datain[64];

long cycles = 0;
//long rounds = 0;


void setup() {
  // setup sd card
  pinMode(10, OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin(10)) {
    return;
  }
  
  //setup lcd
  lcd.begin(16, 2);
  
  //begin I2C Bus
  Wire.begin();
  
  //begin EEPROM with I2C Address 
  mem1.begin(0,0);//addr 0 (DEC) type 0 (defined as 24LC256)
   
  randomSeed(analogRead(1));
  
  lcd.setBacklight(0);
}

// use mem1.writeByte(addr,byte) and mem1.readByte(addr) where -1 < addr < 262144
// use mem1.writePage(page,data) and mem1.readPage(page) where -1 < page < 512 and page length = 64

void loop() {
  //repeat 150 times before saving data to sd card
  long startms = millis();
  long rdeg = 0;
  for(byte a = 0; a < 150; a++) {
    // get random data
    for (byte i = 0; i < 16; i++) {
      dataout[i] = random(255);
    }
    //write it to the eeprom
    mem1.writePage(20,dataout);  //arbitrary value 20
   // delay(1); // a long time, i know!
    //then read it back
    mem1.readPage(20,datain);
    //find degradation of data (0-16*255)
    int cdeg = 0;
    for (byte i = 0; i < 16; i++) {
      cdeg += dataout[i] ^ datain[i];
    }
    rdeg += cdeg;
    cycles++;
        
  }
  long elapsed = millis() - startms;
 // rounds++;
  //save current data
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(cycles);
    dataFile.print(",");
    dataFile.println(rdeg);
    dataFile.close();
    lcd.setCursor(13,1);
    lcd.print(" SD");
  }
  //put it on the lcd
  lcd.setCursor(0,0);
  lcd.print("R:");
  lcd.print(cycles / 150);
  lcd.print("  T:");
  lcd.print(elapsed);
  lcd.setCursor(0,1);
  lcd.print("C:");
  lcd.print(cycles);
  lcd.print(" D:");
  lcd.print((long)rdeg);
}


//from the eeprom library:
/*************************************************** 
  This is a library for the 24LC256 & 24LC512 i2c EEPROM under the
  Arduino Platform.
  
  It provides an easy way to use the above chips while supplying 
  the functionality to read and write pages (64 or 128 bytes) and 
  individual bytes to a page number or address on the EEPROM.
  
  Each page is written in a 16 byte burst mode to account for the
  buffer limit of the Wire library in the Arduino Platform.

  This library is to be used "as-is" without warranty. Please report
  bugs to the GitHub Project.
  
  Written by Michael Neiderhauser.  October 10, 2012
  LGPL license, all text above must be included in any redistribution
 ****************************************************/

