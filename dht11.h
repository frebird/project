#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   


//IO direction
#define DHT11_IO_IN()  {GPIOG->MODER&=~(3<<(9*2));GPIOG->MODER|=0<<9*2;}	//PG9 input
#define DHT11_IO_OUT() {GPIOG->MODER&=~(3<<(9*2));GPIOG->MODER|=1<<9*2;} 	//PG9 output 
////IO operation											   
#define	DHT11_DQ_OUT PGout(9) 
#define	DHT11_DQ_IN  PGin(9)   

u8 DHT11_Init(void);//Init DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//read temperature and humidity
u8 DHT11_Read_Byte(void);//read one byte
u8 DHT11_Read_Bit(void);//read bit
u8 DHT11_Check(void);//check DHT11
void DHT11_Rst(void);//reset dht11 
#endif















