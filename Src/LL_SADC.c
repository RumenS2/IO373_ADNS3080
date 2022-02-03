/*
 * LL_SADC.c
 *
 *  Created on: 29.03.2019 ã.
 *      Author: ru
 */
#include "LL_SADC.h"


#define SDADC_TIMEOUT          200
#define SDADC_CONFREG_OFFSET   0x00000020
#define SDADC_JDATAR_CH_OFFSET 24
#define SDADC_MSB_MASK         0xFFFF0000U
#define SDADC_LSB_MASK         0x0000FFFFU


static uint32_t SDADC_EnterInitMode(SDADC_HandleTypeDef* hsdadc)
{
  uint32_t tickstart ;

  /* Set INIT bit on SDADC_CR1 register */
  hsdadc->Instance->CR1 |= SDADC_CR1_INIT;

  /* Wait INITRDY bit on SDADC_ISR */
  tickstart = SysTickCntr;
  while((hsdadc->Instance->ISR & SDADC_ISR_INITRDY) == (uint32_t)RESET)
  {
    if((SysTickCntr-tickstart) > (SDADC_TIMEOUT*10))
    {
      return 0x1000;
    }
  }

  /* Return HAL status */
  return 0;
}

/**
  * @brief  This function allows to exit from init mode for SDADC instance.
  * @param  hsdadc SDADC handle.
  * @retval None.
  */
static void SDADC_ExitInitMode(SDADC_HandleTypeDef* hsdadc)
{
  /* Reset INIT bit in SDADC_CR1 register */
  hsdadc->Instance->CR1 &= ~(SDADC_CR1_INIT);
}

/**
  * @brief  This function allows to really start regular conversion.
  * @param  hsdadc SDADC handle.
  * @retval HAL status.
  */
uint32_t LL_SDADC_RegConvStart(SDADC_HandleTypeDef* hsdadc)
{
  uint32_t status = 0;

  /* Check regular trigger */
  if(hsdadc->RegularTrigger == SDADC_SOFTWARE_TRIGGER)
  {
    /* Set RSWSTART bit in SDADC_CR2 register */
    hsdadc->Instance->CR2 |= SDADC_CR2_RSWSTART;
  }
  else /* synchronuous trigger */
  {
    /* Enter init mode */
    if(SDADC_EnterInitMode(hsdadc) != 0)
    {
      /* Set SDADC in error state */
      hsdadc->State = HAL_SDADC_STATE_ERROR;
      status = 0x1000;
    }
    else
    {
      /* Set RSYNC bit in SDADC_CR1 register */
      hsdadc->Instance->CR1 |= SDADC_CR1_RSYNC;

      /* Exit init mode */
      SDADC_ExitInitMode(hsdadc);
    }
  }
  /* Update SDADC state only if status is OK */
  if(status == 0)
  {
    hsdadc->State = (hsdadc->State == HAL_SDADC_STATE_READY) ? \
                    HAL_SDADC_STATE_REG : HAL_SDADC_STATE_REG_INJ;
  }
  /* Return function status */
  return status;
}


uint32_t LL_SDADC_Init(SDADC_HandleTypeDef* hsdadc)
{
  /* Check SDADC handle */
  if(hsdadc == NULL)
  {
    return 0x1000;
  }


  /* Initialize SDADC variables with default values */
  hsdadc->RegularContMode     = SDADC_CONTINUOUS_CONV_OFF;
  hsdadc->InjectedContMode    = SDADC_CONTINUOUS_CONV_OFF;
  hsdadc->InjectedChannelsNbr = 1U;
  hsdadc->InjConvRemaining    = 1U;
  hsdadc->RegularTrigger      = SDADC_SOFTWARE_TRIGGER;
  hsdadc->InjectedTrigger     = SDADC_SOFTWARE_TRIGGER;
  hsdadc->ExtTriggerEdge      = SDADC_EXT_TRIG_RISING_EDGE;
  hsdadc->RegularMultimode    = SDADC_MULTIMODE_SDADC1_SDADC2;
  hsdadc->InjectedMultimode   = SDADC_MULTIMODE_SDADC1_SDADC2;
  hsdadc->ErrorCode           = SDADC_ERROR_NONE;

//  /* Call MSP init function */
//  //HAL_SDADC_MspInit(hsdadc);

  /* Set idle low power and slow clock modes */
  hsdadc->Instance->CR1 &= ~(SDADC_CR1_SBI|SDADC_CR1_PDI|SDADC_CR1_SLOWCK);
  hsdadc->Instance->CR1 |= (hsdadc->Init.IdleLowPowerMode | \
                            hsdadc->Init.SlowClockMode);

  /* Set fast conversion mode */
  hsdadc->Instance->CR2 &= ~(SDADC_CR2_FAST);
  hsdadc->Instance->CR2 |= hsdadc->Init.FastConversionMode;

  /* Set reference voltage common to all SDADC instances */
  /* Update this parameter only if needed to avoid unnecessary settling time */
  if((SDADC1->CR1 & SDADC_CR1_REFV) != hsdadc->Init.ReferenceVoltage)
  {
    /* Voltage reference bits are common to all SADC instances but are        */
    /* present in SDADC1 register.                                            */
    SDADC1->CR1 &= ~(SDADC_CR1_REFV);
    SDADC1->CR1 |= hsdadc->Init.ReferenceVoltage;

    /* Wait at least 2ms before setting ADON */
    Delay100us(20);
  }

  /* Enable SDADC */
  hsdadc->Instance->CR2 |= SDADC_CR2_ADON;

  /* Wait end of stabilization */
  while((hsdadc->Instance->ISR & SDADC_ISR_STABIP) != 0U)
  {
  }

  /* Set SDADC to ready state */
  hsdadc->State = HAL_SDADC_STATE_READY;

  /* Return HAL status */
  return 0;
}

uint32_t LL_SDADC_PrepareChannelConfig(SDADC_HandleTypeDef *hsdadc,
                                                 uint32_t ConfIndex,
                                                 SDADC_ConfParamTypeDef* ConfParamStruct)
{
  uint32_t status = 0;
  uint32_t tmp = 0U;

  /* Check parameters */
//  assert_param(IS_SDADC_ALL_INSTANCE(hsdadc->Instance));
//  assert_param(ConfParamStruct != NULL);

  /* Check SDADC state is ready */
  if(hsdadc->State != HAL_SDADC_STATE_READY)
  {
    status = 0x2;
  }
  else
  {
    /* Enter init mode */
    if(SDADC_EnterInitMode(hsdadc) != 0)
    {
      /* Set SDADC in error state */
      hsdadc->State = HAL_SDADC_STATE_ERROR;
      status = 0x100;
    }
    else
    {
      /* Program configuration register with parameters */
      tmp = (uint32_t)((uint32_t)(hsdadc->Instance) + \
                       SDADC_CONFREG_OFFSET + \
                       (uint32_t)(ConfIndex << 2U));
      *(__IO uint32_t *) (tmp) = (uint32_t) (ConfParamStruct->InputMode | \
                                             ConfParamStruct->Gain | \
                                             ConfParamStruct->CommonMode | \
                                             ConfParamStruct->Offset);
      /* Exit init mode */
      SDADC_ExitInitMode(hsdadc);
    }
  }
  /* Return function status */
  return status;
}
