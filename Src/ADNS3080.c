/*
 * ADNS3080.c
 *
 *  Created on: 19.04.2019 ã.
 *      Author: ru
 */
#include "ADNS3080.h"

void ADNS_Reset(void)
{
  Hi_RST_EC3;Hi_NCS_EC1;Hi_MOSI_EC5;Hi_SCLK_EC6;
  Hi_NPD_EC4;TIM2->CNT=0; //insert reset, remove power down
  while (TIM2->CNT<(uint32_t)(72*300)){;} //timer counts with 72MHz
  Lo_RST_EC3;TIM2->CNT=0;//remove reset
  while (TIM2->CNT<(uint32_t)(72UL*40000UL)){LL_IWDG_ReloadCounter(IWDG);} //timer counts with 72MHz
}

inline void t75usDelay(void) //75us delay
{
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(72*75)){;} //timer counts with 72MHz
}
inline void t50usdDelay(void) //75us delay
{
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(72*50)){;} //timer counts with 72MHz
}

uint8_t ADNS_WriteReadByte(uint8_t by)  //WRITE if by|0x80, READ by&0x7f
{
 for (int x=0;x<8;x++)
 {
   Lo_SCLK_EC6;TIM2->CNT=0;
   if (by&0x80) Hi_MOSI_EC5;else Lo_MOSI_EC5;
   while (TIM2->CNT<(uint32_t)(20)){;} //timer counts with 72MHz, i.e. 13ns steps
   Hi_SCLK_EC6;TIM2->CNT=0;
   while (TIM2->CNT<(uint32_t)(20)){;} //timer counts with 72MHz, i.e. 13ns steps
   by<<=1;
   if (Get_MISO_EC2) by|=0x01;
 }
 return by;
}

void WriteReg(uint8_t Adr,uint8_t Val)
{
  Lo_NCS_EC1;
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(10)){;} //timer counts with 72MHz, i.e. 13ns steps
  ADNS_WriteReadByte(Adr|0x80); //send address
  ADNS_WriteReadByte(Val); //send data
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(10)){;} //timer counts with 72MHz, i.e. 13ns steps
  Hi_NCS_EC1;
}


void ReadBurstSomething(uint8_t* arr,uint8_t beg, uint16_t len)
{
  Lo_NCS_EC1;
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(10)){;} //timer counts with 72MHz, i.e. 13ns steps
  ADNS_WriteReadByte(beg); //send address
  t75usDelay();
  for (int x=0; x<len; x++) //7 bytes read
  {
    *arr=ADNS_WriteReadByte(0xff);
    arr++;
  }
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(10)){;} //timer counts with 72MHz, i.e. 13ns steps
  Hi_NCS_EC1;
}


void ReadBurstMotion(int8_t* arr)  //7bytes
{
  Lo_NCS_EC1;
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(10)){;} //timer counts with 72MHz, i.e. 13ns steps
  ADNS_WriteReadByte(0x50); //send address
  t75usDelay();
  for (int x=0; x<7; x++) //7 bytes read
  {
    *arr=ADNS_WriteReadByte(0xff);
    arr++;
  }
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(10)){;} //timer counts with 72MHz, i.e. 13ns steps
  Hi_NCS_EC1;
}

uint8_t ReadMot(int8_t* arr)
{
  uint8_t z;
  Lo_NCS_EC1;
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(10)){;} //timer counts with 72MHz, i.e. 13ns steps
  ADNS_WriteReadByte(0x02); //send address
  t75usDelay();
    z=ADNS_WriteReadByte(0xff);*arr=z;
    if ((z&0x80)==0) {goto exii;} //no motion
    ADNS_WriteReadByte(0x03);t75usDelay(); //send address
    arr++;
    *arr=ADNS_WriteReadByte(0xff);
    arr++;
    ADNS_WriteReadByte(0x04);t75usDelay(); //send address
    *arr=ADNS_WriteReadByte(0xff);
exii:
  TIM2->CNT=0;while (TIM2->CNT<(uint32_t)(10)){;} //timer counts with 72MHz, i.e. 13ns steps
  Hi_NCS_EC1;
  return z;
}

