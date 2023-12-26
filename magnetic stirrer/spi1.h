#ifndef SPI1_H
#define SPI1_H

// SPI1 setup functions
void SPI1_InitPort(void);
void SPI1_Setup(void);

// SPI1 control functions
void SPI1_Enable (void);
void SPI1_Disable (void);
void SPI1_NSSenable (void);
void SPI1_NSSdisable (void);

// SPI1 communicate function
void SPI1_Transmit (uint8_t *Tx_data_array, int size);
void SPI1_Receive (uint8_t *RX_data_array, int size);
void SPI1_TransmitSingleByte (uint8_t byte);

#endif