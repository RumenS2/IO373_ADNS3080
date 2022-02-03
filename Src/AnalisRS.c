#include "AnalisRS.h"
#include "variables.h"
#include "hw_config.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_it.h"
#include <string.h>

void RSC_GetStatus(void );
//void RSC_Get7ADCvolt(void );
//void RSC_Get9ADCspc(void );
void RSC_GetSVpBV(void );
void RSC_GetConfig(void );
void RSC_SetStatus(void );
void RSC_SetOlCo(void );
void RSC_SetUSERI(void );
void RSC_SetConfig(void );
void RSC_SetOlCoRetStatus(void);
void RSC_SetOSP(void);

uint16_t RsThisSlaveDeviceN;

#define ERRLIN_BUF_LEN (RSBUFLEN/8)  //2*4
uint8_t RSBuffer[RSBUFLEN],RS2Buffer[RSBUFLEN],RS3Buffer[RSBUFLEN];
uint32_t ErrLineBuff[ERRLIN_BUF_LEN][2];
uint16_t ELB_Idx=0;
volatile struct RS_State RSS,RSS2,RSS3;
volatile uint8_t ConfigNowReceived=0;
volatile uint8_t AnalisRS1IsBusy=0,AnalisRS2IsBusy=0,AnalisRS3IsBusy=0;   //!=0 if AnalisRS are executed
//USART_TypeDef* UsartInUse;

__STATIC_INLINE void USART1_TX_START(void){OnDE_OnTrn_U1;LL_USART_EnableDirectionTx(USART1);LL_USART_EnableIT_TXE(USART1);}
__STATIC_INLINE void USART2_TX_START(void){OnDE_OnTrn_U2;LL_USART_EnableDirectionTx(USART2);LL_USART_EnableIT_TXE(USART2);}
__STATIC_INLINE void USART1_RX_START(void){OffDE_OnRcv_U1;LL_USART_EnableIT_RXNE(USART1);LL_USART_EnableDirectionRx(USART1);}
__STATIC_INLINE void USART2_RX_START(void){OffDE_OnRcv_U2;LL_USART_EnableIT_RXNE(USART2);LL_USART_EnableDirectionRx(USART2);}
__STATIC_INLINE void USARTinUSE_RX_DISABLE(USART_TypeDef *USARTx)
				{LL_USART_DisableDirectionRx(USARTx);LL_USART_DisableIT_RXNE(USARTx);LL_USART_DisableIT_IDLE(USARTx);}

void AnalisRS(void);
//void AnalisMRS3(void);
void AnalisRS2(void);
void AnalisRS1main(void);


void SaveErrInList(uint32_t EFlags,uint32_t Cline)
{
  ErrLineBuff[ELB_Idx][0]=EFlags;ErrLineBuff[ELB_Idx][1]=Cline;
  ELB_Idx++;if (ELB_Idx==ERRLIN_BUF_LEN) ELB_Idx=0;
}

//!==========================================================================================================
void AnalisRS(void)
{
//uint8_t t8;
//pak1:;
//  t8=__LDREXB(&AnalisRS1IsBusy);  //check only from irq if AnalisRS simultaneously is irq AND backgorund used
//  if (t8) goto rs1isbusy;
//  t8=1;
//  if (__STREXB(t8,&AnalisRS1IsBusy)) goto pak1;
//------------------------------------------------
	AnalisRS1main();
//pak1ex:;
//  t8=__LDREXB(&AnalisRS1IsBusy); //check only from irq if AnalisRS simultaneously is irq AND backgorund used
//  t8=0;
//  if (__STREXB(t8,&AnalisRS1IsBusy)) goto pak1ex;
//rs1isbusy:;
//*************************************************************************
/*
pak2:;
  t8=__LDREXB(&AnalisRS2IsBusy);  //check only from irq if AnalisRS simultaneously is irq AND backgorund used
  if (t8) goto rs2isbusy;
  t8=1;
  if (__STREXB(t8,&AnalisRS2IsBusy)) goto pak2;
//------------------------------------------------
	AnalisRS2();
pak2ex:;
  t8=__LDREXB(&AnalisRS2IsBusy); //check only from irq if AnalisRS simultaneously is irq AND backgorund used
  t8=0;
  if (__STREXB(t8,&AnalisRS2IsBusy)) goto pak2ex;
rs2isbusy:;
*/
}

