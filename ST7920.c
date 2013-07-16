#include "ST7920.h"   
   
   
#if PARALLEL != 1   
   
void LCD_SendByte(unsigned char a)   
{   
    unsigned char i,d;   
    LCD_CS_HIGH();   
    LCD_SIO_OUT();   
    for(i=0;i<8;i++)   
    {   
        LCD_SCK_LOW();  //clrbit(LCD_CTRL,E);     
        d = a&0x80;   
        if(d)   
            LCD_SIO_HIGH(); //setbit(LCD_CTRL,RW);   
        else   
            LCD_SIO_LOW();  //clrbit(LCD_CTRL,RW);   
        a<<=1;   
        LCD_SCK_HIGH();  //setbit(LCD_CTRL,E); //上升弦发送   
    }   
    LCD_CS_LOW();   
}   
   
   
#endif   
   
   
   
//*******************************************   
//*函数名称:void LCD_CheckBusy(void)    *   
//*函数功能:并行方式查忙                  *   
//*形式参数:无                               *   
//*形参说明:无                               *   
//*返回参数:无                               *   
//*使用说明:无                               *   
//*******************************************   
void LCD_CheckBusy(void)   
{   
#if PARALLEL == 1   
    unsigned char temp;   
    LCD_DATA_IN();  //P1 = 0xff;    //输入前置1   
    LCD_RS_LOW();   //RS = 0;       //指令   
    LCD_RW_HIGH();  //RW = 1;       //读模式   
    _delay_us(5);   //延时   
    LCD_E_HIGH();   //E  = 1;    //使能   
    do   
    {      
        temp = LCD_DATA_GET();  //temp = BUSY;   
    }   
    while( 0x80==(temp&0x80) ); //等待不忙,temp=0x00时不忙   
       
    LCD_E_LOW();    //E  = 0;   
    //_delay_ms(20);   
#else   
    unsigned char temp = 0xFC,temp2;   
    do   
    {   
        LCD_SendByte(temp);   
        temp = LCD_ReceiveByte();   
        temp2 = LCD_ReceiveByte();   
        temp &= 0x80;   
    }while( temp==0x80 );   
#endif  //#if PARALLEL == 1   
}   
   
   
//************************************************************************************   
//*函数名称:void LCD_Write(unsigned char Data_Command , unsigned char uc_Content ) *   
//*函数功能:并行模式向LCD发送数据或指令                                              *   
//*形式参数:unsigned char Data_Command , unsigned char uc_Content                    *   
//*形参说明:数据或指令的标志位,指令或数据的内容                                       *   
//*返回参数:无                                                                        *   
//*使用说明:无                                                                        *   
//************************************************************************************   
void LCD_Write( unsigned char Data_Command , unsigned char uc_Content )   
{    
#if PARALLEL == 1   
   
    LCD_CheckBusy();   
    LCD_DATA_OUT();   
   
    if(Data_Command)   
    {   
        LCD_RS_HIGH();  //RS = 1;   //数据   
        //LCD_RW_LOW(); //RW = 0;   //写模式   
    }   
    else    
    {   
        LCD_RS_LOW();   //RS = 0;   //指令   
        //LCD_RW_LOW(); //RW = 0;   //写模式   
    }   
    LCD_RW_LOW();       //RW = 0;   //写模式   
   
    LCD_DATA_PORT = uc_Content;//数据放到P1口上   
    LCD_E_HIGH();   //E = 1;   
    _delay_us(1);   //延时   
    //_nop_();//很重要    
    //_nop_();   
    //_nop_();   
    LCD_E_LOW();    //E = 0;           
#else   
    unsigned char temp;   
   
    LCD_CheckBusy();   
       
    if(Data_Command)   
    {   
        temp = 0xFA;    //RS = 1,数据; RW = 0,写模式   
    }   
    else    
    {   
        temp = 0xF8;    //RS = 0,指令; RW = 0,写模式   
    }   
    LCD_SendByte(temp);   
   
    temp = uc_Content&0xF0;   
    LCD_SendByte(temp);   
   
    temp = (uc_Content<<4)&0xF0;   
    LCD_SendByte(temp);   
#endif  //#if PARALLEL == 1                                                
}   
   
   
//********************************************   
//*函数名称:LCD_ReadData         *   
//*函数功能:并行读LCD数据                     *   
//*形式参数:无                                *   
//*形参说明:无                                *   
//*返回参数:读回的数据                        *   
//*使用说明:无                                *   
//********************************************   
unsigned char LCD_ReadData(void)   
{   
#if PARALLEL == 1   
    unsigned char uc_Content;   
    LCD_CheckBusy();   
    //_delay_us(2);   
   
    LCD_DATA_IN();  //P1 = 0xff;//输入前置1   
    LCD_RS_HIGH();  //RS = 1;   //数据   
    LCD_RW_HIGH();  //RW = 1;   //读模式   
   
    LCD_E_HIGH();   //E = 1;    //使能   
    _delay_us(1);   //延时很重要    
    uc_Content = LCD_DATA_GET();    //uc_Content = P1;    //P1口的内容放到变量中   
    LCD_E_LOW();    //E = 0;   
   
    _delay_us(1);   //延时很重要    
   
    return uc_Content;   
#endif  //#if PARALLEL == 1   
}   
   
   
   
   
   
