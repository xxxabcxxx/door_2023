#include "lcd.h"
#include "font_ascii_16x8.h"

void SPI_LCD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);

    //CS DC BL???????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, GPIO_Pin_3);   //DC
    GPIO_SetBits(GPIOD, GPIO_Pin_4);   //CS
    GPIO_SetBits(GPIOD, GPIO_Pin_5);   //BL

    //spi???????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;// GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI3, &SPI_InitStructure);

    SPI_Cmd(SPI3, ENABLE);

}

uint8_t spi_readwrite(uint8_t TxData)
{
    uint8_t i=0;

    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)
    {
        i++;
        if(i>200)return 0;
    }

    SPI_I2S_SendData(SPI3, TxData);
    i=0;

    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET)
    {
        i++;
        if(i>200)return 0;
    }

    return SPI_I2S_ReceiveData(SPI3);
}

/******************************************************************************
      ?????????LCD???????
      ????????dat ????????
      ???????  ??
******************************************************************************/
void LCD_WR_DATA8(uint8_t dat)
{
    LCD_CS_Clr();
    LCD_DC_Set();
//    LCD_Writ_Bus(dat);
//    SPI_DataSizeConfig(SPI3,SPI_DataSize_8b);
    spi_readwrite(dat);
    LCD_CS_Set();
}


/******************************************************************************
      ?????????LCD???????
      ????????dat ????????
      ???????  ??
******************************************************************************/
void LCD_WR_DATA(uint16_t dat)
{
//    LCD_Writ_Bus(dat>>8);
//    LCD_Writ_Bus(dat);
//    SPI_DataSizeConfig(SPI3,SPI_DataSize_16b);
    LCD_CS_Clr();
    LCD_DC_Set();
    spi_readwrite(dat>>8);
    spi_readwrite(dat);

    LCD_CS_Set();
}


/******************************************************************************
      ?????????LCD???????
      ????????dat ????????
      ???????  ??
******************************************************************************/
void LCD_WR_REG(uint8_t dat)
{
    LCD_CS_Clr();
    LCD_DC_Clr();//?????
//    LCD_Writ_Bus(dat);
//    SPI_DataSizeConfig(SPI3,SPI_DataSize_8b);
    spi_readwrite(dat);

    LCD_CS_Set();
}


