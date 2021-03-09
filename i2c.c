#include "i2c.h"

//���ߣ������ı�SDA�ĵ�ƽ
//     ����level��Ϊ1ʱ����SDA
//                Ϊ0ʱ����SDA
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

//���ߣ������ı�SCL�ĵ�ƽ
//����level��Ϊ1ʱ����SCL
//                Ϊ0ʱ����SCL
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
	//��GPIOB��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//��PB6��PB7����Ϊ�������ģʽ�ٶ�Ϊ50MHZ
	My_GPIO.GPIO_Mode=GPIO_Mode_Out_PP;
	My_GPIO.GPIO_Pin=I2C_SDA_PIN|I2C_SCL_PIN;
	My_GPIO.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(I2C_SDA_LIT, &My_GPIO);
	
	I2C_SDA(1);//��SDA��Ϊ�ߵ�ƽ
	I2C_SCL(1);//��SCL��Ϊ�ߵ�ƽ
}



//����IIC��ʼ�źš���ʼ�ź�Ϊ��SCLΪ���ڼ䣬SDA�ɸ߱�Ϊ��
void IIC_Start(void)
{
	SDA_OUT();     //SDA����Ϊ���
	I2C_SDA(1);//����SDA 	  
	I2C_SCL(1);//����SCL
	delay_us(4);
 	I2C_SDA(0);//SDA�ɸ߱�Ϊ�� 
	delay_us(4);
	I2C_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
}

//����IICֹͣ�źš�ֹͣ�ź�Ϊ��SCLΪ���ڼ䣬SDA�ɵͱ�Ϊ��
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	I2C_SCL(0);
	I2C_SDA(0);
 	delay_us(4);
	I2C_SCL(1);
	I2C_SDA(1);//����I2C���߽����ź�.SDA�ɵͱ�Ϊ��
	delay_us(4);							   	
}
 
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	I2C_SDA(1);delay_us(1);	   
	I2C_SCL(1);delay_us(1);	 
	while(GPIO_ReadInputDataBit(I2C_SDA_LIT, I2C_SDA_PIN))//��ȡ���ӻ����͵�Ӧ���źź�����ѭ����Ӧ���ź�Ϊ������һλSDA��
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	I2C_SCL(0);//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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

//IIC����һ���ֽ�			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA_OUT(); 	    
    I2C_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {
      I2C_SDA((txd&0x80)>>7);//����txd�����λ��1��0������SDA��ƽ�ߵͣ�������txd�����λ��SDA���ϣ�
      txd<<=1; 	   
		  delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		  I2C_SCL(1);
		  delay_us(2); 
		  I2C_SCL(0);
		  delay_us(2);
    }	 
}

//��1���ֽ�
//������ack=1ʱ������Ӧ��ACK��
//      ack=0��������Ӧ��nACK��
//����ֵ����SDA���յ���һ���ֽڣ�һ��16��������
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
			IIC_NAck();//����nACK
	else
			IIC_Ack(); //����ACK   
	return receive;
}

//I2Cһ������ͨ�ţ���device_addr������ַ�Ĵ��豸�еļĴ�����ַaddrд��1�ֽ�data����
//������device_addr��Ҫͨ�ŵĴ��豸������ַ
//      addr�����豸�еļĴ�����ַ��������ָ��Ĵ���Ҳ���������ݼĴ���
//      data��д���һ�ֽ�����
void I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr)
{
	IIC_Start();//����������ʼ�ź���������  
	
	/*if(device_addr==0xA0) //���������ַ0xA0
		IIC_Send_Byte(0xA0 + ((addr/256)<<1));//���͸ߵ�ַ
	else*/
	IIC_Send_Byte(device_addr);	    //��������ַ
	IIC_Wait_Ack(); 
	IIC_Send_Byte(addr&0xFF);   //���ͼĴ�����ַ
	IIC_Wait_Ack(); 
	IIC_Send_Byte(data);     //�����ֽ�							   
        IIC_Wait_Ack();  		    	   
        IIC_Stop();//����һ��ֹͣ���� 
	if(device_addr==0xA0) //
		delay_ms(10);
	else
		delay_us(2);
}

//���Ĵ����������
uint16_t I2C_ReadByte(uint16_t addr,uint8_t device_addr,uint8_t ByteNumToRead)  
{	
		uint16_t data;
		IIC_Start();  
		/*if(device_addr==0xA0)
			IIC_Send_Byte(0xA0 + ((addr/256)<<1));
		else*/
		IIC_Send_Byte(device_addr);	
		IIC_Wait_Ack();
		IIC_Send_Byte(addr&0xFF);   //���͵͵�ַ
		IIC_Wait_Ack(); 
 
		IIC_Start();  	
		IIC_Send_Byte(device_addr+1);	    //��������ַ
		IIC_Wait_Ack();
		if(ByteNumToRead == 1)//LM75�¶�����Ϊ11bit
		{
			data=IIC_Read_Byte(0);
		}
		else
			{
				data=IIC_Read_Byte(1);
				data=(data<<8)+IIC_Read_Byte(0);
			}
		IIC_Stop();//����һ��ֹͣ����	    
		return data;
}

