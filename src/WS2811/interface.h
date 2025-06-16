#ifndef INTERFACE_H

#include <Arduino.h>
void initRmt();
void setPixel(uint16_t idx, uint8_t r, uint8_t g, uint8_t b);
void show();
void colorGradientGenerate(uint32_t initialColor, uint32_t finalColor, uint32_t* dataArray, uint16_t size);

#endif