#ifndef __GEC_USART_H__
#define __GEC_USART_H__

/*
	debug_init ���Դ���(USART1)������
			@baudrate: ���ڲ�����
*/
void debug_init(int baudrate);

/*
	debug_send_byte ���Դ��ڷ���1���ֽ�
*/
void debug_send_byte(char ch);

/*
	debug_send_str �������һ���ַ���
*/
void debug_send_str(char* s,int len);


extern char recv_buf[512]; 	//������յ����ַ���
extern int  recv_len;	  	//�յ����ַ�������; 
extern int  recv_cnt;      	//��������
extern int  recv_flag;		  	// ������ɱ�־   		1�������    	0δ���


#endif
