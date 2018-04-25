/*
 * twi_master.h
 *
 * Created: 2017/8/17 上午 11:33:27
 *  Author: TwnET
 */ 


#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

#include "twi_comm.h"

void TwiComm_master_init(StrTWICommPack* p_property);

void TwiComm_master_write(StrTWICommPack* p_property);

void TwiComm_master_write_byte(
	StrTWICommPack* p_property, uint8_t slave_id, uint8_t* p_data);

void TwiComm_master_write_bytes(
	StrTWICommPack* p_property, uint8_t slave_id, uint8_t* p_data, uint16_t length);

void TwiComm_master_read(StrTWICommPack* p_property);

void TwiComm_master_read_byte(
	StrTWICommPack* p_property, uint8_t slave_id, uint8_t* p_data);

void TwiComm_master_write_bytes(
	StrTWICommPack* p_property, uint8_t slave_id, uint8_t* p_data, uint16_t length);




#endif /* TWI_MASTER_H_ */