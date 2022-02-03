#include "variables.h"

volatile struct Oper_StateSend OStSnd={0};
volatile struct Oper_StateRet OStRet={0};
volatile struct Main_State MState={0};
struct Main_State MStateW={0};
struct Some_Info SInf={0};
//volatile uint16_t PWM1_Val,PWM2_Val;



volatile struct OldComands OlCo={0};

struct OnlySP OSP={0};

struct ACFG W_CFG={0},R2_CFG={0};

int32_t PulseCounter=0;
volatile uint16_t USERI=0;














