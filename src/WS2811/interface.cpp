#include <Arduino.h>


#define outpin 2

constexpr uint32_t transmissionTime = 100; //full bit transmission cycle of 1.25 us * 80MHz
constexpr uint32_t zero = 20; //0.5 us/12.5ns = 20 cycles of high for 0 bit
constexpr uint32_t one = 48; //0.6 us/12.5ns = 48 cycles of high for 1 bit

static inline uint32_t cycleCount(){
  uint32_t ccount;
  asm volatile ("rsr %0, ccount" : "=a"(ccount)); // Compiler directive to execute ASM and retrieve cycle count from ccount register
  return ccount;
}

static inline void sendBit(bool b){

  const uint32_t highTime = b? one : zero;
  uint32_t startTime = cycleCount();

  GPIO.out_w1ts = (1U << outpin);  //Send HIGH 
  while (cycleCount() - startTime < highTime){} //Wait for the corressponding bit time
  GPIO.out_w1tc = (1U << outpin); //Send LOW
  while (cycleCount() - startTime < transmissionTime){} //Wait for the corresponding bit low time 
}

void IRAM_ATTR sendColor(uint32_t color){
  portDISABLE_INTERRUPTS(); //disable interrupts since very time sensitive
  for(int8_t bit = 23; bit >= 0; --bit){
    sendBit(color & (1UL << bit));
  }

  portENABLE_INTERRUPTS(); //enable interrupts again
}

void reset(){
  GPIO.out_w1tc = (1U << outpin);
  delayMicroseconds(320); // WS2811 demands > 280us of LOW time between LED transmission cycles
}
