/************************************************************************/
/* MPU9255 with TWI COMM library, which calibration first and polling data                                                                     
/*
/* Author: TwnET
/* Last Build: 2018/04/25
/************************************************************************/


// F_CPU is used by utils/delay.h for _delay_ms function calculation delay ticks
#define F_CPU 11059200UL

// Main header for ASA M128 Dev board
#include "ASA_Lib.h"

#include "twi_comm.h"

#include <avr/pgmspace.h>

#define TWI_STATUS_MASK	0xF8

#define MPU9255_SLA		0b01101000

char ASA_ID_set(unsigned char ASA_ID);

	typedef struct {
		uint8_t acc_x_h;
		uint8_t acc_x_l;
		uint8_t acc_y_h;
		uint8_t acc_y_l;
		uint8_t acc_z_h;
		uint8_t acc_z_l;
	} StrIMU_Acc_packet;

	typedef struct {
		uint8_t gryo_x_h;
		uint8_t gryo_x_l;
		uint8_t gryo_y_h;
		uint8_t gryo_y_l;
		uint8_t gryo_z_h;
		uint8_t gryo_z_l;
	} StrIMU_Gryo_packet;

union content {
	int16_t numbersigned;
	uint16_t numberunsigned;
	uint8_t hilobyte[2];
}; // 0=high, 1=low

int16_t convert15to16bits(uint8_t* dest){

	uint8_t bitMask = 0x80;
	uint8_t signedOrUnsigned;

	

	union content testvalue;

	testvalue.hilobyte[0] = dest[1];
	testvalue.hilobyte[1] = dest[0];

	signedOrUnsigned = testvalue.hilobyte[1]&bitMask;

	if (signedOrUnsigned==0x00){
		testvalue.numberunsigned = testvalue.numberunsigned>>1;
	}
	else{
		testvalue.numbersigned = (uint16_t)testvalue.numbersigned>>1;
		testvalue.numberunsigned |= 1<<15;
	}

	return testvalue.numbersigned;
}

uint16_t convert16to15bits(int16_t inputnumber,uint8_t* dest){

	uint8_t bitMask = 0x80;
	uint8_t signedOrUnsigned;
	uint8_t forCopy;
	uint8_t hiByte, loByte;

	union content testvalue;

	testvalue.numbersigned= inputnumber;
	testvalue.numbersigned = (uint16_t)testvalue.numbersigned<<1;

	dest[1] = testvalue.hilobyte[0];
	dest[0] = testvalue.hilobyte[1];
	forCopy=testvalue.hilobyte[0];
	testvalue.hilobyte[0]=testvalue.hilobyte[1];
	testvalue.hilobyte[1]=forCopy;

	return testvalue.numberunsigned;
}

int32_t total_value_ax = 0;
int32_t total_value_ay = 0;
int32_t total_value_az = 0;
int32_t total_value_gx = 0;
int32_t total_value_gy = 0;
int32_t total_value_gz = 0;

uint16_t count = 0;

void measure(StrIMU_Acc_packet* p_accPacket_t, StrIMU_Gryo_packet* p_gryoPacket_t) {
	total_value_ax += p_accPacket_t->acc_x_h << 8 | p_accPacket_t->acc_x_l;
	total_value_ay += p_accPacket_t->acc_y_h << 8 | p_accPacket_t->acc_x_l;
	total_value_az += (p_accPacket_t->acc_z_h << 8 | p_accPacket_t->acc_x_l) - 16384;
	total_value_gx += p_gryoPacket_t->gryo_x_h << 8 | p_gryoPacket_t->gryo_x_l;
	total_value_gy += p_gryoPacket_t->gryo_y_h << 8 | p_gryoPacket_t->gryo_y_l;
	total_value_gz += p_gryoPacket_t->gryo_z_h << 8 | p_gryoPacket_t->gryo_z_l;
	count++;
}

int16_t getOffsetData(uint8_t reg, uint8_t * tx_data, StrTWICommPack packet)
{
	/** Read OFFSET register which contain default offset value **/
	// Send target register address First
	tx_data[0] = reg;
	TwiComm_master_write_byte(&packet, MPU9255_SLA, tx_data);
	// Wait for Slave [ACK] and finish write operation
	_delay_ms(1);
	
	// Get OFFSET data
	uint8_t buffer[2] = {0, 0};
	TwiComm_master_read_bytes(&packet, MPU9255_SLA, buffer, 2);
	// Wait for Slave [ACK] and finish write operation
	_delay_ms(1);
	
	int16_t default_value = convert15to16bits(buffer);
	
	return default_value;
}

