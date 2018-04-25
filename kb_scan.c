/*
 * kb_scan.c
 *
 * Created: 2017/7/26 上午 08:21:44
 *  Author: TwnET
 */ 
#include <avr\io.h>
#include <avr\interrupt.h>
 #include "kb_scan.h"

 /****** Data structure for keyboard scan state *******/

 char kb_state[KB_ROW][KB_COL];
 //char kb_asci[KB_ROW*KB_COL] = {'c','=','/','*','-','3','6','9','+','2','5','8','0','1','4','7'};
 char kb_asci[KB_ROW*KB_COL] = {'F','E','D','C','B','3','6','9','A','2','5','8','0','1','4','7'};

 void scan_keyboard() {
	 static int row_index = 0;
	 if(row_index == KB_ROW)
	 row_index = 0;

	 // Enable Low level on row_index
	 PORTA = 0x0f & (~(0x01 << row_index));

	 scan_row(row_index);

	 row_index++;
 }

 int scan_row(int row) {

	 for(int i=0; i<KB_COL; i++) {
		 if(PINA & (0x10 << i))
		 kb_state[KB_ROW-row-1][KB_COL-i-1] = 0;
		 else {
			 kb_state[KB_ROW-row-1][KB_COL-i-1] = 1;
		 }
	 }
 }

 void init_scan_io()
 {
	 printf("Initialize the Board !\n");

	 // Initialize the PORTA for scanning keyboard
	 DDRA = 0x0f;
	 PORTA = 0x0f;
 }

 void init_scan_mat()
 {
	 // Initialize the  keyboard status matrix
	 for(int i=0; i<KB_ROW; i++) {
		 for(int j=0; j<KB_COL; j++) {
			 kb_state[KB_ROW][KB_COL] = KB_STATUS_FALLING;
		 }
	 }
 }

 char get_kb_input() {
	 for(int i=0; i<KB_ROW; i++) {
		 for(int j=0; j<KB_COL; j++) {
			 if(kb_state[i][j])
			 return kb_asci[KB_ROW*i + j];
		 }
	 }
	 return '\0';
 }