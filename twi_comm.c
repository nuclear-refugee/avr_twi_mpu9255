/*
 * twi_comm.c
 *
 * Created: 2017/8/17 上午 11:59:42
 *  Author: TwnET
 */ 

#include "twi_comm.h"
#include "twi_slave.h"
#include "twi_master.h"

StrTWICommPack* main_twi_comm;

void TwiComm_init() {

	// SCL freq = (CPU Clock) / (16 + 2(TWBR) * (prescale value))
	// 400 kHz = (16MHz) / (16 + 2(12) * (1))
	TWBR = 12;
	TWSR = 0;   // TWPS = 0 => prescale value = 1

	// Setup TWI Control register for Initialize TWI Module
	TWCR =	(1 << TWEN) |						// Enable TWI interface and release TWI pins.
			(0 << TWIE) | (0 << TWINT) |                // Disable TWI interrupt.
			(0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // No signal requests.
			(0 << TWWC);
	
}

void TwiComm_start(StrTWICommPack* p_property) {
	
	// register StrTWICommPack into main_twi_comm,
	// in order to used by ISR(TWI_vect)
	main_twi_comm =  p_property;

	if(main_twi_comm->mode == TWI_COMM_MODE_MASTER) {
		TwiComm_master_init(p_property);
	}
	else {
		TwiComm_slave_init(p_property);
	}


}

void TwiComm_stop() {

	// Setup TWI Control register for stop operation
	TWCR = (1 << TWEN) |                                  // Enable TWI interface and release TWI pins.
	       (0 << TWIE) | (1 << TWINT) |                   // Disable TWI interupt and clear the interupt flag.
	       (0 << TWEA) | (0 << TWSTA) | (1 << TWSTO) |    // Send stop condition.
	       (0 << TWWC);

}

void TwiComm_start_ack(uint8_t is_ack) {
	if(is_ack) {
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
	}
	else {
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
	}
	return;
}

ISR(TWI_vect) {
	// printf("Status : %02X\n", TWSR & TWI_STATUS_MASK);
	switch(TWSR & TWI_STATUS_MASK) {
	case TW_START:
	case TW_REP_START:
		// There is only for master mode, don't need to check mode

		// Reset the TwiComm packet data pointer 
		// and wait for start transmit/received data
		main_twi_comm->data_pointer = 0;

		TWDR =	(main_twi_comm->slave_id << 1) |	
				(main_twi_comm->method);
		
		// Send SLA+R/W and wait for slave's ACK
		TwiComm_start_ack(USE_NACK);

		break;
	
	case TW_MT_SLA_ACK:
	case TW_MT_DATA_ACK:

		if( main_twi_comm->data_pointer < main_twi_comm->data_length ) {
			
			// Sending data and count-up data_pointer
			TWDR = main_twi_comm->p_data[main_twi_comm->data_pointer++];

			TwiComm_start_ack(USE_NACK);
		}
		else {
			// at end of p_data, issue the stop operation
			TwiComm_stop();	
		}
		break;

	case TW_MR_SLA_ACK:
		if( main_twi_comm->data_length == 1)
			TwiComm_start_ack(USE_NACK);
		else
			TwiComm_start_ack(USE_ACK);
		break;
	case TW_MR_DATA_ACK:
		// Receiving data and count-up data_pointer
		main_twi_comm->p_data[main_twi_comm->data_pointer++] = TWDR;
		
		if(main_twi_comm->data_pointer < main_twi_comm->data_length - 1) {
					
			TwiComm_start_ack(USE_ACK);
		}
		else {
			// at end of p_data, issue the stop operation (NACK)
			TwiComm_start_ack(USE_NACK);
		}
		break;

	case TW_MT_SLA_NACK:

		// TODO: Need to a counter for fail record
		// Repeat start ( The slave device may be busy )
		TwiComm_start(main_twi_comm);
		break;
	
	case TW_MT_DATA_NACK:

		TwiComm_init();
		break;

	case TW_MR_SLA_NACK:
		
		// TODO: Need to a counter for fail record
		// Repeat start ( The slave device may be busy )
		TwiComm_start(main_twi_comm);
		break;

	case TW_MR_DATA_NACK:

		main_twi_comm->p_data[main_twi_comm->data_pointer++] = TWDR;
		TwiComm_stop();
		break;
		
		
	}
}





