/*
 * twi_salve.h
 *
 * Created: 2017/8/17 上午 11:55:06
 *  Author: TwnET
 */ 


#ifndef TWI_SALVE_H_
#define TWI_SALVE_H_

#include "twi_comm.h"

void TwiComm_slave_init(StrTWICommPack* p_property);

void TwiComm_slave_read(StrTWICommPack* p_property);

void TwiComm_slave_write(StrTWICommPack* p_property);





#endif /* TWI_SALVE_H_ */