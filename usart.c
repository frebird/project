#include "sys.h"
#include "usart.h"	  

#include "includes.h"					//ucos Ê¹ÓÃ	  
#endif

#if 1
#pragma import(__use_no_semihosting)             
              
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef¡¯ d in stdio.h. */ 
FILE __stdout;       
   
_sys_exit(int x) 
{ 
	x = x; 
} 
 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0); 
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //enable receive
  	
u8 USART_RX_BUF[USART_REC_LEN];     

//bit15£¬	finished
//bit14£¬	receive 0x0d
//bit13~0£¬	number of data
u16 USART_RX_STA=0;       //status	  
  
void USART1_IRQHandler(void)
{
	u8 res;	
#if SYSTEM_SUPPORT_OS 		.
	OSIntEnter();    
#endif
	if(USART1->SR&(1<<5))//receive data
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x8000)==0)//unfinished
		{
			if(USART_RX_STA&0x4000)//receive 0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//error 
				else USART_RX_STA|=0x8000;	//finished
			}else /
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//error 
				}		 
			}
		}  		 									     
	} 
#if SYSTEM_SUPPORT_OS 	
	OSIntExit();  											 
#endif
} 
#endif										 
//Init IO USART1
//pclk2:PCLK2(Mhz)
//bound: 
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);
	mantissa=temp;				
	fraction=(temp-mantissa)*16; 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<0;   	//ENABLE PORTA Clock  
	RCC->APB2ENR|=1<<4;  	//ENABLE USART1 Clock 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//set bound
 	USART1->BRR=mantissa; 		 
	USART1->CR1&=~(1<<15); 	
	USART1->CR1|=1<<3;  	//ENABLE USART1
#if EN_USART1_RX		  
	//ENABLE receive interupt
	USART1->CR1|=1<<2;  	
	USART1->CR1|=1<<5;    	//	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);
#endif
	USART1->CR1|=1<<13;  	
}














