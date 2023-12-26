#ifndef MAX7219_H
#define MAX7219_H
#include <stdint.h>

void MAX7219_sendData(uint8_t address, uint8_t data);
void MAX7219_Init(void);
void MAX7219_displayINT(uint32_t num);
void MAX7219_clear(void);

#endif
