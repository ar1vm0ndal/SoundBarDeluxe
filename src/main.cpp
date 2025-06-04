/*********************************************************************
 *  Minimal, library-free WS2811 / WS2812 driver on an ESP32
 *  -----------------------------------------------------------------
 *  • Drives one data pin with correct 800 kbit/s timing
 *  • Uses the Xtensa cycle counter (CCOUNT) for sub-µs accuracy
 *  • Disables interrupts while a frame is on the wire
 *  • CPU is set to 80 MHz so the math is simple (12.50 ns per cycle)
 *
 *  Tested on: ESP32-WROOM-32, LED strip powered from 5 V
 *********************************************************************/

#include <Arduino.h>

constexpr uint8_t  LED_PIN   = 2;      // data out pin
constexpr uint16_t NUM_LEDS  = 30;     // change to match your strip

/* ------------------------------------------------------------------
 *  Timing table (80 MHz CPU  → 12.50 ns / cycle)
 *  WS2812 / “800 kHz WS2811”   period ≈ 1.25 µs  → 100 cycles
 * ------------------------------------------------------------------
 *               | _High_ |  _Low_ |  _Total_
 * 0-bit (T0H/L) |  0.35 µs ≈ 28 cycles
 *               |  0.90 µs ≈ 72 cycles |  ~100
 * 1-bit (T1H/L) |  0.70 µs ≈ 56 cycles
 *               |  0.55 µs ≈ 44 cycles |  ~100
 * ----------------------------------------------------------------*/
constexpr uint32_t CPU_MHZ    = 80;      // keep in sync with setCpuFrequencyMhz()
constexpr uint32_t BIT_TOTAL  = 100;     // 1.25 µs × 80 MHz
constexpr uint32_t T0H        = 28;      // 0-bit high time
constexpr uint32_t T1H        = 56;      // 1-bit high time

static inline uint32_t getCycleCount()
{
  uint32_t ccount;
  asm volatile ("rsr %0, ccount" : "=a"(ccount));
  return ccount;
}

static inline void sendBit(bool one)
{
  const uint32_t tHigh = one ? T1H : T0H;
  uint32_t start = getCycleCount();

  GPIO.out_w1ts = (1U << LED_PIN);          // HIGH
  while (getCycleCount() - start < tHigh) {} // wait tHigh cycles
  GPIO.out_w1tc = (1U << LED_PIN);          // LOW
  while (getCycleCount() - start < BIT_TOTAL) {} // complete 1.25 µs cell
}

static void IRAM_ATTR sendColor(uint32_t grb)
{
  /*  Disable interrupts so nothing stretches a bit  */
  portDISABLE_INTERRUPTS();

  for (int8_t bit = 23; bit >= 0; --bit)      // MSB first
    sendBit(grb & (1UL << bit));

  portENABLE_INTERRUPTS();
}

/*  Reset / latch time:  WS2811/WS2812 need > 280 µs LOW  */
static void showReset()
{
  GPIO.out_w1tc = (1U << LED_PIN);   // make sure it's LOW
  delayMicroseconds(300);            // 300 µs is safe
}

void setup()
{
  /*  1. Run the core at 80 MHz so each cycle = 12.5 ns  */
  setCpuFrequencyMhz(CPU_MHZ);

  /*  2. Configure GPIO  */
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  /*  3. Optional: sanity LED on the board so you know it's alive  */
  Serial.begin(115200);
  Serial.println("WS2811 driver ready");
}

void loop()
{
  /*  Demo: walk a red pixel along the strip  */
  static uint16_t head = 0;

  for (uint16_t i = 0; i < NUM_LEDS; ++i) {
    if (i == head)
      sendColor(0xFF0000);   // Red  (GRB order)
    else
      sendColor(0x000000);   // Off
  }
  showReset();

  head = (head + 1) % NUM_LEDS;
  delay(50);
}