//*************************************************************   
//*函数名称:void LCD_DDRAM_AddressSet(unsigned char ucDDramAdd) *   
//*函数功能:设定DDRAM(文本区)地址到地址计数器AC                *   
//*形式参数:unsigned char ucDDramAdd                                  *   
//*行参说明:具体地址                                          *   
//*格式说明:                                                  *   
//*     RS  RW  DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0               *   
//*     0   0   1   AC6 AC5 AC4 AC3 AC2 AC1 AC0               *   
//*返回参数:无                                                 *   
//*使用说明:                                                  *   
//* 第一行地址:80H~8FH                                         *   
//* 第二行地址:90H~9FH                                         *   
//* 第三行地址:A0H~AFH                                         *   
//* 第四行地址:B0H~BFH                                         *   
//*************************************************************   
void LCD_DDRAM_AddressSet(unsigned char ucDDramAdd)   
{   
    LCD_Write(LCD_COMMAND,LCD_BASIC_FUNCTION);          //基本指令集   
    LCD_Write(LCD_COMMAND,ucDDramAdd);              //设定DDRAM地址到地址计数器AC   
}   
   
   
//*************************************************************   
//*函数名称:void LCD_CGRAM_AddressSet(unsigned char ucCGramAdd) *   
//*函数功能:设定CGRAM(自定义字库区)地址到地址计数器AC         *   
//*形式参数:unsigned char ucCGramAdd                                  *   
//*形参说明:具体地址                                          *   
//*格式说明:                                                  *   
//*     RS  RW  DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0               *   
//*     0   0   0   1   AC5 AC4 AC3 AC2 AC1 AC0               *   
//*返回参数:无                                                 *   
//*使用说明:地址范围为40H~3FH                                  *   
//*************************************************************   
void LCD_CGRAM_AddressSet(unsigned char ucCGramAdd)   
{   
    LCD_Write(LCD_COMMAND,LCD_BASIC_FUNCTION);          //基本指令集   
    LCD_Write(LCD_COMMAND,ucCGramAdd);              //设定CGRAM地址到地址计数器AC   
}   
   
   
//*******************************************************************************   
//*函数名称:void LCD_GDRAM_AddressSet(unsigned char ucGDramAdd)                   *   
//*函数功能:设定GDRAM(图形区)地址到地址计数器AC                                 *   
//*形式参数:unsigned char ucGDramAdd                                                    *   
//*形参说明:具体地址值                                                          *   
//*     RS  RW  DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0                                 *   
//*     0   0   1   AC6 AC5 AC4 AC3 AC2 AC1 AC0                                 *   
//*     先设定垂直位置再设定水平位置(连续写入两个字节完成垂直和水平位置的设置)  *    
//*     垂直地址范围:AC6~AC0                                                    *   
//*     水平地址范围:AC3~AC0                                                    *   
//*返回参数:无                                                                  *   
//*使用说明:必须在扩展指令集的情况下使用                                        *   
//*******************************************************************************   
void LCD_GDRAM_AddressSet(unsigned char ucGDramAdd)   
{   
    LCD_Write(LCD_COMMAND,LCD_EXTEND_FUNCTION);         //扩展指令集   
    LCD_Write(LCD_COMMAND,ucGDramAdd);         
}   
   
   
   
