#include "ssd1306.h"
#include "ssd1306font.h"



//通过I2C向SSD1306写数据
void SSD1306_Write_Data(uint8_t data)
{
	I2C_WriteByte(SSD1306_DAT_ADDR,data,SSD1306_I2C_ADDR);
}

//通过I2C向SSD1306写命令
void SSD1306_Write_Cmd(uint8_t cmd)
{
	I2C_WriteByte(SSD1306_CMD_ADDR,cmd,SSD1306_I2C_ADDR);
}


uint8_t OLED[8][128]={0};
//SSD1306初始化
void SSD1306_Init(void)
{
	delay_ms(200);
	SSD1306_Write_Cmd(0xAE);//--display off

	  SSD1306_Write_Cmd(0x00);//--set low column address
	  SSD1306_Write_Cmd(0x10);//--set high column address
	  SSD1306_Write_Cmd(0x40);//--set start line address

	  SSD1306_Write_Cmd(0xB0);//--set page address

	  SSD1306_Write_Cmd(0x81);// contract control
	  SSD1306_Write_Cmd(0xFF);//--128
	  SSD1306_Write_Cmd(0xA1);//set segment re-map  0 to 127
	  SSD1306_Write_Cmd(0xA6);//set normal  display
	  SSD1306_Write_Cmd(0xA8);//set multiplex ratio(1 to 64)
	  SSD1306_Write_Cmd(0x3F);//--1/32 duty

	  SSD1306_Write_Cmd(0xC8);//Com scan direction
	  SSD1306_Write_Cmd(0xD3);//set display offset
	  SSD1306_Write_Cmd(0x00);//no offset

	  SSD1306_Write_Cmd(0xD5);//set display clock divide ratio/oscillator frequency
	  SSD1306_Write_Cmd(0x80);//

	  SSD1306_Write_Cmd(0xD8);//set area color mode off
	  SSD1306_Write_Cmd(0x05);//

	  SSD1306_Write_Cmd(0xD9);//Set Pre-Charge Period
	  SSD1306_Write_Cmd(0xF1);//

	  SSD1306_Write_Cmd(0xDA);//set com pin  hardware configuartion
	  SSD1306_Write_Cmd(0x12);//

	  SSD1306_Write_Cmd(0xDB);//set Vcomh
	  SSD1306_Write_Cmd(0x30);//0x20,0.77xVcc

	  SSD1306_Write_Cmd(0x8D);//set charge pump enable
	  SSD1306_Write_Cmd(0x14);//

	  SSD1306_Write_Cmd(0xAF);//--turn on oled panel
}

//坐标设置：也就是在哪里显示
void OLED_SetPos(uint8_t x, uint8_t y)
{
  //以下3个寄存器只在页寻址的模式下有效
  SSD1306_Write_Cmd(0xb0+y);             //页地址设置     0xb0~0xb7
  SSD1306_Write_Cmd(((x&0xf0)>>4)|0x10); //列高位地址设置
  SSD1306_Write_Cmd((x&0x0f));           //列低位地址设置
}

//开启OLED显示
void OLED_DisplayOn(void)
{
  SSD1306_Write_Cmd(0X8D);  //SET DCDC命令
  SSD1306_Write_Cmd(0X14);  //DCDC ON
  SSD1306_Write_Cmd(0XAF);  //DISPLAY ON
}

//关闭OLED显示
void OLED_DisplayOff(void)
{
  SSD1306_Write_Cmd(0X8D);  //SET DCDC命令
  SSD1306_Write_Cmd(0X10);  //DCDC OFF
  SSD1306_Write_Cmd(0XAE);  //DISPLAY OFF
}



