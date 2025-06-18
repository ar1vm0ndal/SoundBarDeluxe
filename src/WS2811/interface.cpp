#include <Arduino.h>
#include "driver/rmt.h"
#include "interface.h"
            
constexpr gpio_num_t LED_PIN = GPIO_NUM_2;     //DIN pin


//RMT Clock(80MHz)-- Uses APB (Advanced Peripheral bus timing)
constexpr uint8_t  RMT_DIV   = 1;              // 80 MHz / 1


constexpr uint16_t T0H = 20;   // 0.5 µs / 12.5 ns
constexpr uint16_t T1H = 48;   // 0.6 µs/ 12.5 ns
constexpr uint16_t T0L = 100 - T0H;   //1.25 µs cell
constexpr uint16_t T1L = 100 - T1H;
constexpr uint16_t RESET_TICKS = 24000; // 300 µs reset






static inline rmt_item32_t bit0(){
   return rmt_item32_t{T0H, 1, T0L, 0}; 
}

static inline rmt_item32_t bit1(){
   return rmt_item32_t{T1H, 1, T1L, 0}; 
}

// One frame buffer: 24 bits of data * numLEDs  +  reset pulse 
static rmt_item32_t frame[NUM_LEDS * 24 + 1];


void initRmt(){
    rmt_config_t cfg{};                 //create cfg object and change properties after

    cfg.rmt_mode         = RMT_MODE_TX;
    cfg.channel          = RMT_CHANNEL_0;
    cfg.gpio_num         = LED_PIN;
    cfg.mem_block_num    = 1;           
    cfg.clk_div          = RMT_DIV;

    cfg.tx_config.loop_en         = false;
    cfg.tx_config.carrier_en      = false;
    cfg.tx_config.idle_output_en  = true;
    cfg.tx_config.idle_level      = RMT_IDLE_LEVEL_LOW;

    rmt_config(&cfg);
    rmt_driver_install(cfg.channel, 0, 0);
}


void setPixel(uint16_t idx, uint8_t r, uint8_t g, uint8_t b){
    uint32_t grb = (b << 16) | (r << 8) | g;       // BRG bit mask
    rmt_item32_t* p = &frame[idx * 24];

    for (int bit = 23; bit >= 0; --bit, ++p){
        *p = (grb & (1UL << bit)) ? bit1() : bit0();
    }
}

void show(){
    frame[NUM_LEDS * 24] = rmt_item32_t{ 0, 0, RESET_TICKS, 0 };
    rmt_write_items(RMT_CHANNEL_0, frame, NUM_LEDS * 24 + 1, true);
    rmt_wait_tx_done(RMT_CHANNEL_0, portMAX_DELAY);
}

void colorGradientGenerate(uint32_t initialColor, uint32_t finalColor, uint32_t* dataArray, uint16_t size){
    for (uint16_t i = 0; i < size; ++i){
        double multiplier = double(i) / (size - 1); 
        uint8_t red   = uint8_t(multiplier * ((finalColor >> 16) & 0xFF) + (1.0 - multiplier) * ((initialColor >> 16) & 0xFF));  //MSB
        uint8_t green = uint8_t(multiplier * (((finalColor & 0xFF00) >> 8) & 0xFF) + (1.0 - multiplier) * (((initialColor & 0xFF00) >> 8) & 0xFF));
        uint8_t blue  = uint8_t(multiplier * (finalColor & 0xFF) + (1.0 - multiplier) * (initialColor & 0xFF)); //LSB
        dataArray[i]  = (uint32_t(red)   << 16) | (uint32_t(green) <<  8) | uint32_t(blue);
    }
}
