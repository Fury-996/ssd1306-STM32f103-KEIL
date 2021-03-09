#include "i2c.h"

//工具：用来改变SDA的电平
//     参数level：为1时拉高SDA
//                为0时拉低SDA
void I2C_SDA(uint8_t level)
{
	if(level==1)
	{
		GPIO_SetBits(I2C_SDA_LIT, I2C_SDA_PIN);
	}
	if(level==0)
	{
		GPIO_ResetBits(I2C_SDA_LIT, I2C_SDA_PIN);
	}
}

//工具：用来改变SCL的电平
//参数level：为1时拉高SCL
//                为0时拉低SCL
void I2C_SCL(uint8_t level)
{
	if(level==1)
	{
		GPIO_SetBits(I2C_SCL_LIT, I2C_SCL_PIN);
	}
	if(level==0)
	{
		GPIO_ResetBits(I2C_SCL_LIT, I2C_SCL_PIN);
	}
}



void MYI2C_Init(void)
{
	GPIO_InitTypeDef My_GPIO;
	//打开GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//将PB6和PB7设置为推挽输出模式速度为50MHZ
	My_GPIO.GPIO_Mode=GPIO_Mode_Out_PP;
	My_GPIO.GPIO_Pin=I2C_SDA_PIN|I2C_SCL_PIN;
	My_GPIO.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(I2C_SDA_LIT, &My_GPIO);
	
	I2C_SDA(1);//将SDA置为高电平
	I2C_SCL(1);//将SCL置为高电平
}



//产生IIC起始信号。起始信号为：SCL为高期间，SDA由高变为低
void IIC_Start(void)
{
	SDA_OUT();     //SDA设置为输出
	I2C_SDA(1);//拉高SDA 	  
	I2C_SCL(1);//拉高SCL
	delay_us(4);
 	I2C_SDA(0);//SDA由高变为低 
	delay_us(4);
	I2C_SCL(0);//钳住I2C总线，准备发送或接收数据 
}

//产生IIC停止信号。停止信号为：SCL为高期间，SDA由低变为高
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	I2C_SCL(0);
	I2C_SDA(0);
 	delay_us(4);
	I2C_SCL(1);
	I2C_SDA(1);//发送I2C总线结束信号.SDA由低变为高
	delay_us(4);							   	
}
 
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	I2C_SDA(1);delay_us(1);	   
	I2C_SCL(1);delay_us(1);	 
	while(GPIO_ReadInputDataBit(I2C_SDA_LIT, I2C_SDA_PIN))//读取到从机发送的应答信号后跳出循环。应答信号为：拉低一位SDA线
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	I2C_SCL(0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	I2C_SCL(0);
	SDA_OUT();
	I2C_SDA(0);
	delay_us(2);
	I2C_SCL(1);
	delay_us(2);
	I2C_SCL(0);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	I2C_SCL(0);
	SDA_OUT();
	I2C_SDA(1);
	delay_us(2);
	I2C_SCL(1);
	delay_us(2);
	I2C_SCL(0);
}

//IIC发送一个字节			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA_OUT(); 	    
    I2C_SCL(0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
      I2C_SDA((txd&0x80)>>7);//依照txd的最高位的1，0来决定SDA电平高低（即发送txd的最高位到SDA线上）
      txd<<=1; 	   
		  delay_us(2);   //对TEA5767这三个延时都是必须的
		  I2C_SCL(1);
		  delay_us(2); 
		  I2C_SCL(0);
		  delay_us(2);
    }	 
}

//读1个字节
//参数：ack=1时，发送应答（ACK）
//      ack=0，不发送应答（nACK）
//返回值：从SDA接收到的一个字节（一个16进制数）
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
    I2C_SCL(0);
    delay_us(2);
		I2C_SCL(1);
    receive<<=1;
    if(GPIO_ReadInputDataBit(I2C_SDA_LIT, I2C_SDA_PIN))receive++;   
		delay_us(1); 
  }					 
	if (!ack)
			IIC_NAck();//发送nACK
	else
			IIC_Ack(); //发送ACK   
	return receive;
}

//I2C一次完整通信，向device_addr器件地址的从设备中的寄存器地址addr写入1字节data数据
//参数：device_addr：要通信的从设备器件地址
//      addr：从设备中的寄存器地址。可以是指令寄存器也可以是数据寄存器
//      data：写入的一字节数据
void I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr)
{
	IIC_Start();//主机发送起始信号启用总线  
	
	/*if(device_addr==0xA0) //如果器件地址0xA0
		IIC_Send_Byte(0xA0 + ((addr/256)<<1));//发送高地址
	else*/
	IIC_Send_Byte(device_addr);	    //发器件地址
	IIC_Wait_Ack(); 
	IIC_Send_Byte(addr&0xFF);   //发送寄存器地址
	IIC_Wait_Ack(); 
	IIC_Send_Byte(data);     //发送字节							   
        IIC_Wait_Ack();  		    	   
        IIC_Stop();//产生一个停止条件 
	if(device_addr==0xA0) //
		delay_ms(10);
	else
		delay_us(2);
}

//读寄存器或读数据
uint16_t I2C_ReadByte(uint16_t addr,uint8_t device_addr,uint8_t ByteNumToRead)  
{	
		uint16_t data;
		IIC_Start();  
		/*if(device_addr==0xA0)
			IIC_Send_Byte(0xA0 + ((addr/256)<<1));
		else*/
		IIC_Send_Byte(device_addr);	
		IIC_Wait_Ack();
		IIC_Send_Byte(addr&0xFF);   //发送低地址
		IIC_Wait_Ack(); 
 
		IIC_Start();  	
		IIC_Send_Byte(device_addr+1);	    //发器件地址
		IIC_Wait_Ack();
		if(ByteNumToRead == 1)//LM75温度数据为11bit
		{
			data=IIC_Read_Byte(0);
		}
		else
			{
				data=IIC_Read_Byte(1);
				data=(data<<8)+IIC_Read_Byte(0);
			}
		IIC_Stop();//产生一个停止条件	    
		return data;
}

