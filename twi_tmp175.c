// F_CPU is used by utils/delay.h for _delay_ms function calcutation delay ticks
#define F_CPU 11059200UL

// Main header for ASA M128 Dev board
#include "ASA_Lib.h"

#include "twi_comm.h"

#define TWI_STATUS_MASK	0xF8

char ASA_ID_set(unsigned char ASA_ID);

//#define _TWI_DEBUG

void main() {
	
	// Initialize the ASA M128 Library
	ASA_M128_set();

	ASA_ID_set(1);

	TwiComm_init();

	StrTWICommPack packet;

	uint8_t rx_data[128];
	uint8_t tx_data[128];

	tx_data[0] = 0x01;
	TwiComm_master_write_byte(&packet, 0b01001111, tx_data);
	_delay_ms(10);

	TwiComm_master_read_byte(&packet, 0b01001111, rx_data);
	_delay_ms(10);

	tx_data[1] = rx_data[0];
	tx_data[1] |= _BV(6) | _BV(5);

	TwiComm_master_write_bytes(&packet, 0b01001111, tx_data, 2);
	_delay_ms(10);

	tx_data[0] = 0x00;
	TwiComm_master_write_byte(&packet, 0b01001111, tx_data, 1);
	_delay_ms(10);

	while(1) {

		//TwiComm_master_read(&packet);
		TwiComm_master_read_bytes(&packet, 0b01001111, rx_data, 2);
		
		printf("Get Data %02X %02X %f\n", rx_data[0], rx_data[1], 0.0625*(rx_data[0] << 4 | rx_data[1] >> 4));

		_delay_ms(250);
	}

}