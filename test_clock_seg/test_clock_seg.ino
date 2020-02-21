// https://www.best-microcontroller-projects.com/max7219.html
// max7219_two
// Based on https://gist.github.com/nrdobie/8193350
#include <DS3231.h>
#include <SPI.h>
#define SS_PIN A3
#define CS_SEG A3
#define CS_DOT A2

// MAX7219 SPI LED Driver
#define MAX7219_TEST 0x0f // in real code put into a .h file
#define MAX7219_BRIGHTNESS 0x0a // in real code put into a .h file
#define MAX7219_SCAN_LIMIT 0x0b // in real code put into a .h file
#define MAX7219_DECODE_MODE 0x09 // in real code put into a .h file
#define MAX7219_SHUTDOWN 0x0C // in real code put into a .h file

DS3231 RTC;
const long hammock_unixtime = 1403913000;

void maxTransferCMD(uint8_t address, uint8_t value) {  
uint8_t i;

  digitalWrite(CS_SEG, LOW);   
  SPI.transfer(address);      // Send address.
  SPI.transfer(value);        // Send the value.
  SPI.transfer(address);      // Send address. 
  SPI.transfer(value);        // Send the value.
  digitalWrite(CS_SEG, HIGH); // Finish transfer.
}

void writeDigits(int value){
  for(int i = 5; i > 0; i--){
    digitalWrite(CS_SEG, LOW);
    SPI.transfer(i);
    SPI.transfer(value % 10);
    digitalWrite(CS_SEG, HIGH);
  value = value / 10;
  }
}

void setRTC(){
    RTC.setClockMode(false);  // set to 24h
    RTC.setYear(20);
    RTC.setMonth(2);
    RTC.setDate(2);
    RTC.setDoW(5);
    RTC.setHour(14);
    RTC.setMinute(16);
    RTC.setSecond(30);
}

void getYearSince(){
  long value;
  value = (
}

void setup() {
  Serial.begin(9600);
  Serial.println("Debug MAX7219");

  pinMode(CS_SEG, OUTPUT);
  pinMode(CS_DOT, OUTPUT);

  SPI.setBitOrder(MSBFIRST);   // Reverse the SPI Data o/p. 
  SPI.begin();                 // Start SPI
  Wire.begin();
  //setRTC();
  // Run test - All LED segments lit. 
  //maxTransferCMD(MAX7219_TEST, 0x01);  delay(1000);  
  maxTransferCMD(MAX7219_TEST, 0x00);        // Finish test mode.
  maxTransferCMD(MAX7219_DECODE_MODE, 0x1F); // Enable BCD mode. 
  maxTransferCMD(MAX7219_BRIGHTNESS, 0x00);  // Use lowest intensity. 
  maxTransferCMD(MAX7219_SCAN_LIMIT, 0x1F);  // Scan all digits.
  maxTransferCMD(MAX7219_SHUTDOWN, 0x01);    // Turn on chip.

}

void loop() {
  for(int i = 0; i < 99999;){
    i += 10;
    writeDigits(RTC.getMinute());
    delay(10);
  }
}