void AnalisRS2(void) //Master mode
{
//int16_t ti16;
//int32_t ti32;
//------------------------------------------------
  if (RSS2.CntAnalisRS<SysTickCntr)
  {
    if (RSS2.State==RSstIdle)
    {
      RSS2.CntAnalisRS=SysTickCntr+127; //12.7ms
      RSS2.RetrCnt=0;
      RSS2.CommCntr++; if (RSS2.CommCntr>1) RSS2.CommCntr=1;

      if (RSS2.CommCntr==1)
      {
        RSS2.CntAnalisRS=SysTickCntr+137; //13.7ms
        RS2Buffer[RsPosDN]=RsExtSlaveDeviceN;RS2Buffer[RsPosCMD]=COMM_SET;RS2Buffer[RsPosSubCMD]=SubCmd_StructSTATUS;
        memcpy((void*)&RS2Buffer[RsPosSubCMD+1],(const void*)&OStSnd,sizeof(OStSnd));
	    RSS2.BegPnt=&RS2Buffer[0];RSS2.EndPnt=&RS2Buffer[RsPosSubCMD+sizeof(OStSnd)+2];
        Calc_CS_WithCopy((uint8_t*)&RS2Buffer[0],(uint8_t*)&RS2Buffer[0],RsPosSubCMD+1+sizeof(OStSnd));
        RSS2.State=RSstTrn;RSS2.TraceCod=0;
      } //!RsExtSlaveDeviceN 1 SET SubCmd_StructSTATUS
/*
      if (RSS2.CommCntr==2)
      {
        RSS2.CntAnalisRS=SysTickCntr+137; //13.7ms
        RS2Buffer[RsPosDN]=RsExtSlaveDeviceN;RS2Buffer[RsPosCMD]=COMM_GET;RS2Buffer[RsPosSubCMD]=SubCmd_AINspc;
		RSS2.BegPnt=&RS2Buffer[0];RSS2.EndPnt=&RS2Buffer[RsPosSubCMD+2];
		Calc_CS_WithCopy((uint8_t*)&RS2Buffer[0],(uint8_t*)&RS2Buffer[0],RsPosSubCMD+1);
		HiTRN_Enbl_485_3;RSS2.State=RSstTrn;RSS2.TraceCod=0;
      }//!RsExtSlaveDeviceN 1 GET SubCmd_AINspc
*/
/*
      if (RSS2.CommCntr==3)
      {
        RSS2.CntAnalisRS=SysTickCntr+137; //13.7ms
        RS2Buffer[RsPosDN]=Rs2SlaveDeviceN;RS2Buffer[RsPosCMD]=COMM_SET;RS2Buffer[RsPosSubCMD]=CommandToIOM; //SubCmd_ModeAuto SubCmd_ModeMan SubCmd_Reload
		RSS2.BegPnt=&RS2Buffer[0];RSS2.EndPnt=&RS2Buffer[RsPosSubCMD+2];
		Calc_CS_WithCopy((uint8_t*)&RS2Buffer[0],(uint8_t*)&RS2Buffer[0],RsPosSubCMD+1);
        HiTRN_Enbl_485_3;RSS2.State=RSstTrn;RSS2.TraceCod=0;
      }//!Rs2SlaveDeviceN 3 SET SubCmd_ModeAuto SubCmd_ModeMan SubCmd_Reload
*/
      USART2_TX_START();
      goto exiir;
    }
  }
//maybeSomeUnknDelay:;
  if ((RSS2.State!=RSstRcv)&&(RSS2.State!=RSstWait1Rcv)) goto exiir;
  if (RSS2.CntTimeout>SysTickCntr) goto exiir;
  USARTinUSE_RX_DISABLE(USART3);
  if (RSS2.State==RSstWait1Rcv){RSS2.ErrCod=5;RSS2.RetrCnt++;RSS2.State=RSstIdle;goto exii;}
  RSS2.State=0xff;
  if (RSS2.wCRC!=0)
  {
    if (RSS2.BegPnt==&RS2Buffer[0]) {RSS2.ErrCod=2;}else{RSS2.ErrCod=3;}
    RSS2.RetrCnt++;RSS2.State=RSstIdle;goto exii;
  }
  if (RS2Buffer[RsPosDN]!=RsThisSlaveDeviceN) { RSS2.RetrCnt++;RSS2.ErrCod=1;RSS2.State=RSstIdle; goto exii;}
//!check in exact command  if (RS2Buffer[RsPosDN]!=RsThisSlaveDeviceN) { RSS2.RetrCnt++;RSS2.ErrCod=1;RSS2.State=RSstIdle; goto exii;}
  RSS2.LastAnsTimeStamp=SysTickCntr;
  if (RS2Buffer[RsPosCMD]==COMM_GET) //check slave answer on COMM_GET
  {
/*
    if (RS2Buffer[RsPosSubCMD]==SubCmd_AINspc)
    {
      if (RS2Buffer[RsPosDN]!=RsExtSlaveDeviceN) {MState.ErrorFlags|=WF_SlaveExtCommErr;RSS2.RetrCnt++;RSS2.ErrCod=1;RSS2.State=RSstIdle; goto exii;}
      RSS2.RetrCnt=0;RSS2.ErrCod=0;MState.ErrorFlags&=~WF_SlaveExtCommErr;
      memcpy((void*)&fExtADC_ArrSpec[0],(const void*)&RS2Buffer[RsPosSubCMD+1],sizeof(fExtADC_ArrSpec));
      RSS2.State=RSstIdle;RSS2.CntAnalisRS=SysTickCntr; //force next question immediately
      goto exii;
    }
*/
    RSS2.RetrCnt++;RSS2.ErrCod=12;RSS2.State=RSstIdle; //start new question!!!
  }
  if (RS2Buffer[RsPosCMD]==COMM_SET) //check slave answer on COMM_SET (in fact this is some as Exchange)
  {   //no another checks?
    if (RS2Buffer[RsPosSubCMD]==SubCmd_StructSTATUS)
    {
      if (RS2Buffer[RsPosDN]!=RsExtSlaveDeviceN) {MState.StatusFlags|=WF_SlaveExtCommErr;RSS2.RetrCnt++;RSS2.ErrCod=1;RSS2.State=RSstIdle; goto exii;}
      RSS2.RetrCnt=0;RSS2.ErrCod=0;MState.StatusFlags&=~WF_SlaveExtCommErr;
      memcpy((void*)&OStRet,(const void*)&RS2Buffer[RsPosSubCMD+1],sizeof(OStRet));
      RSS2.State=RSstIdle;RSS2.CntAnalisRS=SysTickCntr; //force next question immediately
      goto exii;
    }
/*
    if (RS2Buffer[RsPosSubCMD]==CommandToIOM)
    {
      if (RS2Buffer[RsPosDN]!=Rs2SlaveDeviceN) {MState.ErrorFlags|=WF_SlaveExtCommErr;RSS2.RetrCnt++;RSS2.ErrCod=1;RSS2.State=RSstIdle; goto exii;}
        LoadSnsCopy=*((uint16_t*)&RS2Buffer[RsPosSubCMD+3]);
        MState.USERO=*((uint16_t*)&RS2Buffer[RsPosSubCMD+3]);
        LoadPhaseCopy=*((uint16_t*)&RS2Buffer[RsPosSubCMD+1]);
      RSS2.RetrCnt=0;RSS2.ErrCod=0;MState.ErrorFlags&=~WF_SlaveExtCommErr;
      RSS2.State=RSstIdle;goto exii;
    }
*/
     RSS2.RetrCnt=0;RSS2.ErrCod=0;RSS2.State=RSstIdle;//if (RSS2.CommCntr==3) RSS2.CommCntr=2;
     goto exii;
  }
  RSS2.RetrCnt++;RSS2.ErrCod=13;RSS2.State=RSstIdle;//if (RSS2.CommCntr==3)RSS2.CommCntr=2; //start new question!!!
exii:;
//rserrcnt+=RSS2.RetrCnt;
exiir:;
}
//!==========================================================================================================
void AnalisRS1main(void) //!main channel (in fact works as SLAVE)
{
  if (RSS.State!=RSstRcv) goto exii;
  if (RSS.CntTimeout>SysTickCntr) goto exii;
//>>>>>>>>>>>>>>>>
  USARTinUSE_RX_DISABLE(USART1);RSS.State=0xff;
//---------------
  if (RSBuffer[RsPosDN]!=RsThisSlaveDeviceN)
  {
     //RSS.State=RSstNoValidAns;
     RSS.RetrCnt++; RSS.ErrCod=2;
     if (RSS.BegPnt==&RSBuffer[0]) {RSS.ErrCod=0x03;} //no message in buff Err 3???
     if (RSS.wCRC==0) RSS.ErrCod=0; //0x10 message not for us
     RSS.State=RSstWait1Rcv;
     USART1_RX_START();
     goto exii;
  }
  if (RSS.wCRC!=0)
  {
	if (RSS.BegPnt==&RSBuffer[0]) {RSS.ErrCod=0x03;}  //no message in buff Err 3???
	else
    {
		RSS.ErrCod=4;
		if (RSS.BegPnt==&RSBuffer[1]) {RSS.ErrCod=0x00;}  //ONLY ONE char in buff
    }
	RSS.State=RSstWait1Rcv;
	USART1_RX_START();
	goto exii;
  }
//forcedtest:;
	    if (RSBuffer[RsPosCMD]==COMM_GET)
	    {
              RSS.RetrCnt=0;RSS.ErrCod=0;
              if (RSBuffer[RsPosSubCMD]==SubCmd_StructSTATUS) {RSC_GetStatus();goto cgans;}
//              if (RSBuffer[RsPosSubCMD]==SubCmd_AINvolt) {RSC_Get7ADCvolt();goto cgans;}
//              if (RSBuffer[RsPosSubCMD]==SubCmd_AINspc) {RSC_Get9ADCspc();goto cgans;}
              if (RSBuffer[RsPosSubCMD]==SubCmd_Config) {RSC_GetConfig();goto cgans;}
              if (RSBuffer[RsPosSubCMD]==SubCmd_SVpBV)  {RSC_GetSVpBV();goto cgans;}
//              if (RSBuffer[RsPosSubCMD]==SubCmd_OIST)  {RSC_GetOIST();goto cgans;}
          //unkn cmd
              RSS.RetrCnt++;RSS.ErrCod=1;
              RSS.State=RSstWait1Rcv; //wait new question!!!
              USART1_RX_START();
              goto exii;
cgans:;
              RSS.State=RSstTrn;RSS.TraceCod=0;RSS.LastAnsTimeStamp=SysTickCntr;
              USART1_TX_START(); //idle frame will be generated because TE is set here
              goto exii;
	    }

	    if (RSBuffer[RsPosCMD]==COMM_SET)
	    {
              RSS.RetrCnt=0;RSS.ErrCod=0;
              if (RSBuffer[RsPosSubCMD]==SubCmd_OldComands) {RSC_SetOlCoRetStatus();goto cgans;} //receive AMKkeyboard, send MState
              if (RSBuffer[RsPosSubCMD]==SubCmd_USERI) {RSC_SetUSERI();goto cgans;}
              if (RSBuffer[RsPosSubCMD]==SubCmd_Config) {RSC_SetConfig();goto cgans;}
              if (RSBuffer[RsPosSubCMD]==SubCmd_SetOSP) {RSC_SetOSP();goto cgans;}
               //unkn cmd
              RSS.RetrCnt++;RSS.ErrCod=5;
              RSS.State=RSstWait1Rcv; //wait new question!!!
              USART1_RX_START();
              goto exii;
            }

        //RSS.State=RSstNoValidAns; //unkn cmd
            RSS.RetrCnt++;RSS.ErrCod=6;
            RSS.State=RSstWait1Rcv; //wait new question!!!
            USART1_RX_START();
            goto exii;
exii:;

}

