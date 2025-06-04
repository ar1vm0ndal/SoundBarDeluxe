#include <Arduino.h>
#include "WS2811/interface.h"

#define out 2
#define NUM_LEDS 60
#define CPU_FREQUENCY 80


void setup(){
  setCpuFrequencyMhz(CPU_FREQUENCY);
  pinMode(out, OUTPUT);
  digitalWrite(out, LOW);
}

void loop(){

  for (uint16_t i = 0; i < NUM_LEDS; ++i) {
    sendColor(0x00FF00);   // Red (BRG order)
  }
  reset();
  delay(1000);
  for (uint16_t i = 0; i < NUM_LEDS; ++i) {
    sendColor(0x00FF66);   // Red (BRG order)
  }
  reset();
  delay(1000);
    for (uint16_t i = 0; i < NUM_LEDS; ++i) {
    sendColor(0x0000FF);   // Red (BRG order)
  }
  reset();
  delay(1000);
}