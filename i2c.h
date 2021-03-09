#ifndef _I2C_H
#define _I2C_H
#include "stm32f10x.h"
#include "delay.h"
/*ģ��I2C
  I2Cͨ�Ź��̣�����������ʼ�ź���������
	            ��������һ���ֽ����ݣ�������һ����Ҫͨ�ŵĴӻ���ַ��
							�ӻ�����һλӦ���ź�
							��������һ���ֽ�����
							�ӻ�����Ӧ���ź�
							    ����Ҫ���͵����ݴ�Сѭ��֮ǰ��������
							����������ֹ�ź��ͷ�����

��ģ��I2CĬ�ϣ� ��ʼ�ź�Ϊ��SCLΪ���ڼ䣬SDA�ɸ߱�Ϊ��
               ֹͣ�ź�Ϊ��SCLΪ���ڼ䣬SDA�ɵͱ�Ϊ��
							 Ӧ���ź�Ϊ������һλSDA��
							 ʱ��Ϊ���͵ĵ�һ���ֽ�Ϊ������ַ���ڶ����ֽ�Ϊ�Ĵ�����ַ���������ֽ�Ϊ���ݣ�ֹͣ��
							 
һ��ʹ�ù��̣�IIC_Init();
             I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr);
*/

//��PB7����Ϊģ��I2C��SDA
#define I2C_SDA_LIT   GPIOB
#define I2C_SDA_PIN   GPIO_Pin_7
//��PB6����Ϊģ��I2C��SCL
#define I2C_SCL_LIT   GPIOB
#define I2C_SCL_PIN   GPIO_Pin_6


#define SDA_IN()      {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}  //��PB7����Ϊ��������
#define SDA_OUT()     {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}  //��PB7����Ϊ��������ٶ�Ϊ50Mhz

//IIC���в�������
void MYI2C_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				        //����IIC��ʼ�ź�
void IIC_Stop(void);	  			      //����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			    //IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				      //IIC�ȴ�ACK�ź�
void IIC_Ack(void);					        //IIC����ACK�ź�
void IIC_NAck(void);				        //IIC������ACK�ź�
 
void I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr);
uint16_t I2C_ReadByte(uint16_t addr,uint8_t device_addr,uint8_t ByteNumToRead);//�Ĵ�����ַ��������ַ��Ҫ�����ֽ���

#endif