void USART1_IRQHandler (void) //Slave VERSION
{
unsigned int IIR;
  IIR = USART1->ISR;

   if ((RSS.State==RSstWait1Rcv))
	   if (IIR&USART_ISR_RXNE)
		   {
			   RSS.BegPnt=&RSBuffer[0];RSS.wCRC=0xffff;
			   RSS.CntTimeout=SysTickCntr+3; //~3chars
			   *RSS.BegPnt=(uint8_t)USART1->RDR;Crc16ModbusFast(&RSS.wCRC,RSS.BegPnt);
			   RSS.BegPnt++;
			   if (IIR&(USART_ISR_FE|USART_ISR_IDLE|USART_ISR_LBDF)){USART1->ICR=(USART_ICR_FECF|USART_ICR_IDLECF|USART_ICR_LBDCF);}
			   LL_USART_EnableIT_IDLE(USART1);
                           RSS.State=RSstRcv;
			   goto exii;
		   }
	if ((RSS.State==RSstTrn)&&(IIR&USART_ISR_TXE))
	{
		USART1->TDR=*RSS.BegPnt;//*RSS.BegPnt=0xcc; //sc_add_30-09-2015
		RSS.BegPnt++;//RSS.TraceCod++;
		if (RSS.BegPnt>RSS.EndPnt)
		{
		  LL_USART_DisableIT_TXE(USART1);
		  LL_USART_EnableIT_TC(USART1);
	            RSS.State=RSstWaitTC_data;
		}
		goto exii;
	}
//	if ((RSS.State==RSstWaitTC_data)&&(IIR & USART_SR_TC)) //!!!!THIS IS A SLAVE CASE!!!!!!!!!
//	{   USART1->BRR=usart1_BRKbdr_value;USART1->DR=0x80;//slow data mode simulate break (or FE)
//		RSS.State=RSstWaitTC_break;
//		goto exii;
//	}
	if ((RSS.State==RSstWaitTC_data)&&(IIR&USART_ISR_TC)) //!!!!THIS IS A SLAVE CASE!!!!!!!!!
	{ //  V--------SLAVE returns in idle, wait new quest from mast
		LL_USART_DisableIT_TC(USART1);LL_USART_DisableDirectionTx(USART1);//!now IDLE state comes form 2x680ohm to +5 and 0V
		USART1_RX_START();
// not needed in slave mode		RSS.CntTimeout=SysTickCntr+100; //+10ms bonus time to slave begin answer
                RSS.State=RSstWait1Rcv;
		goto exii;
	}
	if (RSS.State==RSstRcv)
	{
		*RSS.BegPnt=(uint8_t)USART1->RDR;
	     if (IIR&(USART_ISR_FE|USART_ISR_IDLE|USART_ISR_LBDF))//!now IDLE state comes form 2x680ohm to +5 and 0V
	     {
		     USART1->ICR=(USART_ICR_FECF|USART_ICR_IDLECF|USART_ICR_LBDCF);
		     if (RSS.wCRC==0){USARTinUSE_RX_DISABLE(USART1);RSS.CntTimeout=SysTickCntr;goto exii;}
	     }
		Crc16ModbusFast(&RSS.wCRC,RSS.BegPnt);RSS.CntTimeout=SysTickCntr+3; //~2.5-3chars
		RSS.BegPnt++;if (RSS.BegPnt>=&RSBuffer[RSBUFLEN-1]){RSS.BegPnt--;USARTinUSE_RX_DISABLE(USART1);RSS.CntTimeout=SysTickCntr;} //no waiting
		goto exii;
	}
	RSS.ErrCod=(uint8_t)USART1->RDR;
	RSS.ErrCod=128; //unknown state, TX&RX must be disabled, go in idle
	USARTinUSE_RX_DISABLE(USART1);
	LL_USART_DisableIT_TXE(USART1);LL_USART_DisableIT_TC(USART1);LL_USART_DisableDirectionTx(USART1);
	RSS.State=RSstWait1Rcv; USART1_RX_START();// SLAVE VERSION!!!!!
exii:;
}

