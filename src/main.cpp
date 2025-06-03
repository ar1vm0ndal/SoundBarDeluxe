#include <Arduino.h>
#define outpin 2

inline void delayCycles(uint32_t cycles) {
  while (cycles--) {
    __asm__ __volatile__("nop");
  }
}

void IRAM_ATTR sendZero() {
  GPIO.out_w1ts = (1 << outpin);     
  delayCycles(60);                   
  GPIO.out_w1tc = (1 << outpin);     
  delayCycles(240);                   
}

void IRAM_ATTR sendOne() {
  GPIO.out_w1ts = (1 << outpin);     
  delayCycles(144);                  
  GPIO.out_w1tc = (1 << outpin);     
  delayCycles(156);                   
}

void IRAM_ATTR sendRET() {
  GPIO.out_w1tc = (1 << outpin);
  delayCycles(14500);                
}

void IRAM_ATTR sendBits(uint32_t color) {
  for (int8_t i = 23; i >= 0; i--) {  // MSB first
    if (color & (1 << i)) sendOne();
    else sendZero();
  }
}

void setup() {
  pinMode(outpin, OUTPUT);
  digitalWrite(outpin, LOW);
  Serial.begin(9600);
}

void loop() {
  sendBits(0x00FF00);  // Green (in GRB order)
  sendRET();
}