/******************************************************************************
      ????????????????????????
      ????????x1,x2 ??????????????????
                y1,y2 ??????????????????
      ???????  ??
******************************************************************************/
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    if(USE_HORIZONTAL==0)
    {
        LCD_WR_REG(0x2a);//????????
        LCD_WR_DATA(x1+2);
        LCD_WR_DATA(x2+2);
        LCD_WR_REG(0x2b);//????????
        LCD_WR_DATA(y1+1);
        LCD_WR_DATA(y2+1);
        LCD_WR_REG(0x2c);//???????
    }
    else if(USE_HORIZONTAL==1)
    {
        LCD_WR_REG(0x2a);//????????
        LCD_WR_DATA(x1+2);
        LCD_WR_DATA(x2+2);
        LCD_WR_REG(0x2b);//????????
        LCD_WR_DATA(y1+3);
        LCD_WR_DATA(y2+3);
        LCD_WR_REG(0x2c);//???????
    }
    else if(USE_HORIZONTAL==2)
    {
        LCD_WR_REG(0x2a);//????????
        LCD_WR_DATA(x1+1);
        LCD_WR_DATA(x2+1);
        LCD_WR_REG(0x2b);//????????
        LCD_WR_DATA(y1+2);
        LCD_WR_DATA(y2+2);
        LCD_WR_REG(0x2c);//???????
    }
    else
    {
        LCD_WR_REG(0x2a);//????????
        LCD_WR_DATA(x1+3);
        LCD_WR_DATA(x2+3);
        LCD_WR_REG(0x2b);//????????
        LCD_WR_DATA(y1+2);
        LCD_WR_DATA(y2+2);
        LCD_WR_REG(0x2c);//???????
    }
}
//--------------------------------------------------------------------------------------------------------/
/*********************************************************************************************************/
/*******************??????ST7735??????????                       **************************************/
/*********************************************************************************************************/
void LCD_Init(void)
{
    SPI_LCD_Init();         //?????GPIO

    LCD_RES_Clr();           //???

    LCD_RES_Set();

    LCD_BLK_Set();          //?????

    LCD_WR_REG(0x11);       //Sleep out
        //Delay 120ms
    //------------------------------------ST7735S Frame rate-------------------------------------------------//
    LCD_WR_REG(0xB1);       //Frame rate 80Hz
    LCD_WR_DATA8(0x05);  //02
    LCD_WR_DATA8(0x3C);  //35
    LCD_WR_DATA8(0x3C);  //36

    LCD_WR_REG(0xB2);       //Frame rate 80Hz
    LCD_WR_DATA8(0x05); //02
    LCD_WR_DATA8(0x3C); //35
    LCD_WR_DATA8(0x3C); //36
    LCD_WR_REG(0xB3);       //Frame rate 80Hz
    LCD_WR_DATA8(0x05); //02
    LCD_WR_DATA8(0x3C); //35
    LCD_WR_DATA8(0x3C);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3C);
    LCD_WR_DATA8(0x3C);
    //------------------------------------End ST7735S Frame rate-------------------------------------------//
    LCD_WR_REG(0xB4);      //Dot inversion
    LCD_WR_DATA8(0x03);

    LCD_WR_REG(0x3a);     // Interface pixel format
  //LCD_WR_DATA8(0x03);    // 12-bit/pixel RGB 4-4-4 (4k colors)
    LCD_WR_DATA8(0x05);    // 16-bit/pixel RGB 5-6-5 (65k colors)
  //LCD_WR_DATA8(0x06);    // 18-bit/pixel RGB 6-6-6 (256k colors)

    //------------------------------------ST7735S Power Sequence-----------------------------------------//
    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0xA2);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x84);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA8(0xC5);
    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x00);
    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x8D);
    LCD_WR_DATA8(0x2A);
    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x8D);
    LCD_WR_DATA8(0xEE);
    //---------------------------------End ST7735S Power Sequence---------------------------------------//
    LCD_WR_REG(0xC5);                                      //VCOM
    LCD_WR_DATA8(0x0a);

    LCD_WR_REG(0x36);                                      //???????????
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);          //??????0XC8
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
    else LCD_WR_DATA8(0xA8);
    //---------------------------------ST7735S Gamma Sequence-----------------------------------------//
     LCD_WR_REG(0XE0);
     LCD_WR_DATA8(0x12);
     LCD_WR_DATA8(0x1C);
     LCD_WR_DATA8(0x10);
     LCD_WR_DATA8(0x18);
     LCD_WR_DATA8(0x33);
     LCD_WR_DATA8(0x2C);
     LCD_WR_DATA8(0x25);
     LCD_WR_DATA8(0x28);
     LCD_WR_DATA8(0x28);
     LCD_WR_DATA8(0x27);
     LCD_WR_DATA8(0x2F);
     LCD_WR_DATA8(0x3C);
     LCD_WR_DATA8(0x00);
     LCD_WR_DATA8(0x03);
     LCD_WR_DATA8(0x03);
     LCD_WR_DATA8(0x10);
     LCD_WR_REG(0XE1);
     LCD_WR_DATA8(0x12);
     LCD_WR_DATA8(0x1C);
     LCD_WR_DATA8(0x10);
     LCD_WR_DATA8(0x18);
     LCD_WR_DATA8(0x2D);
     LCD_WR_DATA8(0x28);
     LCD_WR_DATA8(0x23);
     LCD_WR_DATA8(0x28);
     LCD_WR_DATA8(0x28);
     LCD_WR_DATA8(0x26);
     LCD_WR_DATA8(0x2F);
     LCD_WR_DATA8(0x3B);
     LCD_WR_DATA8(0x00);
     LCD_WR_DATA8(0x03);
     LCD_WR_DATA8(0x03);
     LCD_WR_DATA8(0x10);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
    LCD_WR_REG(0x20);     // Display inversion off
  //LCD_WR_REG(0x21);     // Display inversion on

    LCD_WR_REG(0x13);     // Partial mode off


    LCD_WR_REG(0x2A);  //Column addr set
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x7F);

    LCD_WR_REG(0x2B);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x7F);


//  LCD_WR_REG(0x21);                  //????????????????????
    LCD_WR_REG(0x29); //Display on

    /*?????????????*/
    LCD_Fill(0,0,LCD_W,LCD_H,RED);
    LCD_Fill(0,0,LCD_W,LCD_H,GREEN);
    LCD_Fill(0,0,LCD_W,LCD_H,BLUE);
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);


}