//........................................................................................
void USART2_IRQHandler (void) //Master Version
{
unsigned int IIR;
  IIR = USART2->ISR;

   if ((RSS2.State==RSstWait1Rcv)&&(IIR&USART_ISR_RXNE))
   {
    RSS2.BegPnt=&RS2Buffer[0];RSS2.wCRC=0xffff;
    RSS2.CntTimeout=SysTickCntr+3;
    *RSS2.BegPnt=(uint8_t)USART2->RDR;Crc16ModbusFast(&RSS2.wCRC,RSS2.BegPnt);
    RSS2.BegPnt++;
    if (IIR&(USART_ISR_FE|USART_ISR_IDLE|USART_ISR_LBDF)){USART1->ICR=(USART_ICR_FECF|USART_ICR_IDLECF|USART_ICR_LBDCF);}
    LL_USART_EnableIT_IDLE(USART1);
    RSS.State=RSstRcv;
    goto exii;
   }
   if ((RSS2.State==RSstTrn)&&(IIR & USART_ISR_TXE))
   {
		USART2->TDR=*RSS2.BegPnt;//*RSS2.BegPnt=0xcc; //sc_add_30-09-2015
		RSS2.BegPnt++;//RSS2.TraceCod++;
		if (RSS2.BegPnt>RSS2.EndPnt)
		{
			LL_USART_DisableIT_TXE(USART2);
			LL_USART_EnableIT_TC(USART2);
                        RSS2.State=RSstWaitTC_data;
		}
		goto exii;
   }
//	if ((RSS2.State==RSstWaitTC_data)&&(IIR & USART_ISR_TC)) //!!!!THIS IS A SLAVE CASE!!!!!!!!!
//	{   USART1->BRR=usart1_BRKbdr_value;USART1->TDR=0x80;//slow data mode simulate break (or FE)
//		RSS2.State=RSstWaitTC_break;
//		goto exii;
//	}
   if ((RSS2.State==RSstWaitTC_data)&&(IIR & USART_ISR_TC)) //THIS IS A MASTER CASE!!!!!!!!!
   { //!!! V------MASTER!!! go in Rcv, wait ans from slave
     LL_USART_DisableIT_TC(USART2);LL_USART_DisableDirectionTx(USART2); //!now IDLE state comes form 2x680ohm to +5 and 0V
     RSS2.State=RSstWait1Rcv;
     USART2_RX_START();
     RSS2.CntTimeout=SysTickCntr+100+100+RSS2.ExtraCntTimeout; //10+10ms bonus time to slave begin answer
     goto exii;
   }
   if (RSS2.State==RSstRcv)
   {
     *RSS2.BegPnt=(uint8_t)USART2->RDR;
     if (IIR&(USART_ISR_FE|USART_ISR_IDLE|USART_ISR_LBDF)) //!now IDLE state comes form 2x680ohm to +5 and 0V
     {
	     USART2->ICR=(USART_ICR_FECF|USART_ICR_IDLECF|USART_ICR_LBDCF);
	     if (RSS2.wCRC==0){USARTinUSE_RX_DISABLE(USART2);RSS2.CntTimeout=SysTickCntr;goto exii;}
     }
     Crc16ModbusFast(&RSS2.wCRC,RSS2.BegPnt);RSS2.CntTimeout=SysTickCntr+3; //+RSS2.ExtraCntTimeout; 30.07.2015
     RSS2.BegPnt++;
     if (RSS2.BegPnt>=&RS2Buffer[RSBUFLEN-1])
     {RSS2.BegPnt--;USARTinUSE_RX_DISABLE(USART2);RSS2.CntTimeout=SysTickCntr;} //-20;}
     goto exii;
   }
	RSS2.ErrCod=(uint8_t)USART2->RDR;
	RSS2.ErrCod=128; //unknown state, TX&RX must be disabled, go in idle
	USARTinUSE_RX_DISABLE(USART2);
	LL_USART_DisableIT_TXE(USART2);LL_USART_DisableIT_TC(USART2);LL_USART_DisableDirectionTx(USART2);
	RSS2.State=RSstIdle;
exii:;
//	enableInterrupts();
}

