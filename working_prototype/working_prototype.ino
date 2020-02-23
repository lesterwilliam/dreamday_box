// https://www.best-microcontroller-projects.com/max7219.html
// max7219_two
// Based on https://gist.github.com/nrdobie/8193350
#include <Wire.h>
#include <DS3231.h>
#include <SPI.h>

#define SS_PIN A3
#define CS_SEG A3
#define CS_DOT A2

// MAX7219 SPI LED Driver
#define MAX7219_TEST        0x0f
#define MAX7219_BRIGHTNESS  0x0a
#define MAX7219_SCAN_LIMIT  0x0b
#define MAX7219_DECODE_MODE 0x09
#define MAX7219_SHUTDOWN    0x0C

RTClib RTC;

const long hammock_unixtime = 1403913000;
const long correction_unixtime = 1638043;

const int test_matrix[8] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00};
const unsigned int test_matrix2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned int matrix_heart_small[8] = {0x30, 0x78, 0x7C, 0x3E, 0x3E, 0x7C, 0x78, 0x30};
const unsigned int matrix_heart_big[8] = {0x78, 0xFC, 0xFE, 0x7F, 0x7F, 0xFE, 0xFC, 0x78};
const unsigned int matrix_empty[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};



void maxTransferCMD(uint8_t chipSel, uint8_t address, uint8_t value){  
uint8_t i;
  digitalWrite(chipSel, LOW);   
  SPI.transfer(address);      // Send address.
  SPI.transfer(value);        // Send the value.
  SPI.transfer(address);      // Send address. 
  SPI.transfer(value);        // Send the value.
  digitalWrite(chipSel, HIGH); // Finish transfer.
}

void writeDigits(long value){
  long temp = value;
  int digitCount = 0;
  
  while(temp != 0){
    digitCount++;
    temp /= 10;
  }

  for(int i = 5; i > (5 - digitCount); i--){
    digitalWrite(CS_SEG, LOW);
    SPI.transfer(i);
    SPI.transfer(value % 10);
    digitalWrite(CS_SEG, HIGH);
    value = value / 10;
  }

  for(int i = 0; i < (6 - digitCount); i++){
    digitalWrite(CS_SEG, LOW);
    SPI.transfer(i);
    SPI.transfer(0x0F);
    digitalWrite(CS_SEG, HIGH);
    value = value / 10;
  }
}

void writeMatrix(unsigned int value[]){
  for(int i = 0; i < 8; i++){
    digitalWrite(CS_DOT, LOW);
    SPI.transfer(i + 1);
    SPI.transfer(matrix_heart_big[i]);
    digitalWrite(CS_DOT, HIGH);
  }
}

void writeMatrix_randError(){
  // clear bit bitwise: number &= ~(1UL << n);
  
  for(int i = 0; i < 8; i++){
    unsigned int value = matrix_heart_big[i];
    if(random(0,9) < 9){
      value &= ~(random(0,255));
    }
    //value &= ~(1UL << random(0,8));
    digitalWrite(CS_DOT, LOW);
    SPI.transfer(i + 1);
    SPI.transfer(value);
    digitalWrite(CS_DOT, HIGH);
  }
}

void writeMatrixLine(int value){
  digitalWrite(CS_DOT, LOW);
  SPI.transfer(1);
  SPI.transfer(0x07);
  digitalWrite(CS_DOT, HIGH);

  digitalWrite(CS_DOT, LOW);
  SPI.transfer(2);
  SPI.transfer(0x02);
  digitalWrite(CS_DOT, HIGH);

  digitalWrite(CS_DOT, LOW);
  SPI.transfer(3);
  SPI.transfer(0x04);
  digitalWrite(CS_DOT, HIGH);

  digitalWrite(CS_DOT, LOW);
  SPI.transfer(4);
  SPI.transfer(0x08);
  digitalWrite(CS_DOT, HIGH);

  digitalWrite(CS_DOT, LOW);
  SPI.transfer(5);
  SPI.transfer(0x10);
  digitalWrite(CS_DOT, HIGH);

  digitalWrite(CS_DOT, LOW);
  SPI.transfer(6);
  SPI.transfer(0x00);
  digitalWrite(CS_DOT, HIGH);

  digitalWrite(CS_DOT, LOW);
  SPI.transfer(7);
  SPI.transfer(0x40);
  digitalWrite(CS_DOT, HIGH);

  digitalWrite(CS_DOT, LOW);
  SPI.transfer(8);
  SPI.transfer(0x00);
  digitalWrite(CS_DOT, HIGH);
}

void writeDot(uint8_t row, uint8_t col){
  digitalWrite(CS_DOT, LOW);
  SPI.transfer(row + 1);
  SPI.transfer(col);
  digitalWrite(CS_DOT, HIGH);
}
/*
void setRTC(){
    RTC.setYear(20);
    RTC.setMonth(2);
    RTC.setDate(2);
    RTC.setDoW(5);
    RTC.setHour(14);
    RTC.setMinute(16);
    RTC.setSecond(30);
}*/

long getYearSince(){
  DateTime lol = RTC.now();
  long value;
  value = lol.unixtime() - hammock_unixtime + correction_unixtime;
  Serial.println(lol.unixtime());
  value = (long)(value / 86400);
  return value;
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
  
  // Init 7-seg driver
  //maxTransferCMD(CS_DOT, MAX7219_TEST, 0x01);  delay(1000);  
  maxTransferCMD(CS_SEG, MAX7219_TEST, 0x00);        // Finish test mode.
  maxTransferCMD(CS_SEG, MAX7219_DECODE_MODE, 0x1F); // Enable BCD mode. 
  maxTransferCMD(CS_SEG, MAX7219_BRIGHTNESS, 0x00);  // Use lowest intensity. 
  maxTransferCMD(CS_SEG, MAX7219_SCAN_LIMIT, 0x07);  // Max scan-limit to lower current draw
  maxTransferCMD(CS_SEG, MAX7219_SHUTDOWN, 0x01);    // Turn on chip.

  // Init dot-matrix driver
  //maxTransferCMD(CS_DOT, MAX7219_TEST, 0x01);  delay(1000);  
  //maxTransferCMD(CS_DOT, MAX7219_TEST, 0x00);        // Finish test mode.
  maxTransferCMD(CS_DOT, MAX7219_DECODE_MODE, 0x00); // Enable BCD mode. 
  maxTransferCMD(CS_DOT, MAX7219_BRIGHTNESS, 0x00);  // Use lowest intensity. 
  maxTransferCMD(CS_DOT, MAX7219_SCAN_LIMIT, 0x07);  // Scan all digits.
  maxTransferCMD(CS_DOT, MAX7219_SHUTDOWN, 0x01);    // Turn on chip.
}

void loop() {
  DateTime now = RTC.now();
  
  writeDigits(getYearSince());
  
  writeMatrix_randError();
  delay(20);
}
