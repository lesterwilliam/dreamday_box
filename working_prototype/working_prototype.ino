/*
 * Title:       Dreamday box
 * Author:      Adrian Schwizgebel
 * Date:        23. February 2020
 * Description: [blah]
 * Licence:     MIT
 */

// Library inclusions
#include <Wire.h>     // I2C library for DS3231 RTC
#include <DS3231.h>   // Library for DS3231 RTC
#include <SPI.h>      // SPI library for MAX7219 drivers

// Pin definitions
#define CS_SEG A3 // Chip select for 7-segment display
#define CS_DOT A2 // Chip select for 8x8 Led matrix

// MAX7219 SPI LED Driver register addresses
#define MAX7219_TEST        0x0F
#define MAX7219_BRIGHTNESS  0x0A
#define MAX7219_SCAN_LIMIT  0x0B
#define MAX7219_DECODE_MODE 0x09
#define MAX7219_SHUTDOWN    0x0C

// Timing definitions
#define SEGMENT_INTERVAL    5000 // These values define how frequent those taskt are done (in ms)
#define MATRIX_INTERVAL     200

unsigned long currentMillis = 0;
unsigned long segment_previousMillis = 0;
unsigned long matrix_previousMillis = 0;

RTClib RTC;

// Special date in unixtime code
const long hammock_unixtime = 1403913000;
const long correction_unixtime = 1638043;

// 8x8 Led matrix pictures
const unsigned int test_matrix[8]         = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00};
const unsigned int matrix_heart_small[8]  = {0x30, 0x78, 0x7C, 0x3E, 0x3E, 0x7C, 0x78, 0x30};
const unsigned int matrix_heart_big[8]    = {0x78, 0xFC, 0xFE, 0x7F, 0x7F, 0xFE, 0xFC, 0x78};
const unsigned int matrix_empty[8]        = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void maxTransferCMD(uint8_t chipSel, uint8_t address, uint8_t value){  
uint8_t i;
  digitalWrite(chipSel, LOW);   
  SPI.transfer(address);
  SPI.transfer(value);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(chipSel, HIGH);
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
  for(int i = 0; i < 8; i++){
    unsigned int value = matrix_heart_big[i];
    
    if(random(0,9) < 9){
      value &= ~(random(0,255) & random(0,255) & random(0,255) & random(0,255) & random(0,255) & random(0,255) & random(0,255));
    }

    digitalWrite(CS_DOT, LOW);
    SPI.transfer(i + 1);
    SPI.transfer(value);
    digitalWrite(CS_DOT, HIGH);
  }
}

void writeDot(uint8_t row, uint8_t col){
  digitalWrite(CS_DOT, LOW);
  SPI.transfer(row + 1);
  SPI.transfer(col);
  digitalWrite(CS_DOT, HIGH);
}

long getYearSince(){
  DateTime unixNow = RTC.now();
  
  return (long)((unixNow.unixtime() - hammock_unixtime + correction_unixtime) / 86400);
}

void setup() {
  // Set chip select pins as outputs
  pinMode(CS_SEG, OUTPUT);
  pinMode(CS_DOT, OUTPUT);

  SPI.setBitOrder(MSBFIRST);   // Reverse the SPI Data o/p. 
  SPI.begin();                 // Start SPI
  Wire.begin();
  
  // Init 7-seg driver
  maxTransferCMD(CS_SEG, MAX7219_DECODE_MODE, 0x1F); // Enable BCD mode. 
  maxTransferCMD(CS_SEG, MAX7219_BRIGHTNESS, 0x00);  // Use lowest intensity. 
  maxTransferCMD(CS_SEG, MAX7219_SCAN_LIMIT, 0x07);  // Max scan-limit to lower current draw
  maxTransferCMD(CS_SEG, MAX7219_SHUTDOWN, 0x01);    // Turn on chip.

  // Init dot-matrix driver
  maxTransferCMD(CS_DOT, MAX7219_DECODE_MODE, 0x00); // Enable BCD mode. 
  maxTransferCMD(CS_DOT, MAX7219_BRIGHTNESS, 0x00);  // Use lowest intensity. 
  maxTransferCMD(CS_DOT, MAX7219_SCAN_LIMIT, 0x07);  // Scan all digits.
  maxTransferCMD(CS_DOT, MAX7219_SHUTDOWN, 0x01);    // Turn on chip.

  // Write first time
  writeDigits(getYearSince());
  writeMatrix_randError();
}

void loop() {
  DateTime now = RTC.now();
  
  currentMillis = millis();
  
  if (currentMillis - segment_previousMillis >= SEGMENT_INTERVAL) {
    segment_previousMillis = currentMillis;
    writeDigits(getYearSince());
  }
  
  if (currentMillis - matrix_previousMillis >= MATRIX_INTERVAL) {
    matrix_previousMillis = currentMillis;
    writeMatrix_randError();
  }
}
