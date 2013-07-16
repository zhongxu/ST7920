//可用串口和并口的方式对ST7920操作 
 
#ifndef __ST7920_H__ 
#define __ST7920_H__ 
 
#include <avr/io.h> 
#include <inttypes.h> 
#include <util/delay.h> 
#include <string.h>  
//#include <macros.h>  
 
 
//---------------------------------------------------------------- 
//每行显示8个汉字，16个字符，汉字地址类似为0x80,00x81,0x82,...... 
//字符也使用这些地址，也就是字符对应于汉字左侧使用这些地址， 
//对应于汉字右侧的则没有地址，操作时只能使用写字符串函数先向前一个字符， 
//在接着写一个 
//---------------------------------------------------------------- 
 
#define PARALLEL	1 
 
#if PARALLEL == 1 
 
	#define LCD_CTRL_PORT 	PORTA 
	#define LCD_CTRL_DDR 	DDRA 
	#define LCD_CTRL_PIN 	PINA											 
	#define LCD_RS	5  //CS   第四引脚 
	#define LCD_RW	6	//SIO  第五引脚 
	#define LCD_E	7  //SCK  第六引脚 
	#define LCD_RST	0 
	//#define LCD_PSB	5 
 
	#define LCD_DATA_DDR	DDRC 
	#define LCD_DATA_PIN	PINC 
	#define LCD_DATA_PORT	PORTC 
 
 
	#ifdef LCD_RST 
		#ifdef LCD_PSB 
		#define LCD_CTRL_OUT()	LCD_CTRL_DDR |= (1<<(LCD_RS))|(1<<(LCD_RW))|(1<<(LCD_E))|(1<<(LCD_RST))|(1<<(LCD_PSB)) 
		#else 
		#define LCD_CTRL_OUT()	LCD_CTRL_DDR |= (1<<(LCD_RS))|(1<<(LCD_RW))|(1<<(LCD_E))|(1<<(LCD_RST)) 
		#endif	//#ifdef LCD_PSB 
	#else 
		#ifdef LCD_PSB 
		#define LCD_CTRL_OUT()	LCD_CTRL_DDR |= (1<<(LCD_RS))|(1<<(LCD_RW))|(1<<(LCD_E))|(1<<(LCD_PSB)) 
		#else 
		#define LCD_CTRL_OUT()	LCD_CTRL_DDR |= (1<<(LCD_RS))|(1<<(LCD_RW))|(1<<(LCD_E)) 
		#endif	//#ifdef LCD_PSB 
	#endif	//#ifdef LCD_RST 
 
 
	#define LCD_RS_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_RS) 
	#define LCD_RS_HIGH()	LCD_CTRL_PORT |= (1<<LCD_RS) 
 
	#define LCD_RW_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_RW) 
	#define LCD_RW_HIGH()	LCD_CTRL_PORT |= (1<<LCD_RW) 
 
	#define LCD_E_LOW()		LCD_CTRL_PORT &= ~(1<<LCD_E) 
	#define LCD_E_HIGH()	LCD_CTRL_PORT |= (1<<LCD_E) 
 
	#ifdef LCD_RST 
	#define LCD_RST_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_RST) 
	#define LCD_RST_HIGH()	LCD_CTRL_PORT |= (1<<LCD_RST) 
	#endif	//#ifdef LCD_RST 
 
	#ifdef LCD_PSB 
	#define LCD_PSB_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_PSB) 
	#define LCD_PSB_HIGH()	LCD_CTRL_PORT |= (1<<LCD_PSB) 
	#endif	//#ifdef LCD_PSB 
 
 
 
	#define LCD_DATA_IN()	LCD_DATA_DDR &= ~0xFF 
	#define LCD_DATA_OUT()	LCD_DATA_DDR |= 0xFF 
	#define LCD_DATA_GET()	LCD_DATA_PIN 
 
