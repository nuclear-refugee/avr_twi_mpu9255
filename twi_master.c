/*
 * twi_master.c
 *
 * Created: 2017/8/17 上午 11:43:56
 *  Author: TwnET
 */ 

#include "twi_master.h"

void TwiComm_master_init(StrTWICommPack* p_property) {

	TWCR =  (1 << TWEN) |								// Enable TWI interface and release TWI pins.
			(1 << TWIE) | (1 << TWINT) |                // Enable TWI interupt and clear the interupt flag.
			(0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Enable start condition.
			(0 << TWWC);
}

void TwiComm_master_write(StrTWICommPack* p_property) {
	
	p_property->mode = TWI_COMM_MODE_MASTER;
	p_property->method = TW_WRITE;
	p_property->data_pointer = 0;

	TwiComm_start(p_property);

}

void TwiComm_master_write_byte(
	StrTWICommPack* p_property, uint8_t slave_id, uint8_t *p_data) {
	p_property->slave_id = slave_id;
	// TODO: Check that there is a valid pointer
	p_property->p_data = p_data;
	p_property->data_length = 1;

	TwiComm_master_write(p_property);
}

void TwiComm_master_write_bytes(
	StrTWICommPack* p_property, uint8_t slave_id, uint8_t* p_data, uint16_t length) {
	p_property->slave_id = slave_id;
	// TODO: Check that there is a valid pointer
	p_property->p_data = p_data;
	p_property->data_length = length;

	TwiComm_master_write(p_property);
}

void TwiComm_master_read(StrTWICommPack* p_property) {
	p_property->mode = TWI_COMM_MODE_MASTER;
	p_property->method = TW_READ;
	p_property->data_pointer = 0;

	TwiComm_start(p_property);
}

void TwiComm_master_read_byte(
	StrTWICommPack* p_property, uint8_t slave_id, uint8_t* p_data) {
	p_property->slave_id = slave_id;
	// TODO: Check that there is a valid pointer
	p_property->p_data = p_data;
	p_property->data_length = 1;

	TwiComm_master_read(p_property);
}

void TwiComm_master_read_bytes(
	StrTWICommPack* p_property, uint8_t slave_id, uint8_t* p_data, uint16_t length) {
	p_property->slave_id = slave_id;
	// TODO: Check that there is a valid pointer
	p_property->p_data = p_data;
	p_property->data_length = length;

	TwiComm_master_read(p_property);
}