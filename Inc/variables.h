#ifndef __VARIABLES_H
#define __VARIABLES_H

#include "stm32f3xx.h" //types
//--------main brd erros--------
#define SF_InpPKI             0x00000001
#define SF_InpMKI             0x00000002
#define SF_InpSrvAlram        0x00000004
#define SF_InpSrvNOTReady     0x00000008
#define SF_OutClrAlarm        0x00000010
#define SF_OutSrvON           0x00000020
#define SF_InpZpulse          0x00000040
#define SF_xxxxxxxxxxx        0x00000080
#define EF_HSE_FailedToStart  0x00000100
#define EF_WWG_Reset          0x00000200  //wdg reset
#define EF_ADCNotReady        0x00000400  //some hard err
#define EF_HW_SomeErr         0x00000800  //some hard err
#define FL_TARGET_REACHED     0x00001000
#define SF_Make_Zero_Cal      0x00002000 //? NOT IMPLEMENTED
#define EF_CALIB_IS_LOST      0x00004000 //? NOT IMPLEMENTED
#define EF_BATTERY_PROBLEM    0x00008000 //? NOT IMPLEMENTED
#define EF_COMMAND_REJECTED   0x00010000
#define EF_TREADLE_RELEASED   0x00020000
#define EF_SOME_ALARM_STATE   0x00040000
#define EF_xxxxxx             0x00080000
#define EF_xxxxxxx            0x00100000
#define EF_xxxxxxxx           0x00200000
#define EF_xxxxxxxxx          0x00400000
#define EF_xxxxxxxxxx         0x00800000
#define EF_xxxxxxxxxxx        0x01000000
#define EF_xxxxxxxxxxxx       0x02000000
#define EF_xxxxxxxxxxxxx      0x04000000
#define EF_COMM_TIMEOUT       0x08000000

#define WF_StopIsPressed      0x80000000
#define WF_NotReadyForAuto    0x40000000
#define WF_ResetIfSet         0x20000000
#define WF_SlaveExtCommErr    0x10000000

//--------ext board errors-----------
#define oEF_OverCurrent  0x0001
#define oEF_HSE_FailedToStar 0x0002
#define oEF_WWG_Reset 0x0004
#define oEF_SomeHW_Err 0x0008

#define oWF_StopIsPressed 0x8000

#define ADNSSCANPER 50

struct Oper_StateSend{
 uint16_t  oFlags;
 uint16_t  AODR,BODR,CODR,DODR;
// uint16_t  AIDR,BIDR,CIDR,DIDR;
} __attribute__((__packed__));
struct Oper_StateRet{
 uint16_t  oFlags;
// uint16_t  AODR,BODR,CODR,DODR;
 uint16_t  AIDR,BIDR,CIDR,DIDR;
} __attribute__((__packed__));

struct Main_State{
 uint32_t  StatusFlags;
 int16_t  X,Y;
} __attribute__((__packed__));

struct Some_Info{
 uint8_t  Raw7[8];
} __attribute__((__packed__));

struct OldComands{
uint16_t ButtonsState;
} __attribute__((__packed__));

#define RF_AUTO_PULSING      0x0010
//#define RF_TARGET_REACHED    0x0020
#define RF_START_CMD         0x0030
#define RF_MAN_PLUS	     0x0040
#define RF_MAN_MINUS	     0x0050

#define RF_KIPLUS	     0x0080
#define RF_KIMINUS	     0x0090

#define RF_IDLE		     0x1000

#define RF_ALARM             0xf000
#define RF_ALARM10           0xf010
#define RF_ALARM20           0xf020
#define RF_ALARM30           0xf030

#define RF_JUST_STARTED      0xf0f0

#define KeyMMMask 0x0010
#define KeyMMtst       (OlCo.ButtonsState&0x0010)
#define ResetKeyMMtst  (OlCo.ButtonsState&=~(0x0010))

#define KeyMPMask 0x0020
#define KeyMPtst       (OlCo.ButtonsState&0x0020)
#define ResetKeyMPtst  (OlCo.ButtonsState&=~(0x0020))

#define KeyG1Mask 0x0040
#define KeyG1tst       (OlCo.ButtonsState&0x0040)
#define ResetKeyG1tst  (OlCo.ButtonsState&=~(0x0040))

#define KeyModeAutoMask 0x0080
#define KeyModeAutotst       (OlCo.ButtonsState&0x0080)
#define ResetModeAutotst  (OlCo.ButtonsState&=~(0x0080))


extern volatile struct Oper_StateSend OStSnd;
extern volatile struct Oper_StateRet OStRet;

extern volatile struct Main_State MState;
extern struct Main_State MStateW;

//extern volatile uint16_t PWM1_Val,PWM2_Val;
extern struct Some_Info SInf;

extern volatile struct OldComands OlCo;

struct ACFG {
uint32_t      wID_ConfigValid;
//----
uint16_t      SystemBits;

//------
uint32_t    CS32;
uint32_t ct0xa559c3b7;  //ct used for UNzeroing CS !!!
          } __attribute__((__packed__));

struct OnlySP {
uint16_t      SystemBits;
uint16_t      StartRamp,StopRamp,TSLramp;
float	      fSpeed;
float	      fTargetPos;
                } __attribute__((__packed__)) ;

extern struct OnlySP OSP;

extern struct ACFG W_CFG,R2_CFG;


extern int32_t PulseCounter;
extern volatile uint16_t USERI;

#endif