//*******************************************   
//*函数名称:void LCD_Init(void) *   
//*函数功能:并行初始化LCD                    *   
//*形式参数:无                               *   
//*形参说明:无                               *   
//*返回参数:无                               *   
//*使用说明:无                               *   
//*******************************************   
void LCD_Init(void)   
{   
    LCD_CTRL_OUT();   
#if PARALLEL == 1   
    #ifdef LCD_RST   
        LCD_RST_LOW();   
        _delay_ms(10);   
        LCD_RST_HIGH(); //RST = 1;                  //复位后拉高，停止复位   
    #endif   
    #ifdef LCD_PSB   
        LCD_PSB_HIGH(); //PSB = 1;                  //选择并行传输模式     
    #endif   
#else   
    #ifdef LCD_RST   
        LCD_RST_LOW();   
        _delay_ms(100);   
        LCD_RST_HIGH(); //RST = 1;                  //复位后拉高，停止复位   
    #endif   
    #ifdef LCD_PSB   
        LCD_PSB_LOW();  //PSB = 0;                  //选择串并行传输模式    
    #endif   
#endif  //#if PARALLEL == 1   
   
   
    LCD_Write(LCD_COMMAND,LCD_BASIC_FUNCTION); //基本指令动作      
    LCD_Write(LCD_COMMAND,LCD_CLEAR_SCREEN);    //清屏，地址指针指向00H   
    LCD_Write(LCD_COMMAND,0x06);            //光标的移动方向   
    LCD_Write(LCD_COMMAND,0x0C);            //开显示，关游标   
   
}   
   
   
   
