#include <Arduino.h>

#define outpin 2
#define NUM_LEDS 100
#define CPU_FREQUENCY 80

constexpr uint32_t transmissionTime = 100; //full bit transmission cycle of 1.25 us * 80MHz
constexpr uint32_t zero = 28; //0.35 us/12.5ns = 28 cycles of high for 0 bit
constexpr uint32_t one = 56; //0.55 us/12.5ns = 44 cycles of high for 1 bit

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

static void IRAM_ATTR sendColor(uint32_t color){
  portDISABLE_INTERRUPTS(); //disable interrupts since very time sensitive
  for(int8_t bit = 23; bit >= 0; --bit){
    sendBit(color & (1UL << bit));
  }

  portENABLE_INTERRUPTS(); //enable interrupts again
}

static void reset(){
  GPIO.out_w1tc = (1U << outpin);
  delayMicroseconds(300); // WS2811 demands > 280us of LOW time between LED transmission cycles
}

void setup(){
  setCpuFrequencyMhz(CPU_FREQUENCY);
  pinMode(outpin, OUTPUT);
  digitalWrite(outpin, LOW);
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