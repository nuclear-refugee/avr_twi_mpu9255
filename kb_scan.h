/*
 * kb_scan.h
 *
 * Created: 2017/7/26 上午 08:20:08
 *  Author: TwnET
 */ 


#ifndef KB_SCAN_H_
#define KB_SCAN_H_

/****** Definition for keyboard scanning ******/

#define KB_ROW 4
#define KB_COL 4

#define KB_STATUS_FALLING	0
#define KB_STATUS_RISING	1

/****** Keyboard handle function ******/
void scan_keyboard();
int scan_row(int row);
void init_scan_io();
void init_scan_mat();
char get_kb_input();


#endif /* KB_SCAN_H_ */