//********************************************************   
//*函数名称:void LCD_ClearRam(void)              *   
//*函数功能:并行清屏函数                               *   
//*形式参数:无                                            *   
//*形参说明:无                                            *   
//*返回参数:无                                            *   
//*使用说明:DDRAM填满20H,并设定DDRAM AC到00H           *   
//*格式说明:                                             *   
//*         RS  RW  DB7  DB6  DB5  DB4  DB3 DB2 DB1 DB0  *   
//*         0   0    0    0    0    0    0   0   0   1   *   
//********************************************************   
void LCD_ClearRam(void)   
{   
    LCD_Write(LCD_COMMAND,LCD_BASIC_FUNCTION);          //基本指令集   
    LCD_Write(LCD_COMMAND,LCD_CLEAR_SCREEN);            //清屏   
}   
   
   
//*****************************************************   
//*函数名称:void LCD_GraphModeSet(unsigned char bSelect) *   
//*函数功能:打开或关闭绘图显示                         *   
//*形式参数:unsigned char Select                      *   
//*形参说明:打开或关闭绘图显示的标志位,1打开,0关闭     *   
//*格式说明:                                          *   
//*     RS  RW  DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0       *   
//*     0   0   0   0   1   1   X   RE   G   X        *   
//*                                                   *   
//*     RE:0,基本指令集                                *   
//*     RE:1,扩充指令集                                *   
//*     G :0,绘图显示OFF                              *   
//*     G :1,绘图显示ON                               *   
//*返回参数:无                                         *   
//*使用说明:无                                         *   
//*****************************************************   
void LCD_GraphModeSet(unsigned char Select)   
{   
    LCD_Write(LCD_COMMAND,LCD_EXTEND_FUNCTION); //扩展指令集   
    if(Select)   
    {   
        LCD_Write(LCD_COMMAND,LCD_GRAPH_ON);        //打开绘图模式   
    }   
    else   
    {   
        LCD_Write(LCD_COMMAND,LCD_GRAPH_OFF);       //关闭绘图模式       
    }   
}   
   
   
//*********************************************************************************   
//*函数名称:void LCD_DisplayStrings_WithAddress(unsigned char ucAdd,unsigned char code *p)    *   
//*函数功能:在(文本区)ucAdd指定的位置显示一串字符(或是汉字或是ASCII或是两者混合)  *   
//*形式参数:unsigned char ucAdd,unsigned char code *p                             *   
//*形参说明:指定的位置,要显示的字符串                                             *   
//*地址必须是:80H~87H,90H~97H,88H~8FH,98H~8FH                                      *   
//*返回参数:无                                                                     *   
//*使用说明:使用之前要初始化液晶                                                *   
//*********************************************************************************   
void LCD_DisplayStrings_WithAddress(unsigned char ucAdd,const char *p)   
{   
    unsigned char length;   
    length = strlen(p);   
    LCD_Write(LCD_COMMAND,LCD_BASIC_FUNCTION);              //基本指令动作      
    LCD_DDRAM_AddressSet(ucAdd);   
    for( ;  length ; length -- )   
        LCD_Write(LCD_DATA, *(p++));                       
}   
   
   
//*********************************************************************************   
//*函数名称:void LCD_DisplayChinese(unsigned char x,unsigned char y,unsigned char code *p)    *   
//*函数功能:在(文本区)ucAdd指定的位置显示一串字符(或是汉字或是ASCII或是两者混合)  *   
//*形式参数:unsigned char x,unsigned char y,const char *p                         *   
//*形参说明:指定的位置,要显示的字符串                                             *   
//*地址必须是: x<8, y<4                                                          *   
//*返回参数:无                                                                     *   
//*使用说明:使用之前要初始化液晶                                                *   
//*********************************************************************************   
void LCD_DisplayChinese(unsigned char x,unsigned char y,const char *p)   
{   
    unsigned char length,addr;   
   
    y &= 0x03;  //y < 4   
    x &= 0x07;  //x < 8   
   
    switch( y )   
    {   
        case 0:   
            addr = 0x80;   
        break;   
   
        case 1:   
            addr = 0x90;   
        break;   
   
        case 2:   
            addr = 0x88;   
        break;   
   
        case 3:   
            addr = 0x98;   
        break;   
    }   
   
    addr += x;   
   
    length = strlen(p);   
   
    LCD_Write(LCD_COMMAND,LCD_BASIC_FUNCTION);              //基本指令动作      
    LCD_DDRAM_AddressSet(addr);   
   
    for( ;  length ; length -- )   
        LCD_Write(LCD_DATA, *(p++));   
}   
   
   
   