//........................................................................................
//........................................................................................
//........................................................................................
void RSC_GetStatus(void)
{
  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&MState,sizeof(MState));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(MState)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(MState));
}
//........................................................................................
void RSC_SetStatus(void)
{
//uint32_t errtmp;
//  errtmp=MState.ErrorFlags;
  memcpy((void*)&MState,(const void*)&RSBuffer[RsPosSubCMD+1],sizeof(MState));
  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&MState,sizeof(MState));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(MState)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(MState));
}
//........................................................................................

//........................................................................................
/*
void RSC_Get7ADCvolt(void)
{
  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&fADC_ArrVolt[0],sizeof(fADC_ArrVolt));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(fADC_ArrVolt)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(fADC_ArrVolt));
}
*/
//........................................................................................
/*
void RSC_Get9ADCspc(void)
{
  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&fADC_cArrSpec[0],sizeof(fADC_cArrSpec));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(fADC_cArrSpec)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(fADC_cArrSpec));
}
*/
//........................................................................................
void RSC_GetSVpBV(void)
{
uint16_t tvart=CurrentSVpBV;
  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&tvart,sizeof(tvart));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(tvart)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(tvart));
}
//........................................................................................

//........................................................................................
//........................................................................................
//........................................................................................
//........................................................................................
//........................................................................................
//........................................................................................
//........................................................................................
void RSC_SetUSERI(void)
{
  memcpy((void*)&USERI,(const void*)&RSBuffer[RsPosSubCMD+1],sizeof(USERI));
  MState.StatusFlags&=~WF_ResetIfSet;
//  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&MState.USERI,sizeof(MState.USERI));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(USERI)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(USERI));
}
//........................................................................................

