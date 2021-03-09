#ifndef _I2C_H
#define _I2C_H
#include "stm32f10x.h"
#include "delay.h"
/*模拟I2C
  I2C通信过程：主机发送起始信号启用总线
	            主机发送一个字节数据（此数据一般是要通信的从机地址）
							从机发送一位应答信号
							主机发送一个字节数据
							从机发送应答信号
							    根据要发送的数据大小循环之前两个步骤
							主机发送终止信号释放总线

此模拟I2C默认： 起始信号为：SCL为高期间，SDA由高变为低
               停止信号为：SCL为高期间，SDA由低变为高
							 应答信号为：拉低一位SDA线
							 时序为发送的第一个字节为器件地址，第二个字节为寄存器地址，第三个字节为数据，停止。
							 
一般使用过程：IIC_Init();
             I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr);
*/

//将PB7设置为模拟I2C的SDA
#define I2C_SDA_LIT   GPIOB
#define I2C_SDA_PIN   GPIO_Pin_7
//将PB6设置为模拟I2C的SCL
#define I2C_SCL_LIT   GPIOB
#define I2C_SCL_PIN   GPIO_Pin_6


#define SDA_IN()      {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}  //将PB7设置为上拉输入
#define SDA_OUT()     {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}  //将PB7设置为推挽输出速度为50Mhz

//IIC所有操作函数
void MYI2C_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				        //发送IIC开始信号
void IIC_Stop(void);	  			      //发送IIC停止信号
void IIC_Send_Byte(u8 txd);			    //IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				      //IIC等待ACK信号
void IIC_Ack(void);					        //IIC发送ACK信号
void IIC_NAck(void);				        //IIC不发送ACK信号
 
void I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr);
uint16_t I2C_ReadByte(uint16_t addr,uint8_t device_addr,uint8_t ByteNumToRead);//寄存器地址，器件地址，要读的字节数

#endif

