#ifndef INTERFACE_H

#include <Arduino.h>
void IRAM_ATTR sendColor(uint32_t color);
void reset();
void colorGradientGenerate(uint32_t initialColor, uint32_t finalColor, uint32_t* dataArray, uint8_t size);

#endif