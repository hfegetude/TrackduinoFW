#include <Wire.h>
#include <SD.h>
#include "ov7670.h"
#define PULSE_LENGTH_MS 1
// Camera input/output pin connection to Arduino
#define WRST  22      // Output Write Pointer Reset
#define RRST  23      // Output Read Pointer Reset
#define WEN   24      // Output Write Enable
#define VSYNC 25      // Input Vertical Sync marking frame capture
#define RCLK  26      // Output FIFO buffer output clock
// set OE to low gnd

// FIFO Ram input pins
#define DO7   28     
#define DO6   29   
#define DO5   30   
#define DO4   31   
#define DO3   32   
#define DO2   33   
#define DO1   34
#define DO0   35 
void inline PulseLow(int pin, int duration, int times = 1){
  int i = 0;
  for(i = 0; i<times ; i++){
    digitalWrite(pin, LOW);
    delayMicroseconds(duration);
    digitalWrite(pin, HIGH);
    delayMicroseconds(duration);
  }
}
void inline PulseHigh(int pin, int duration, int times = 1){
  int i = 0;
  for(i = 0; i<times ; i++){
    digitalWrite(pin, HIGH);
    delayMicroseconds(duration);
    digitalWrite(pin, LOW);
    delayMicroseconds(duration);
  }}

void setupOV7670fifo(){
  //Set FIFO control pins
  pinMode(WRST , OUTPUT);
  pinMode(RRST , OUTPUT);
  pinMode(WEN  , OUTPUT);
  pinMode(VSYNC, INPUT);
  pinMode(RCLK , OUTPUT);
  //set data inputs
  DDRL = 0x0;
  PulseLow(WRST, PULSE_LENGTH_MS);
  PulseHigh(RRST, PULSE_LENGTH_MS);
  PulseHigh(RCLK, PULSE_LENGTH_MS, 2);
  digitalWrite(RRST, HIGH);
  //Use onfiguration like the last camera
  camInit();
  setRes(VGA);
  setColorSpace(BAYER_RGB);
  wrReg(0x11,3);
}

void transmit_photo(int wg, int hg)
{
   File ImageOutputFile;
   ImageOutputFile = SD.open("photo_vga.raw", FILE_WRITE);
   digitalWrite(RRST, LOW);
   PulseHigh(RCLK, PULSE_LENGTH_MS,3);
   digitalWrite(RRST, HIGH);

   unsigned long  ByteCounter = 0;
   for (int height = 0; height < hg; height++)
   {
     for (int width = 0; width < wg; width++)
     {
         PulseHigh(RCLK, 1);
         ByteCounter = ByteCounter + ImageOutputFile.write(PINL);
     }
   }
   ImageOutputFile.close();
}

void capture_frame()
{
  //pulse in VSYNC
  while (digitalRead(VSYNC)==1 ) {};
  while (digitalRead(VSYNC)==0 ) {};
  PulseLow(WRST, 6);
  digitalWrite(WEN, HIGH);
  while (digitalRead(VSYNC)==1 ) {};
  while (digitalRead(VSYNC)==0 ) {};
  digitalWrite(WEN, LOW);
  //pulse in VSYNC

}
