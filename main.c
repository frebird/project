#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "dht11.h"

#include "beep.h"

#define MaxData (256)
#define MyAddr (0x01)
#define SAT (0x02)
#define Check1 (0x0D)
#define Check2 (0x0A)
 typedef struct{
	 uint8_t temperature;
	 uint8_t humidity;
	 uint8_t SendBuff[MaxData];	 
	 uint8_t RecBuff[MaxData];	
 }DataBus;
 
DataBus TxData={0,0,{0},{0}};

extern  DataBus TxData;
void SendData(DataBus*dat,uint8_t len, uint8_t op);
void SetReg(u8* USART_RX_BUF);

int main(void)
{ 
	u8 t=0;		
	uint8_t len=0;
	u8 temperature;  	    
	u8 humidity;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);  
	uart_init(9600);		//bound 9600
	
	LED_Init();					//Init LED 
 	LCD_Init();					//Init LCD  	
	BEEP_Init();
	
 	POINT_COLOR=RED;//Set red  
	LCD_ShowString(30,50,200,16,16,"Team Project");	
	LCD_ShowString(30,70,200,16,16,"DHT11 TEST");	
	LCD_ShowString(30,90,200,16,16,"LEI/MASUD/BEN");
	LCD_ShowString(30,110,200,16,16,"2020/4/16");
 	while(DHT11_Init())	//init dht11	
	{
		LCD_ShowString(30,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(30,130,200,16,16,"DHT11 OK");
	POINT_COLOR=BLUE;//set blue 
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	
#if __LCD_1602_H_	
  LCD1602Configuration();  
  LCD_ClearScreen();
	LCD_Write_Cmd(0x80);
	LCD_Write_Dat('A');
#endif	
	
	while(1)
	{	    	    
 		if(t%10==0)
		{									  
				
			DHT11_Read_Data(&(TxData.temperature),&(TxData.humidity));		//read temperature and humidity			
			LCD_ShowNum(30+40,150,TxData.temperature,2,16);		//show temperature	   		   
			LCD_ShowNum(30+40,170,TxData.humidity,2,16);			//show humidity	 	   
		}				   
	 	delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			LED0=!LED0;
		}
		
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;
			
			if(USART_RX_BUF[0]==SAT && USART_RX_BUF[1]==MyAddr){
				switch(USART_RX_BUF[2])
				{
					case 0x03:  //operation send
						SendData(&TxData,len, 0x03);
					break;
					
					case 0x04: //operation set
						SetReg(USART_RX_BUF);
						break;
				}
					
			   
			}
			USART_RX_STA=0;
		}
	}
}

void SendData(DataBus*dat,uint8_t len, uint8_t op)
{
	uint8_t temp,humi;
	
	temp=dat->temperature;
	humi=dat->humidity;
	
	dat->SendBuff[0]=SAT;
	dat->SendBuff[1]=MyAddr;
	dat->SendBuff[2]=op;
	dat->SendBuff[3]=len-1;
	dat->SendBuff[4]=temp;
	dat->SendBuff[5]=humi;
	dat->SendBuff[6]=0x0D;
	dat->SendBuff[7]=0x0A;
	for(int t=0;t<len+5;t++)
	{
		USART_SendData(USART1, TxData.SendBuff[t]);         //send data 
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//wait 
	}
	
}

void SetReg(u8* USART_RX_BUF) 
{
	if(USART_RX_BUF[3]==0x00)
	{
		 BEEP=0;
	}
	if(USART_RX_BUF[3]==0x01)
	{
		BEEP=1;
	}
}


