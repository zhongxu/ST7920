#include "GUI.H"   
   
//***********************************************************************************   
// 本文件函数与硬件无关,但是需要调用函数：   
// LCD_Point(unsigned char x,unsigned char y,unsigned char color);   
//***********************************************************************************   
   
   
   
   
   
   
   
   
   
#if _GUI_LINE == 1   
#if _LCD_POINT  !=  1   
#warning "This function need LCD_Point(),But LCD_Point() IS NOT BE ENABLE!!!!"   
#endif  //#if _LCD_POINT    !=  1   
/********************************************************************    
函 数 名：line()   
功    能：画线   
说    明：在12864屏上画线   
入口参数：x0,y0 ,x1,y1,   color   
          起点    终点     颜色   
返 回 值：无   
***********************************************************************/   
void GUI_Line(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned int color)   
{     
    unsigned int t;     
    int xerr=0,yerr=0,delta_x,delta_y,distance;     
    int incx,incy;     
    unsigned int row,col;   
    if( x0>LCD_X_MAX||   
        x1>LCD_X_MAX||   
        y0>LCD_Y_MAX||   
        y1>LCD_Y_MAX )   
    return;   
   
    delta_x = x1-x0;                    //计算坐标增量     
    delta_y = y1-y0;     
    col = x0;     
    row = y0;   
   
    if(delta_x>0)                        //设置单步方向     
        incx=1;   
    else      
        {     
        if( delta_x==0)   
            incx=0;                     //垂直线     
        else   
            {   
            incx=-1;   
            delta_x=-delta_x;   
            }     
        }   
   
    if(delta_y>0)   
        incy=1;     
    else     
        {     
        if( delta_y==0)   
            incy=0;                     //水平线     
        else   
            {   
            incy=-1;   
            delta_y=-delta_y;   
            }   
        }   
   
    if(delta_x> delta_y )   
        distance=delta_x;               //选取基本增量坐标轴     
    else   
        distance=delta_y;     
   
    for( t=0;t <= distance+1; t++ )     
        {                               //画线输出     
        LCD_Point(col, row, color);   
    
        xerr +=delta_x;   
        yerr +=delta_y;   
   
        if(xerr > distance)     
            {     
            xerr -= distance;     
            col += incx;     
            }     
        if(yerr > distance)     
            {     
            yerr -= distance;     
            row += incy;     
            }     
        }     
}   
   
   
#endif  //_GUI_LINE == 1   