//........................................................................................

void RSC_SetOSP(void)
{
  memcpy((void*)&OSP,(const void*)&RSBuffer[RsPosSubCMD+1],sizeof(struct OnlySP));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+2]; //only as ok
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1);
}

//........................................................................................
void RSC_SetOlCo(void)
{
  memcpy((void*)&OlCo,(const void*)&RSBuffer[RsPosSubCMD+1],sizeof(OlCo));
//  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&OlCo,sizeof(OlCo));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(OlCo)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(OlCo));
}
//...............................
void RSC_SetOlCoRetStatus(void)
{
//  struct Main_State mMState;
//  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&OlCo,sizeof(OlCo));
  RSBuffer[RsPosCMD]=COMM_GET;  //reformat ansewr code, to be accept from master GetStatus
  RSBuffer[RsPosSubCMD]=SubCmd_StructSTATUS;
//  mMState=MState;
  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&MState,sizeof(MState));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(MState)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(MState));
}
//........................................................................................
void RSC_SetConfig(void)
{
//  uint16_t s16;
  memcpy((void*)&W_CFG,(const void*)&RSBuffer[RsPosSubCMD+1],sizeof(W_CFG));
  R2_CFG=W_CFG;

//  if ((MState.msRunFlag&RM_AutoMask)==0) ConfigNowReceived=0x10;
//  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&R_CFG,sizeof(R_CFG));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(W_CFG)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(W_CFG));
}
//........................................................................................
void RSC_GetConfig(void)
{
//  memcpy((void*)&R2_CFG,(const void*)&RSBuffer[RsPosSubCMD+1],sizeof(R2_CFG));
  memcpy((void*)&RSBuffer[RsPosSubCMD+1],(const void*)&R2_CFG,sizeof(R2_CFG));
  RSS.BegPnt=&RSBuffer[0];RSS.EndPnt=&RSBuffer[RsPosSubCMD+sizeof(R2_CFG)+2];
  Calc_CS_WithCopy((uint8_t*)&RSBuffer[0],(uint8_t*)&RSBuffer[0],RsPosSubCMD+1+sizeof(R2_CFG));
}
//........................................................................................