void setOffsetData(uint8_t reg, int16_t new_offset, uint8_t * tx_data, StrTWICommPack packet)
{
	/** Write OFFSET register which contain offset value **/
	// Send target register address First
	tx_data[0] = reg;
	convert16to15bits(new_offset, &(tx_data[1]));
	TwiComm_master_write_bytes(&packet, MPU9255_SLA, tx_data, 3);
	_delay_ms(10);
}

void main() {
	
	// Initialize the ASA M128 Library
	ASA_M128_set();

	ASA_ID_set(1);

	TwiComm_init();

	StrTWICommPack packet;

	uint8_t rx_data[128];
	uint8_t tx_data[128];
	
	StrIMU_Acc_packet acc_data;
	StrIMU_Gryo_packet gryo_data;

	//tx_data[0] = 0x3B;
	//TwiComm_master_write_byte(&packet, MPU9255_SLA, tx_data);
	//_delay_ms(10);
//
	//TwiComm_master_read_bytes(&packet, MPU9255_SLA, rx_data, 1);
	//_delay_ms(10);
//
	//printf("Get Data Reg[%02X] =  %02X\n",
	  //tx_data[0],  rx_data[0]);
	//
	//while(1);


	/************************************************************************/
	/* Calibration MPU9250                                                  */
	/************************************************************************/
	
#define XA_OFFSET_H 0x77
#define YA_OFFSET_H 0x7A
#define ZA_OFFSET_H 0x7D
#define XG_OFFSET_H 0x13
#define YG_OFFSET_H 0x15
#define ZG_OFFSET_H 0x17
	
	
	int16_t default_a_x = getOffsetData(XA_OFFSET_H, tx_data, packet);
	int16_t default_a_y = getOffsetData(YA_OFFSET_H, tx_data, packet);
	int16_t default_a_z = getOffsetData(ZA_OFFSET_H, tx_data, packet);
	int16_t default_g_x = getOffsetData(XG_OFFSET_H, tx_data, packet);
	int16_t default_g_y = getOffsetData(YG_OFFSET_H, tx_data, packet);
	int16_t default_g_z = getOffsetData(ZG_OFFSET_H, tx_data, packet);	

	printf("Default AX offset : %d\n", default_a_x);
	printf("Default AY offset : %d\n", default_a_y);
	printf("Default AZ offset : %d\n", default_a_z);
	printf("Default GX offset : %d\n", default_g_x);
	printf("Default GY offset : %d\n", default_g_y);
	printf("Default GZ offset : %d\n", default_g_z);
	
	// Capture 1000 samples
	for(int i=0; i<1000; i++) {
		tx_data[0] = 0x3B;
		TwiComm_master_write_byte(&packet, MPU9255_SLA, tx_data);
		_delay_ms(1);

		TwiComm_master_read_bytes(&packet, MPU9255_SLA, &acc_data, sizeof(StrIMU_Acc_packet));
		_delay_ms(1);

		tx_data[0] = 0x43;
		TwiComm_master_write_byte(&packet, MPU9255_SLA, tx_data);
		_delay_ms(1);

		TwiComm_master_read_bytes(&packet, MPU9255_SLA, &gryo_data, sizeof(StrIMU_Gryo_packet));
		_delay_ms(1);

		measure(&acc_data, &gryo_data);
	}

	// Compute estimate offset with pre-scale 16 (OFFSET-reg with default x16 prescale)
	int avg_ax = total_value_ax / count / 16 ;
	int avg_ay = total_value_ay / count / 16 ;
	int avg_az = total_value_az / count / 16 ;
	
	int avg_gx = total_value_gx / count / 16 ;
	int avg_gy = total_value_gy / count / 16 ;
	int avg_gz = total_value_gz / count / 16 ;
	printf("Estimate AX offset : %d\n", default_a_x - avg_ax);
	printf("Estimate AY offset : %d\n", default_a_y - avg_ay);
	printf("Estimate AZ offset : %d\n", default_a_z - avg_ay);
	printf("Estimate GX offset : %d\n", default_g_x - avg_gx);
	printf("Estimate GY offset : %d\n", default_g_x - avg_gy);
	printf("Estimate GZ offset : %d\n", default_g_x - avg_gz);

	_delay_ms(1000);
	
	setOffsetData(XA_OFFSET_H, default_a_x - avg_ax, tx_data, packet);
	setOffsetData(YA_OFFSET_H, default_a_y - avg_ay, tx_data, packet);
	setOffsetData(ZA_OFFSET_H, default_a_z - avg_ay, tx_data, packet);
	setOffsetData(XG_OFFSET_H, default_g_x - avg_gx, tx_data, packet);
	setOffsetData(YG_OFFSET_H, default_g_y - avg_gy, tx_data, packet);
	setOffsetData(ZG_OFFSET_H, default_g_z - avg_gz, tx_data, packet);

	// Clear measure variable
	total_value_ax = 0;
	total_value_ay = 0;
	total_value_az = 0;
	total_value_gx = 0;
	total_value_gy = 0;
	total_value_gz = 0;
	count = 0;

	for(int i=0; i<1000; i++) {
		tx_data[0] = 0x3B;
		TwiComm_master_write_byte(&packet, MPU9255_SLA, tx_data);
		_delay_ms(1);

		TwiComm_master_read_bytes(&packet, MPU9255_SLA, &acc_data, sizeof(StrIMU_Acc_packet));
		_delay_ms(1);

		tx_data[0] = 0x43;
		TwiComm_master_write_byte(&packet, MPU9255_SLA, tx_data);
		_delay_ms(1);

		TwiComm_master_read_bytes(&packet, MPU9255_SLA, &gryo_data, sizeof(StrIMU_Gryo_packet));
		_delay_ms(1);

		measure(&acc_data, &gryo_data);
		
	}
	// Compute estimate offset with pre-scale 16 (OFFSET-reg with default x16 prescale)
	avg_ax = total_value_ax / count / 16 ;
	avg_ay = total_value_ay / count / 16 ;
	avg_az = total_value_az / count / 16 ;
	
	avg_gx = total_value_gx / count / 16 ;
	avg_gy = total_value_gy / count / 16 ;
	avg_gz = total_value_gz / count / 16 ;
	printf("new Estimate AX offset : %d\n", default_a_x - avg_ax);
	printf("new Estimate AY offset : %d\n", default_a_y - avg_ay);
	printf("new Estimate AZ offset : %d\n", default_a_z - avg_ay);
	printf("new Estimate GX offset : %d\n", default_g_x - avg_gx);
	printf("new Estimate GY offset : %d\n", default_g_x - avg_gy);
	printf("new Estimate GZ offset : %d\n", default_g_x - avg_gz);


	/************************************************************************/
	/* Polling MPU9250 Gryo and Acc data									*/
	/************************************************************************/
	
	while(1) {

		tx_data[0] = 0x3B;
		TwiComm_master_write_byte(&packet, MPU9255_SLA, tx_data);
		_delay_ms(1);

		TwiComm_master_read_bytes(&packet, MPU9255_SLA, &acc_data, sizeof(StrIMU_Acc_packet));
		_delay_ms(10);

		tx_data[0] = 0x43;
		TwiComm_master_write_byte(&packet, MPU9255_SLA, tx_data);
		_delay_ms(1);

		TwiComm_master_read_bytes(&packet, MPU9255_SLA, &gryo_data, sizeof(StrIMU_Gryo_packet));
		_delay_ms(10);

		//printf("Get Data Reg[%02X:%02X] =  %02X%02X Dec: %d \n", 
			//tx_data[0], tx_data[0]+1,  rx_data[0], rx_data[1], rx_data[0] << 8 | rx_data[1]);
		printf("A_x:%5d \tA_y%5d \tA_z%5d \tG_x:%5d \tG_y:%5d \tG_z:%5d\n",
			acc_data.acc_x_h << 8 | acc_data.acc_x_l,
			acc_data.acc_y_h << 8 | acc_data.acc_y_l,
			acc_data.acc_z_h << 8 | acc_data.acc_z_l,
			gryo_data.gryo_x_h << 8 | gryo_data.gryo_x_l,
			gryo_data.gryo_y_h << 8 | gryo_data.gryo_y_l,
			gryo_data.gryo_z_h << 8 | gryo_data.gryo_z_l);

	}

}
