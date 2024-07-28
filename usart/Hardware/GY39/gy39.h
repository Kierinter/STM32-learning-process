#ifndef __GY39_H__
#define __GY39_H__
#include "stm32f4xx.h"
enum GY39MODE{GUANGZHAO,OTHER};
extern unsigned char gy39_mode;
extern unsigned char recvbuf[32];
extern unsigned char data_len;
extern unsigned char recv_ok;


void gy39_init(void);
void gy39_start(unsigned char mode);
void get_gy39_data(unsigned int data[]);
#endif
