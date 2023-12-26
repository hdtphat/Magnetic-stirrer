#include "stm32f10x.h"                  // Device header
#include "MAX7219.h"
#include "spi1.h"
#include <stdint.h>

void MAX7219_sendData(uint8_t address, uint8_t data){
		uint8_t buf[2];
    buf[0] = address;
    buf[1] = data;
		SPI1_NSSenable();
    SPI1_Transmit (buf, 2);
		SPI1_NSSdisable();
}

void MAX7219_Init(void){
    // set decode mode: 0x09FF
    MAX7219_sendData(0x09,0xFF);
    // set intensity: 0x0A09
    MAX7219_sendData(0x0A, 9);
    // scan limit: 0x0B07
    MAX7219_sendData(0x0B, 7);
    // no shutdown, turn off display test
    MAX7219_sendData(0x0C, 1);
    MAX7219_sendData(0x0F, 0);
}

void MAX7219_displayINT(uint32_t num){
    // count the number of digits
    uint8_t count=1;
    uint32_t n = num;
    while(n/10){
        count++;
        n = n/10;
    }
    // set scanlimit
    MAX7219_sendData(0x0B, count-1);
    // dislay number
    for(uint8_t i=0; i<count;i++){
        MAX7219_sendData(i+1,num%10);
        num = num/10;
    }
}

void MAX7219_clear(void)
{
	MAX7219_sendData(0x01, 0x00);
}
