#include <Arduino.h>
#include "Network/connection.h"
#include "WS2811/interface.h"

#define out 2
#define NUM_LEDS 55
#define CPU_FREQUENCY 80

#define size 10

uint32_t colors [size];
int order = 0;
void setup(){
  Serial.begin(9600);
  WiFiSetup();
  delay(5000);
  setCpuFrequencyMhz(CPU_FREQUENCY);
  pinMode(out, OUTPUT);
  digitalWrite(out, LOW);   
  colorGradientGenerate(0x00FF00,0x00CC55, colors, size);

}

void loop(){
  
  for(uint16_t i = 0; i < NUM_LEDS; ++i) {
    sendColor(colors[order]); //BRG
  }
  reset();
  
  order = (order+1) % size;
  delay(500);
}

