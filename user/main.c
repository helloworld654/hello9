/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		接入onenet，上传数据和命令控制
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "usart.h"
#include "adc.h"

//C库
#include <string.h>


/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
	Led_Init();										//LED初始化
	
	Beep_Init();									//蜂鸣器初始化
	
	Key_Init();										//按键初始化

	Adc_Init();
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	
	float adcx=0,temp=0;

	extern float uv_value;
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266
	
	while(OneNet_DevLink())			//接入OneNET
		DelayXms(500);
	
	// Beep_Set(BEEP_ON);				//鸣叫提示接入成功
	DelayXms(250);
	// Beep_Set(BEEP_OFF);
	
	while(1)
	{
		
		switch(Keyboard())
		{
			case KEY0DOWN:
				
				if(led_status.Led4Sta == LED_OFF)
					Led4_Set(LED_ON);
				else
					Led4_Set(LED_OFF);
				
			break;
			
			case KEY2DOWN:
				
				if(led_status.Led5Sta == LED_OFF)
					Led5_Set(LED_ON);
				else
					Led5_Set(LED_OFF);
				
			break;
			
			case KEY3DOWN:
				
				if(led_status.Led6Sta == LED_OFF)
					Led6_Set(LED_ON);
				else
					Led6_Set(LED_OFF);
				
			break;
			
			case KEY1DOWN:
				
				if(led_status.Led7Sta == LED_OFF)
					Led7_Set(LED_ON);
				else
					Led7_Set(LED_OFF);
				
			break;
			
			default:
			break;
		}
		
		// if(++timeCount >= 500)								
		if(++timeCount >= 50)									
		{
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			
			timeCount = 0;
			ESP8266_Clear();

			// UsartPrintf(USART1, "befor:%f\r\n",temp);

			UsartPrintf(USART1, "uv_value:%f\r\n",uv_value);
		}
		
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);

		adcx=Get_Adc_Average(ADC_Channel_1,10);
		temp=(float)adcx*(3.3/4096)*100;

		if(temp<3)
			uv_value=0;
		else
			uv_value=temp;
		
		DelayXms(10);
	
	}

}
