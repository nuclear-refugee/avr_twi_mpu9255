/*
 * twi_salve.c
 *
 * Created: 2017/8/17 上午 11:54:50
 *  Author: TwnET
 */ 

 #include "twi_slave.h"

 void TwiComm_slave_init(StrTWICommPack* p_property) {
	// Loading address into TWI address register
	TWAR = p_property->slave_id << 1;
	
	// Enable TWI, Enable TWI Interrupt, Clear Interrupt Flag, Enable Match Ack
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
 }

 void TwiComm_slave_read(StrTWICommPack* p_property) {
	// Not used
	p_property->mode = TWI_COMM_MODE_SLAVE;
	p_property->method = TW_READ;

 }

 void TwiComm_slave_write(StrTWICommPack* p_property) {
	
 }