#include "rtthread.h"
#include "function.h"
/******************************************************************************
      ????????????????????????
      ????????xsta,ysta   ???????
                xend,yend   ???????
                                color       ????????
      ???????  ??
******************************************************************************/
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{
    //rt_enter_critical();
    uint16_t i,j;
    LCD_Address_Set(xsta,ysta,xend-1,yend-1);//??????????
    for(i=ysta;i<yend;i++)
    {
        for(j=xsta;j<xend;j++)
        {
            LCD_WR_DATA(color);
        }
    }
    DelayUs(50);
    //rt_exit_critical();
}

/******************************************************************************
      ????????????????û???
      ????????x,y ????????
                color ??????
      ???????  ??
******************************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
    LCD_Address_Set(x,y,x,y);//???ù?????
    LCD_WR_DATA(color);
}


/******************************************************************************
      ?????????????
      ????????x1,y1   ???????
                x2,y2   ???????
                color   ??????
      ???????  ??
******************************************************************************/
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
    uint16_t t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //????????????
    delta_y=y2-y1;
    uRow=x1;//???????????
    uCol=y1;
    if(delta_x>0)incx=1; //???õ???????
    else if (delta_x==0)incx=0;//?????
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//????
    else {incy=-1;delta_y=-delta_x;}
    if(delta_x>delta_y)distance=delta_x; //????????????????
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        LCD_DrawPoint(uRow,uCol,color);//????
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


/******************************************************************************
      ???????????????
      ????????x1,y1   ???????
                x2,y2   ???????
                color   ????????
      ???????  ??
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
      ????????????
      ????????x0,y0   ???????
                r       ??
                color   ??????
      ???????  ??
******************************************************************************/
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
    int a,b;
    a=0;b=r;
    while(a<=b)
    {
        LCD_DrawPoint(x0-b,y0-a,color);             //3
        LCD_DrawPoint(x0+b,y0-a,color);             //0
        LCD_DrawPoint(x0-a,y0+b,color);             //1
        LCD_DrawPoint(x0-a,y0-b,color);             //2
        LCD_DrawPoint(x0+b,y0+a,color);             //4
        LCD_DrawPoint(x0+a,y0-b,color);             //5
        LCD_DrawPoint(x0+a,y0+b,color);             //6
        LCD_DrawPoint(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))//??????????????
        {
            b--;
        }
    }
}

// /******************************************************************************
//       ?????????????????
//       ????????x,y???????
//                 *s ??????????
//                 fc ??????
//                 bc ???????
//                 sizey ??? ??? 16 24 32
//                 mode:  0???????  1??????
//       ???????  ??
// ******************************************************************************/
// void LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
// {
//     while(*s!=0)
//     {
//         if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
//         else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
//         else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
//         else return;
//         s+=2;
//         x+=sizey;
//     }
// }

// /******************************************************************************
//       ????????????????16x16????
//       ????????x,y???????
//                 *s ?????????
//                 fc ??????
//                 bc ???????
//                 sizey ???
//                 mode:  0???????  1??????
//       ???????  ??
// ******************************************************************************/
// void LCD_ShowChinese16x16(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
// {
//     uint8_t i,j;
//     uint16_t k;
//     uint16_t HZnum;//???????
//     uint16_t TypefaceNum;//??????????????
//     uint16_t x0=x;
//     TypefaceNum=sizey/8*sizey;//????????????????????????????8??????????
//                               //??????û???????????????,????????????????
//     HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);  //?????????
//     for(k=0;k<HZnum;k++)
//     {
//         if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
//         {
//             LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//             for(i=0;i<TypefaceNum;i++)
//             {
//                 for(j=0;j<8;j++)
//                 {
//                     if(!mode)//???????
//                     {
//                         if(tfont16[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//                         else LCD_WR_DATA(bc);
//                     }
//                     else//??????
//                     {
//                         if(tfont16[k].Msk[i]&(0x01<<j)) LCD_DrawPoint(x,y,fc);//???????
//                         x++;
//                         if((x-x0)==sizey)
//                         {
//                             x=x0;
//                             y++;
//                             break;
//                         }
//                     }
//                 }
//             }
//         }
//         continue;  //??????????????????????????????????????????????
//     }
// }