//........................................................................................
//===========================================================================================//
/*
void AddCRC(volatile uint16_t *pbCRC,volatile uint8_t *pbBuffer)
{
uint16_t wBit,wTmp;
  wBit=(uint16_t)*pbBuffer;
 *pbCRC^=wBit;
  wBit=8;
  do    {
          wTmp=*pbCRC&0x1;
          *pbCRC=*pbCRC>>1;
          if ( wTmp!=0 ) *pbCRC^=0xa001;
          wBit--;
        } while (wBit>0);
}
*/
void Crc16ModbusFast(volatile uint16_t* lCrc, volatile uint8_t* lData) // sourcer32@gmail.com
{
    static const uint16_t CrcTable[16] = { // Nibble lookup for 0xA001 polynomial
        0x0000,0xCC01,0xD801,0x1400,0xF001,0x3C00,0x2800,0xE401,
        0xA001,0x6C00,0x7800,0xB401,0x5000,0x9C01,0x8801,0x4400 };
    uint16_t Crc;
    uint8_t Data;
    Crc=*lCrc; Data=*lData;
    Crc = Crc ^ Data;

    // Process 8-bits, 4 at a time, or 2 rounds

    Crc = (Crc >> 4) ^ CrcTable[Crc & 0xF];
    Crc = (Crc >> 4) ^ CrcTable[Crc & 0xF];

    *lCrc=Crc;
}

//===========================================================================================//
uint16_t Calc_CS(uint8_t* BegAd, uint16_t Len)
{
  uint16_t  wCRC=0xffff;
  for (;Len !=0 ; Len-- )
    {
     Crc16ModbusFast(&wCRC,BegAd);BegAd++;
    }// end for
   return wCRC;
}
//============================================================================================//
void Calc_CS_WithCopy(uint8_t* BegSourceAddr,uint8_t* BegDestAddr, uint16_t Len)
{
  uint16_t  wCRC=0xffff;
  for (;Len !=0 ; Len-- )
  {
    Crc16ModbusFast(&wCRC,BegSourceAddr);
	*BegDestAddr=*BegSourceAddr;
	BegSourceAddr++;BegDestAddr++;
   }// end for

  *BegDestAddr=(uint8_t)wCRC;
  BegDestAddr++;
  *BegDestAddr=(uint8_t)(wCRC>>8);
}
//==============================================================================================//