//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样
void OLED_Clear(void)
{
	uint8_t i,n;

  for(i=0;i<8;i++)
  {
    SSD1306_Write_Cmd (0xb0+i);    //设置页地址（0~7）
    SSD1306_Write_Cmd (0x00);      //设置显示位置—列低地址
    SSD1306_Write_Cmd (0x10);      //设置显示位置—列高地址
    for(n=0;n<128;n++)
      SSD1306_Write_Data(0);
  } //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127，y:0~7
//Char_Size:选择字体 16/12
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{
	uint8_t c=0,i=0;

  c=chr-' ';//得到偏移后的值
  if(x>MAX_COLUMN-1)
  {
    x=0;
    y=y+2;
  }
  if(Char_Size ==16)
  {
    OLED_SetPos(x,y);
    for(i=0;i<8;i++)
    {
      SSD1306_Write_Data(F8X16[c*16+i]);//先写上半部分
    }

    OLED_SetPos(x,y+1);
    for(i=0;i<8;i++)
    {
      SSD1306_Write_Data(F8X16[c*16+i+8]);//后写下半部分
    }
  }
  else
  {
    OLED_SetPos(x,y);
    for(i=0;i<6;i++)
    {
      SSD1306_Write_Data(F6x8[c][i]);
    }
  }
}

//显示一个字符串
void OLED_ShowString(uint8_t x,uint8_t y,char *str,uint8_t Char_Size)
{
  unsigned char j=0;

  while (str[j]!='\0')
  {
    OLED_ShowChar(x,y,str[j],Char_Size);
    x+=8;
    if(x>120)
    {
      x=0;
      y+=2;
    }
    j++;//移动一次就是一个page，取值0-7
  }
}

//显示汉字
//由于汉字是16*16大小的，所以最多显示4行汉字
//index:在汉字取模中的索引
void OLED_ShowCN(uint8_t x,uint8_t y,uint8_t index)
{
	uint8_t t;

  OLED_SetPos(x,y);
  for(t=0;t<16;t++)
  {
    SSD1306_Write_Data(Hzk[index][t]);
  }

  OLED_SetPos(x,y+1);
  for(t=0;t<16;t++)
  {
    SSD1306_Write_Data(Hzk[index][t+16]);
  }
}

//显示汉字
//由于汉字是13*12大小的，所以最多显示5行汉字
//index:在汉字取模中的索引
void OLED_ShowLCN(uint8_t x,uint8_t y,uint8_t index)
{
	uint8_t t;

  OLED_SetPos(x,y);
  for(t=0;t<12;t++)
  {
    SSD1306_Write_Data(LHzk[index][t]);
  }

  OLED_SetPos(x,y+1);
  for(t=0;t<12;t++)
  {
    SSD1306_Write_Data(LHzk[index][t+12]);
  }
}


//显示变化数字严禁改变
void OLED_ShowCNU(uint8_t x,uint8_t y,uint8_t index)
{
	uint8_t t;

  OLED_SetPos(x,y);
  for(t=0;t<6;t++)
  {
    SSD1306_Write_Data(CNUM[index][t]);
  }

  OLED_SetPos(x,y+1);
  for(t=0;t<6;t++)
  {
    SSD1306_Write_Data(CNUM[index][t+6]);
  }
}

//显示变化的数字串
//integer 整数部分
//decimal 小数部分
void OLED_ShowCSNU(uint8_t x,uint8_t y,uint8_t integer,uint8_t decimal)
{
	uint8_t q,w,e;


	q=integer/10;
	w=integer%10;

	e=decimal%10;

	OLED_ShowCNU(x,y,q);
	OLED_ShowCNU(x+6,y,w);

	OLED_ShowCNU(x+12,y,10);

	OLED_ShowCNU(x+15,y,e);


}


void SSD1306_Show_Change_Char(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{
	uint8_t c=0,i=0;
	c=chr-' ';
	if(x>MAX_COLUMN-1)
	{
		x=0;
	    y=y+2;
	}

	if(Char_Size==8)
	{
		OLED_SetPos(x,y);
		for(i=0;i<6;i++)
		{
			SSD1306_Write_Data(SCN12X8[c][i]);
		}
	}

	else if(Char_Size==16)
	{
		OLED_SetPos(x,y);
		    for(i=0;i<8;i++)
		    {
		      SSD1306_Write_Data((uint8_t)MCN12X8[c*16+i]);//先写上半部分
		    }

		    OLED_SetPos(x,y+1);
		    for(i=0;i<8;i++)
		    {
		      SSD1306_Write_Data((uint8_t)MCN12X8[c*16+i+8]);//后写下半部分
		    }
	}

}

