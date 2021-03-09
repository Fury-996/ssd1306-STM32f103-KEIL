#include "ssd1306.h"
#include "ssd1306font.h"



//ͨ��I2C��SSD1306д����
void SSD1306_Write_Data(uint8_t data)
{
	I2C_WriteByte(SSD1306_DAT_ADDR,data,SSD1306_I2C_ADDR);
}

//ͨ��I2C��SSD1306д����
void SSD1306_Write_Cmd(uint8_t cmd)
{
	I2C_WriteByte(SSD1306_CMD_ADDR,cmd,SSD1306_I2C_ADDR);
}


uint8_t OLED[8][128]={0};
//SSD1306��ʼ��
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

//�������ã�Ҳ������������ʾ
void OLED_SetPos(uint8_t x, uint8_t y)
{
  //����3���Ĵ���ֻ��ҳѰַ��ģʽ����Ч
  SSD1306_Write_Cmd(0xb0+y);             //ҳ��ַ����     0xb0~0xb7
  SSD1306_Write_Cmd(((x&0xf0)>>4)|0x10); //�и�λ��ַ����
  SSD1306_Write_Cmd((x&0x0f));           //�е�λ��ַ����
}

//����OLED��ʾ
void OLED_DisplayOn(void)
{
  SSD1306_Write_Cmd(0X8D);  //SET DCDC����
  SSD1306_Write_Cmd(0X14);  //DCDC ON
  SSD1306_Write_Cmd(0XAF);  //DISPLAY ON
}

//�ر�OLED��ʾ
void OLED_DisplayOff(void)
{
  SSD1306_Write_Cmd(0X8D);  //SET DCDC����
  SSD1306_Write_Cmd(0X10);  //DCDC OFF
  SSD1306_Write_Cmd(0XAE);  //DISPLAY OFF
}



//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��
void OLED_Clear(void)
{
	uint8_t i,n;

  for(i=0;i<8;i++)
  {
    SSD1306_Write_Cmd (0xb0+i);    //����ҳ��ַ��0~7��
    SSD1306_Write_Cmd (0x00);      //������ʾλ�á��е͵�ַ
    SSD1306_Write_Cmd (0x10);      //������ʾλ�á��иߵ�ַ
    for(n=0;n<128;n++)
      SSD1306_Write_Data(0);
  } //������ʾ
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127��y:0~7
//Char_Size:ѡ������ 16/12
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{
	uint8_t c=0,i=0;

  c=chr-' ';//�õ�ƫ�ƺ��ֵ
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
      SSD1306_Write_Data(F8X16[c*16+i]);//��д�ϰ벿��
    }

    OLED_SetPos(x,y+1);
    for(i=0;i<8;i++)
    {
      SSD1306_Write_Data(F8X16[c*16+i+8]);//��д�°벿��
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

//��ʾһ���ַ���
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
    j++;//�ƶ�һ�ξ���һ��page��ȡֵ0-7
  }
}

//��ʾ����
//���ں�����16*16��С�ģ����������ʾ4�к���
//index:�ں���ȡģ�е�����
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

//��ʾ����
//���ں�����13*12��С�ģ����������ʾ5�к���
//index:�ں���ȡģ�е�����
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


//��ʾ�仯�����Ͻ��ı�
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

//��ʾ�仯�����ִ�
//integer ��������
//decimal С������
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
		      SSD1306_Write_Data((uint8_t)MCN12X8[c*16+i]);//��д�ϰ벿��
		    }

		    OLED_SetPos(x,y+1);
		    for(i=0;i<8;i++)
		    {
		      SSD1306_Write_Data((uint8_t)MCN12X8[c*16+i+8]);//��д�°벿��
		    }
	}

}

