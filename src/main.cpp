#include <Arduino.h>
#include "WS2811/interface.h"

#define out 2
#define NUM_LEDS 55
#define CPU_FREQUENCY 80

#define size 100

uint32_t colors [size];
int order = 0;
void setup(){
  Serial.begin(9600);
  setCpuFrequencyMhz(CPU_FREQUENCY);
  pinMode(out, OUTPUT);
  digitalWrite(out, LOW);  
  colorGradientGenerate(0x000000, 0x0000FF, colors, size);
}

void loop(){
  
  for(uint16_t i = 0; i < NUM_LEDS; ++i) {
    sendColor(colors[order]);   
  }
  reset();
  order = (order+1) % size;
  Serial.println(order);
  Serial.println(colors[order]);
  delay(50);
}