#else	//PARALLEL == 0,串口操作 
 
 
	#define LCD_CTRL_PORT 	PORTA 
	#define LCD_CTRL_DDR 	DDRA 
	#define LCD_CTRL_PIN 	PINA											 
	#define LCD_CS	5	//RS   第四引脚 
	#define LCD_SIO	6	//RW  第五引脚 
	#define LCD_SCK	7	//E  第六引脚 
	#define LCD_RST	0 
	//#define LCD_PSB	5 
 
	#define LCD_DATA_DDR	DDRC 
	#define LCD_DATA_PIN	PINC 
	#define LCD_DATA_PORT	PORTC 
 
 
	#ifdef LCD_RST 
		#ifdef LCD_PSB 
		#define LCD_CTRL_OUT()	LCD_CTRL_DDR |= (1<<(LCD_CS))|(1<<(LCD_SCK))|(1<<(LCD_RST))|(1<<(LCD_PSB)) 
		#else 
		#define LCD_CTRL_OUT()	LCD_CTRL_DDR |= (1<<(LCD_CS))|(1<<(LCD_SCK))|(1<<(LCD_RST)) 
		#endif	//#ifdef LCD_PSB 
	#else 
		#ifdef LCD_PSB 
		#define LCD_CTRL_OUT()	LCD_CTRL_DDR |= (1<<(LCD_CS))|(1<<(LCD_SCK))|(1<<(LCD_PSB)) 
		#else 
		#define LCD_CTRL_OUT()	LCD_CTRL_DDR |= (1<<(LCD_CS))|(1<<(LCD_SCK)) 
		#endif	//#ifdef LCD_PSB 
	#endif	//#ifdef LCD_RST 
 
	#define LCD_SIO_IN()	LCD_CTRL_DDR	&= ~(1<<LCD_SIO) 
	#define LCD_SIO_OUT()	LCD_CTRL_DDR	|= (1<<LCD_SIO) 
 
 
	#define LCD_CS_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_CS) 
	#define LCD_CS_HIGH()	LCD_CTRL_PORT |= (1<<LCD_CS) 
 
	#define LCD_SCK_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_SCK) 
	#define LCD_SCK_HIGH()	LCD_CTRL_PORT |= (1<<LCD_SCK) 
 
	#define LCD_SIO_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_SIO) 
	#define LCD_SIO_HIGH()	LCD_CTRL_PORT |= (1<<LCD_SIO) 
	#define LCD_SIO_GET()	((LCD_CTRL_PIN>>LCD_SIO)&0x01) 
 
	#ifdef LCD_RST 
	#define LCD_RST_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_RST) 
	#define LCD_RST_HIGH()	LCD_CTRL_PORT |= (1<<LCD_RST) 
	#endif	//#ifdef LCD_RST 
 
	#ifdef LCD_PSB 
	#define LCD_PSB_LOW()	LCD_CTRL_PORT &= ~(1<<LCD_PSB) 
	#define LCD_PSB_HIGH()	LCD_CTRL_PORT |= (1<<LCD_PSB) 
	#endif	//#ifdef LCD_PSB 
 
 
#endif	//#if PARALLEL == 1 
 
//已经基本参数 
#define LCD_X_MAX	127 
#define LCD_Y_MAX	63 
 
 
 
//每行的首地址,基于RT12864-4M型液晶 
#define LINE_ONE_ADDRESS   0x80 
#define LINE_TWO_ADDRESS   0x90 
#define LINE_THREE_ADDRESS 0x88 
#define LINE_FOUR_ADDRESS  0x98 
 
//基本指令集预定义 
#define LCD_DATA               	1         	//数据位 
#define LCD_COMMAND            	0		 	//命令位 
#define	LCD_CLEAR_SCREEN       	0x01 	 	//清屏	 
#define	LCD_ADDRESS_RESET      	0x02		//地址归零	  
#define	LCD_BASIC_FUNCTION	  	0x30		//基本指令集 
#define	LCD_EXTEND_FUNCTION	   	0x34		//扩充指令集 
 
//扩展指令集预定义 
#define	LCD_AWAIT_MODE			0x01	 	//待命模式	 
#define LCD_ROLLADDRESS_ON		0x03		//允许输入垂直卷动地址 
#define LCD_IRAMADDRESS_ON		0x02		//允许输入IRAM地址	 
#define	LCD_SLEEP_MODE			0x08	 	//进入睡眠模式 
#define	LCD_NO_SLEEP_MODE		0x0c	 	//脱离睡眠模式 
#define LCD_GRAPH_ON			0x36		//打开绘图模式 
#define LCD_GRAPH_OFF			0x34		//关闭绘图模式 
 
 
#ifndef LCD_COLOR 
#define LCD_COLOR_BLACK			1 
#define LCD_COLOR_WHITE			0 
#endif	//#ifndef LCD_COLOR 
 
 
 
 
extern void LCD_Init(void); 
extern void LCD_ClearRam(void); 
extern void LCD_GraphModeSet(unsigned char Select); 
extern void LCD_DisplayStrings_WithAddress(unsigned char ucAdd,const char *p); 
extern void LCD_DisplayStrings(unsigned char x,unsigned char y,const char *p); 
extern void LCD_DisplayChinese(unsigned char x,unsigned char y,const char *p); 
extern void LCD_Point(unsigned char x,unsigned char y,unsigned char color); 
 
 
 
 
 
#endif	//#ifndef __ST7920_H__ 

