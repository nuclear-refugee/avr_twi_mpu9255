/*
 * twi_comm.h
 *
 * Created: 2017/8/17 上午 11:51:48
 *  Author: TwnET
 */ 


#ifndef TWI_COMM_H_
#define TWI_COMM_H_

#define TWI_COMM_MODE_SLAVE 0
#define TWI_COMM_MODE_MASTER 1

#include "ASA_Lib.h"
#include <util/twi.h>
#include <avr/interrupt.h>

#define _BV(X) (0x01 << X)
#define TWI_STATUS_MASK	0xF8

#define USE_ACK		(1)
#define USE_NACK	(0)

typedef struct {
/*
	If use slave mode then set mode = TWI_COMM_MODE_SALVE
	If use master mode then set mode = TWI_COMM_MODE_MASTER
*/
	uint8_t mode;
/*
	While there is slave mode, Slave ID is self-id
	While there is master mode, Slave ID is remote device id
*/
	uint8_t slave_id;
//	the method property is for master mode
	uint8_t method;
//	context for data communication
	uint8_t* p_data;
	uint16_t data_pointer;
	uint16_t data_length;

} StrTWICommPack;

StrTWICommPack* main_twi_comm;

void TwiComm_init();

void TwiComm_start(StrTWICommPack* p_property);

void TwiComm_stop();

void TwiComm_start_ack(uint8_t is_ack);

ISR(TWI_vect);

#endif /* TWI_COMM_H_ */