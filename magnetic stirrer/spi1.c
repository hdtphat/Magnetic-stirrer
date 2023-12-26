#include "stm32f10x.h"                  // Device header
#include "spi1.h"
#include <stdint.h>

void SPI1_InitPort(void){
	RCC->APB2ENR |=  (1<<2);  // Enable GPIOA clock
	GPIOA->CRL = 0;
	GPIOA->CRL |= (11U<<20);   // PA5 (SCK) AF output Push Pull
	GPIOA->CRL |= (11U<<28);   // PA7 (MOSI) AF output Push Pull
	GPIOA->CRL |= (1<<26);    // PA6 (MISO) Input mode (floating)
	GPIOA->CRL |= (3<<16);    // PA4 used for CS, GPIO Output 
}

void SPI1_Setup(void){
	RCC->APB2ENR |= (1U<<12);  		// Enable SPI1 CLock
	SPI1->CR1 &= ~(1U<<0);   			// CPOL= 0
	SPI1->CR1 &= ~(1U<<1);			// CPHA= 0
	SPI1->CR1 |= (1U<<2);  				// MSTR = 1 -> Master Mode
	SPI1->CR1 |= (4U<<3);  				// BR[2:0] = 100: fPCLK/32
	SPI1->CR1 &= ~(1U<<7);  			// LSBFIRST = 0 -> MSB first
	SPI1->CR1 |= (1U<<8) | (1U<<9);  	// SSM=1, SSi=1 -> Software Slave Management
	SPI1->CR1 &= ~(1U<<10);  			// RXONLY = 0 -> full-duplex
	SPI1->CR1 &= ~(1U<<11);  			// DFF=0 -> 8 bit data frame
	SPI1->CR2 = 0;
}

void SPI1_Enable (void){
	// SPE=1, Peripheral enabled
	SPI1->CR1 |= (1U<<6);   					
}

void SPI1_Disable (void){
	// SPE=0, Peripheral Disabled
	SPI1->CR1 &= ~(1U<<6);   
}

void SPI1_NSSenable (void){
	GPIOA->BSRR |= (1U<<4)<<16;
}

void SPI1_NSSdisable (void){
	GPIOA->BSRR |= (1U<<4);
}

void SPI1_Transmit (uint8_t *Tx_data_array, int size){	
	int i=0;
	while (i<size){
		// wait for TXE bit to set -> This will indicate that the buffer is empty
		while (!((SPI1->SR)&(1<<1))) {} 
		// load the data into the Data Register
		SPI1->DR = Tx_data_array[i];  
		// increase i for later transmition
		i++;
	}	
	// wait for TXE bit to set -> make sure that the buffer is empty
	while (!((SPI1->SR)&(1<<1))) {}  
	// wait for BSY bit to Reset -> make sure that SPI is not busy in communication	
	while (((SPI1->SR)&(1<<7))) {}  
	// clear the Overrun flag by reading DR and SR
	uint32_t clear_flag = SPI1->DR;
	clear_flag = SPI1->SR;
}

void SPI1_Receive (uint8_t *RX_data_array, int size){
	while (size){
		// wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		while (((SPI1->SR)&(1<<7))) {}
		// send dummy data as any SPI device can only send data after recive some
		SPI1->DR = 0;  
		// Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
		while (!((SPI1->SR) &(1<<0))){}
		*RX_data_array++ = (SPI1->DR);
		size--;
	}	
}

void SPI1_TransmitSingleByte (uint8_t byte){
	// wait for TXE bit to set -> This will indicate that the buffer is empty
	while (!((SPI1->SR)&(1<<1))) {} 
	// load the data into the Data Register
	SPI1->DR = byte;  
	// wait for TXE bit to set -> make sure that the buffer is empty
	while (!((SPI1->SR)&(1<<1))) {}  
	// wait for BSY bit to Reset -> make sure that SPI is not busy in communication	
	while (((SPI1->SR)&(1<<7))) {}  
	// clear the Overrun flag by reading DR and SR
	uint32_t clear_flag = SPI1->DR;
	clear_flag = SPI1->SR;
}