// /******************************************************************************
//       ????????????????24x24????
//       ????????x,y???????
//                 *s ?????????
//                 fc ??????
//                 bc ???????
//                 sizey ???
//                 mode:  0???????  1??????
//       ???????  ??
// ******************************************************************************/
// void LCD_ShowChinese24x24(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
// {
//     uint8_t i,j;
//     uint16_t k;
//     uint16_t HZnum;//???????
//     uint16_t TypefaceNum;//??????????????
//     uint16_t x0=x;
//     TypefaceNum=sizey/8*sizey;//????????????????????????????8??????????
//                               //??????û???????????????,????????????????
//     HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);  //?????????
//     for(k=0;k<HZnum;k++)
//     {
//         if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
//         {
//             LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//             for(i=0;i<TypefaceNum;i++)
//             {
//                 for(j=0;j<8;j++)
//                 {
//                     if(!mode)//???????
//                     {
//                         if(tfont24[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//                         else LCD_WR_DATA(bc);
//                     }
//                     else//??????
//                     {
//                         if(tfont24[k].Msk[i]&(0x01<<j)) LCD_DrawPoint(x,y,fc);//???????
//                         x++;
//                         if((x-x0)==sizey)
//                         {
//                             x=x0;
//                             y++;
//                             break;
//                         }
//                     }
//                 }
//             }
//         }
//         continue;  //??????????????????????????????????????????????
//     }
// }

// /******************************************************************************
//       ????????????????32x32????
//       ????????x,y???????
//                 *s ?????????
//                 fc ??????
//                 bc ???????
//                 sizey ???
//                 mode:  0???????  1??????
//       ???????  ??
// ******************************************************************************/
// void LCD_ShowChinese32x32(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
// {
//     uint8_t i,j;
//     uint16_t k;
//     uint16_t HZnum;//???????
//     uint16_t TypefaceNum;//??????????????
//     uint16_t x0=x;
//     TypefaceNum=sizey/8*sizey;//????????????????????????????8??????????
//                               //??????û???????????????,????????????????
//     HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);  //?????????
//     for(k=0;k<HZnum;k++)
//     {
//         if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
//         {
//             LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//             for(i=0;i<TypefaceNum;i++)
//             {
//                 for(j=0;j<8;j++)
//                 {
//                     if(!mode)//???????
//                     {
//                         if(tfont32[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//                         else LCD_WR_DATA(bc);
//                     }
//                     else//??????
//                     {
//                         if(tfont32[k].Msk[i]&(0x01<<j)) LCD_DrawPoint(x,y,fc);//???????
//                         x++;
//                         if((x-x0)==sizey)
//                         {
//                             x=x0;
//                             y++;
//                             break;
//                         }
//                     }
//                 }
//             }
//         }
//         continue;  //??????????????????????????????????????????????
//     }
// }


/******************************************************************************
      ???????????????????
      ????????x,y???????
                num ?????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
    uint8_t temp,sizex,t;
    uint16_t i,TypefaceNum;//??????????????
    uint16_t x0=x;
    sizex=sizey/2;
    TypefaceNum=sizex/8*sizey;
    num=num-' ';    //?õ???????
    LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //???ù?????
    for(i=0;i<TypefaceNum;i++)
    {
        if(sizey==16)temp=ascii_1608[num][i];              //????8x16????
        else if(sizey==32)temp=ascii_3216[num][i];       //????16x32????
        else return;
        for(t=0;t<8;t++)
        {
            if(!mode)//???????
            {
                if(temp&(0x01<<t))LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
            else//??????
            {
                if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//???????
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


/******************************************************************************
      ?????????????????
      ????????x,y???????
                *p ???????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
    while(*p!='\0')
    {
        LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
        x+=sizey/2;
        p++;
    }
}


/******************************************************************************
      ????????????????
      ????????m??????n???
      ???????  ??
******************************************************************************/
uint32_t mypow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;
    return result;
}


/******************************************************************************
      ????????????????????
      ????????x,y???????
                num ????????????
                len ?????????
                fc ??????
                bc ???????
                sizey ???
      ???????  ??
******************************************************************************/
void LCD_ShowIntNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{
    uint8_t t,temp;
    uint8_t enshow=0;
    uint8_t sizex=sizey/2;
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


/******************************************************************************
      ??????????????????????
      ????????x,y???????
                num ???????????
                len ?????????
                fc ??????
                bc ???????
                sizey ???
      ???????  ??
******************************************************************************/
void LCD_ShowFloatNum1(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{
    uint8_t t,temp,sizex;
    uint16_t num1;
    sizex=sizey/2;
    num1=num*100;
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


/******************************************************************************
      ??????????????
      ????????x,y???????
                length ??????
                width  ?????
                pic[]  ??????
      ???????  ??
******************************************************************************/
void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[])
{
    uint16_t i,j,k=0;
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


