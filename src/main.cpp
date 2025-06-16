#include <Arduino.h>
#include "Network/connection.h"
#include "WS2811/interface.h"

#define out 2
#define CPU_FREQUENCY 160

constexpr uint16_t size = 100;

DRAM_ATTR uint32_t colors[size];   
volatile uint32_t order = 0;            
void setup(){
  Serial.begin(9600);
  WiFiSetup();
  delay(5000);
  setCpuFrequencyMhz(CPU_FREQUENCY);
  pinMode(out, OUTPUT);
  digitalWrite(out, LOW);   
  initRmt();
  colorGradientGenerate(0xFF0000,0xCC5500, colors, size);

}

void loop(){
  
    for (uint16_t i = 0; i < NUM_LEDS; ++i) {
        uint32_t c = colors[(order + i) % size];
        uint8_t  r = c >> 16;
        uint8_t  g = (c >> 8) & 0xFF;
        uint8_t  b = c & 0xFF;
        setPixel(i, r, g, b);
    }
    show();

    order = (order + 1) % size;
    delay(40);
}

