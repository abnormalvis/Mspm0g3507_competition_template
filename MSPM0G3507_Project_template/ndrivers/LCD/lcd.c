#include "lcd.h"
#include "lcd_init.h"
#include "lcdfont.h"

void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
    u16 i,j; 
    LCD_Address_Set(xsta,ysta,xend-1,yend-1);
    for(i=ysta;i<yend;i++)
    {                                                      	    	
        for(j=xsta;j<xend;j++)
        {
            LCD_WR_DATA(color);
        }
    } 			    	
}

void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
    LCD_Address_Set(x,y,x,y);
    LCD_WR_DATA(color);
} 

void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
    u16 t; 
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1;
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;
    if(delta_x>0)incx=1;
    else if (delta_x==0)incx=0;
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;
    else {incy=-1;delta_y=-delta_y;}
    if(delta_x>delta_y)distance=delta_x;
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        LCD_DrawPoint(uRow,uCol,color);
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}

void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
    int a,b;
    a=0;b=r;  
    while(a<=b)
    {
        LCD_DrawPoint(x0-b,y0-a,color);
        LCD_DrawPoint(x0+b,y0-a,color);
        LCD_DrawPoint(x0-a,y0+b,color);
        LCD_DrawPoint(x0-a,y0-b,color);
        LCD_DrawPoint(x0+b,y0+a,color);
        LCD_DrawPoint(x0+a,y0-b,color);
        LCD_DrawPoint(x0+a,y0+b,color);
        LCD_DrawPoint(x0-b,y0+a,color);
        a++;
        if((a*a+b*b)>(r*r))
        {
            b--;
        }
    }
}

void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
    while(*s!=0)
    {
        if(sizey==12) LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
        else if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
        else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
        else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
        else return;
        s+=3;
        x+=sizey;
    }
}

// 简化处理：没有字体表时提供最小 stub 实现，避免链接错误。
// 这些实现不绘制汉字，只保留指针占位行为（兼容调用），后续可替换为完整字库渲染实现。
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
    (void)x; (void)y; (void)s; (void)fc; (void)bc; (void)sizey; (void)mode;
}

void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
    (void)x; (void)y; (void)s; (void)fc; (void)bc; (void)sizey; (void)mode;
}

void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
    (void)x; (void)y; (void)s; (void)fc; (void)bc; (void)sizey; (void)mode;
}

void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
    (void)x; (void)y; (void)s; (void)fc; (void)bc; (void)sizey; (void)mode;
}

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
{
    u8 temp,sizex,t,m=0;
    u16 i,TypefaceNum;
    u16 x0=x;

    if(sizey > 16)
    {
        sizey = 16;
    }
    sizex=sizey/2;
    TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
    num=num-' ';
    LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);
    for(i=0;i<TypefaceNum;i++)
    {
        if(sizey==12)temp=ascii_1206[num][i];
        else if(sizey==16)temp=ascii_1608[num][i];
        else return;
        for(t=0;t<8;t++)
        {
            if(!mode)
            {
                if(temp&(0x01<<t))LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
                m++;
                if(m%sizex==0)
                {
                    m=0;
                    break;
                }
            }
            else
            {
                if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);
                x++;
                if((x-x0)==sizex)
                {
                    x=x0;
                    y++;
                    break;
                }
            }
        }
    }
}

void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
{
    while(*p!='\0')
    {
        LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
        x+=sizey/2;
        p++;
    }
}

u32 mypow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}

void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
{
    u8 t,temp;
    u8 enshow=0;
    u8 sizex=sizey/2;
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
                continue;
            }else enshow=1;
        }
        LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
    }
}

void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
{
    u8 t,temp,sizex;
    u16 num1;
    sizex=sizey/2;
    num1=(u16)(num*100);
    for(t=0;t<len;t++)
    {
        temp=(num1/mypow(10,len-t-1))%10;
        if(t==(len-2))
        {
            LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
            t++;
            len+=1;
        }
        LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
    }
}

void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
{
    u16 i,j;
    u32 k=0;
    LCD_Address_Set(x,y,x+length-1,y+width-1);
    for(i=0;i<length;i++)
    {
        for(j=0;j<width;j++)
        {
            LCD_WR_DATA8(pic[k*2]);
            LCD_WR_DATA8(pic[k*2+1]);
            k++;
        }
    }
}