//*********************************************************************************   
//*函数名称:void LCD_DisplayStrings(unsigned char x,unsigned char y,unsigned char code *p)    *   
//*函数功能:在(文本区)ucAdd指定的位置显示一串ASCII                                *   
//*形式参数:unsigned char x,unsigned char y,const char *p                         *   
//*形参说明:指定的位置,要显示的字符串                                             *   
//*地址必须是: x<16, y<4                                                         *   
//*返回参数:无                                                                     *   
//*使用说明:使用之前要初始化液晶                                                *   
//*********************************************************************************   
void LCD_DisplayStrings(unsigned char x,unsigned char y,const char *p)   
{   
    unsigned char length,addr;   
    unsigned char AddrEO;   //x地址是奇偶   
    unsigned char Data1;   
   
    y &= 0x03;  //y < 4   
    x &= 0x0F;  //x < 16   
   
    switch( y )   
    {   
        case 0:   
            addr = 0x80;   
        break;   
   
        case 1:   
            addr = 0x90;   
        break;   
   
        case 2:   
            addr = 0x88;   
        break;   
   
        case 3:   
            addr = 0x98;   
        break;   
    }   
   
    addr += x/2;   
    AddrEO = x&0x01;   
   
    length = strlen(p);   
   
    LCD_Write(LCD_COMMAND,LCD_BASIC_FUNCTION);              //基本指令动作      
    LCD_DDRAM_AddressSet(addr);   
   
    if(AddrEO == 0x01)   
        {   
        Data1 = LCD_ReadData();   
        LCD_DDRAM_AddressSet(addr);   
        LCD_Write(LCD_DATA, Data1);   
        }   
   
    for( ;  length ; length -- )   
        LCD_Write(LCD_DATA, *(p++));   
}   
   
   
//*********************************************************************   
//*函数名称:void LCD_ImgDisplay(unsigned char code *img)         *   
//*函数功能:全屏显示128*64个象素的图形                                *   
//*形式参数:unsigned char code *img                                   *   
//*形参说明:要显示的图形                                              *   
//*液晶屏坐标说明:                                                    *   
//*      ________________128个像素______________________              *   
//*      |(0,0)                                    (7,0)|             *   
//*      |                                              |             *   
//*   64 |                                              |             *   
//*   个 |(0,31)                                  (7,31)|             *   
//*   像 |(8,0)                                   (15,0)|             *   
//*   素 |                                              |             *   
//*      |                                              |             *   
//*      |(8,31)                                 (15,31)|             *   
//*      |______________________________________________|             *   
//*                                                                   *   
//返回参数:无                                                         *   
//使用说明:此函数适用于CM12864-12型液晶                               *   
//*********************************************************************   
void LCD_ImgDisplay(unsigned char *img)   
{   
    unsigned char i,j;   
    LCD_GraphModeSet(0x00); //先关闭图形显示功能   
    for(j=0;j<32;j++)   
     {   
        for(i=0;i<8;i++)   
         {   
            LCD_Write(LCD_COMMAND,0x80+j);      //设定垂直坐标   
            LCD_Write(LCD_COMMAND,0x80+i);      //设定水平坐标   
            LCD_Write(LCD_DATA,img[j*16+i*2]);  //放入数据高字节   
            LCD_Write(LCD_DATA,img[j*16+i*2+1]);    //放入数据低字节   
         }   
     }   
    for(j=32;j<64;j++)   
     {   
        for(i=0;i<8;i++)   
         {   
            LCD_Write(LCD_COMMAND,0x80+j-32);   
            LCD_Write(LCD_COMMAND,0x88+i);   
            LCD_Write(LCD_DATA,img[j*16+i*2]);   
            LCD_Write(LCD_DATA,img[j*16+i*2+1]);   
         }   
     }   
    LCD_GraphModeSet(0x01);//最后打开图形显示功能   
}   
   
void LCD_DisplayCLR()   
{   
    unsigned char i,j;   
    LCD_GraphModeSet(0x00); //先关闭图形显示功能   
    for(j=0;j<32;j++)   
     {   
        for(i=0;i<8;i++)   
         {   
            LCD_Write(LCD_COMMAND,0x80+j);      //设定垂直坐标   
            LCD_Write(LCD_COMMAND,0x80+i);      //设定水平坐标   
            LCD_Write(LCD_DATA,0x00);   //放入数据高字节   
            LCD_Write(LCD_DATA,0x00);   //放入数据低字节   
         }   
     }   
    for(j=32;j<64;j++)   
     {   
        for(i=0;i<8;i++)   
         {   
            LCD_Write(LCD_COMMAND,0x80+j-32);   
            LCD_Write(LCD_COMMAND,0x88+i);   
            LCD_Write(LCD_DATA,0x00);   
            LCD_Write(LCD_DATA,0x00);   
         }   
     }   
    LCD_GraphModeSet(0x01);//最后打开图形显示功能   
}   
   
   
//******************************************************************************   
//*函数名称:void LCD_ImgDisplayCharacter(unsigned char x,unsigned char y,unsigned char code *img) *   
//*函数功能:使用绘图的方法,在(x,y)处画一个16*16点阵的图案,也可以是字符        *   
//*形式参数:unsigned char x,unsigned char y,unsigned char code *img                                    *   
//*         x取值范围:0~7                                                      *   
//*         y取值范围:0~31 (针对CM12864-12型液晶)                               *   
//*形参说明:坐标水平位置,坐标垂直位置,要显示的图形                               *   
//*液晶屏坐标说明:                                                            *   
//*      ________________128个像素______________________              *   
//*      |(0,0)                                    (7,0)|             *   
//*      |                                              |             *   
//*   64 |                                              |             *   
//*   个 |(0,31)                                  (7,31)|             *   
//*   像 |(8,0)                                   (15,0)|             *   
//*   素 |                                              |             *   
//*      |                                              |             *   
//*      |(8,31)                                 (15,31)|             *   
//*      |______________________________________________|             *   
//*                                                                   *   
//*返回参数:无                                                              *   
//*使用说明:此函数适用于CM12864-12型液晶         *   
//******************************************************************************   
void LCD_ImgDisplayCharacter(unsigned char x,unsigned char y,unsigned char *img)   
{   
    unsigned char i;   
    LCD_GraphModeSet(0x00);     //先关闭图形显示功能   
    LCD_Write(LCD_COMMAND,LCD_EXTEND_FUNCTION);   
    for(i=0;i<16;i++)   
     {   
        LCD_GDRAM_AddressSet(0x80+y+i);   
        LCD_GDRAM_AddressSet(0x80+x);   
        LCD_Write(LCD_DATA,img[i*2]);   
        LCD_Write(LCD_DATA,img[i*2+1]);   
     }   
   LCD_GraphModeSet(0x01);              //最后打开图形显示功能   
}   
   
   
   
//******************************************************************************   
//*函数名称:void LCD_Point(unsigned char x,unsigned char y,unsigned char color)*   
//*函数功能:使用绘图的方法,在(x,y)处画一个16*16点阵的图案,也可以是字符        *   
//*形式参数:unsigned char x,unsigned char y,unsigned char color                *   
//*         x取值范围:0~127                                                    *   
//*         y取值范围:0~63 (针对CM12864-12型液晶)                               *   
//*形参说明:坐标水平位置,坐标垂直位置                                          *   
//*液晶屏坐标说明:                                                            *   
//*      ________________128个像素______________________              *   
//*      |(0,0)                                    (7,0)|             *   
//*      |                                              |             *   
//*   64 |                                              |             *   
//*   个 |(0,31)                                  (7,31)|             *   
//*   像 |(8,0)                                   (15,0)|             *   
//*   素 |                                              |             *   
//*      |                                              |             *   
//*      |(8,31)                                 (15,31)|             *   
//*      |______________________________________________|             *   
//*                                                                   *   
//*返回参数:无                                                              *   
//*使用说明:此函数适用于CM12864-12型液晶         *   
//******************************************************************************   
void LCD_Point(unsigned char x,unsigned char y,unsigned char color)   
{   
    unsigned char i,AddrX,AddrY;    //寄存器地址   
    unsigned char BitTemp;      //该点在16bit的位置   
    unsigned char DataH,DataL;   
   
    AddrX = x>>4;             // x/16   
    AddrY = y&0x3F;             // y%64   
    if(y > 31)   
    {   
        AddrX += 8;   
        AddrY -= 32;   
    }   
    BitTemp = x&0x0F;           // x%16   
   
       
    LCD_GraphModeSet(0x00);     //先关闭图形显示功能   
    //LCD_Write(LCD_COMMAND,LCD_EXTEND_FUNCTION);   
       
    for(i=0;i<4;i++)   
    {   
    LCD_GDRAM_AddressSet(0x80+AddrY);   
    LCD_GDRAM_AddressSet(0x80+AddrX);   
    //LCD_Write(LCD_COMMAND,LCD_BASIC_FUNCTION);            //基本指令集   
    DataH = LCD_ReadData();   
    DataH = LCD_ReadData();   
    DataL = LCD_ReadData();   
    //DataH = LCD_ReadData();   
    //DataL = LCD_ReadData();   
    }   
   
    if(color == LCD_COLOR_BLACK)   
    {   
        if(BitTemp > 7)   
        {   
            DataL |= (0x80>>(BitTemp-8));   
        }   
        else   
        {   
            DataH |= (0x80>>(BitTemp));   
        }   
    }   
    else   
    {   
    }   
   
    LCD_GDRAM_AddressSet(0x80+AddrY);   
    LCD_GDRAM_AddressSet(0x80+AddrX);   
    LCD_Write(LCD_DATA,DataH);   
    LCD_Write(LCD_DATA,DataL);   
       
    LCD_GraphModeSet(0x01);             //最后打开图形显示功能   
}   
   
   
   

