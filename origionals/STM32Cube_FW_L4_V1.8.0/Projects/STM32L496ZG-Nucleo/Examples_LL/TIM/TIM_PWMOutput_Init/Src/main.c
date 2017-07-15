/**
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_PWMOutput_Init/Src/main.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    21-April-2017
  * @brief   This example describes how to use a timer peripheral to generate a 
  *          PWM output signal and update PWM duty cycle
  *          using the STM32L4xx TIM LL API.
  *          Peripheral initialization done using LL initialization function.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L4xx_LL_Examples
  * @{
  */

/** @addtogroup TIM_PWMOutput_Init
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Number of output compare modes */
#define TIM_DUTY_CYCLES_NB 11

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Duty cycles: D = T/P * 100%                                                */
/* where T is the pulse duration and P  the period of the PWM signal          */
static uint32_t aDutyCycle[TIM_DUTY_CYCLES_NB] = {
  0,    /*  0% */
  10,   /* 10% */
  20,   /* 20% */
  30,   /* 30% */
  40,   /* 40% */
  50,   /* 50% */
  60,   /* 60% */
  70,   /* 70% */
  80,   /* 80% */
  90,   /* 90% */
  100,  /* 100% */
};

/* Duty cycle index */
static uint8_t iDutyCycle = 0;

/* Measured duty cycle */
__IO uint32_t uwMeasuredDutyCycle = 0;

/* Private function prototypes -----------------------------------------------*/
__STATIC_INLINE void     SystemClock_Config(void);
__STATIC_INLINE void     Configure_TIMPWMOutput(void);
__STATIC_INLINE void     Configure_DutyCycle(uint32_t OCMode);
__STATIC_INLINE void     LED_Init(void);
__STATIC_INLINE void     UserButton_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the system clock to 80 MHz */
  SystemClock_Config();

  /* Initialize LED1 */
  LED_Init();

  /* Initialize button in EXTI mode */
  UserButton_Init();
  
  /* Configure the timer in output compare mode */
  Configure_TIMPWMOutput();

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Configures the timer to generate a PWM signal on the OC1 output.
  * @param  None
  * @retval None
  */
__STATIC_INLINE void  Configure_TIMPWMOutput(void)
{
  LL_TIM_InitTypeDef    tim_initstruct;
  LL_TIM_OC_InitTypeDef tim_oc_initstruct;
  
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  
  /* GPIO TIM3_CH2 configuration */
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_7, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_7, LL_GPIO_AF_2);
  
  /***********************************************/
  /* Configure the NVIC to handle TIM3 interrupt */
  /***********************************************/
  NVIC_SetPriority(TIM3_IRQn, 0);
  NVIC_EnableIRQ(TIM3_IRQn);
  
  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); 
  
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set fields of initialization structure */
  /* - Set the pre-scaler value to have TIM3 counter clock equal to 10 kHz  */
  /* - Set the auto-reload value to have a counter frequency of 100 Hz        */
  tim_initstruct.Prescaler         = __LL_TIM_CALC_PSC(SystemCoreClock, 10000);
  tim_initstruct.CounterMode       = LL_TIM_COUNTERMODE_UP;
  tim_initstruct.Autoreload        = __LL_TIM_CALC_ARR(SystemCoreClock, tim_initstruct.Prescaler, 100);
  tim_initstruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
  tim_initstruct.RepetitionCounter = (uint8_t)0x00;
  
  /* Initialize TIM instance according to parameters defined in               */
  /* initialization structure.                                                */
  LL_TIM_Init(TIM3, &tim_initstruct);
  
  /* Enable TIM3_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */  
  LL_TIM_EnableARRPreload(TIM3);
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set fields of initialization structure                                   */
  /*  - Set compare value to 0 (0% duty cycle )                               */
  /*  To set compare value to half of the counter period (50% duty cycle ),   */
  /*  user could write:                                                       */
  /*  tim_oc_initstruct.CompareValue = ( (LL_TIM_GetAutoReload(TIM3) + 1 ) / 2); */     
  tim_oc_initstruct.OCMode       = LL_TIM_OCMODE_PWM1;
  tim_oc_initstruct.OCState      = LL_TIM_OCSTATE_DISABLE;
  tim_oc_initstruct.OCNState     = LL_TIM_OCSTATE_DISABLE;
  tim_oc_initstruct.CompareValue = 0;
  tim_oc_initstruct.OCPolarity   = LL_TIM_OCPOLARITY_HIGH;
  tim_oc_initstruct.OCNPolarity  = LL_TIM_OCPOLARITY_HIGH;
  tim_oc_initstruct.OCIdleState  = LL_TIM_OCIDLESTATE_LOW;
  tim_oc_initstruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  
  /* Initialize TIM instance according to parameters defined in               */
  /* initialization structure.                                                */
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH2, &tim_oc_initstruct);
  
  /* Enable TIM3_CCR3 register preload. Read/Write operations access the      */
  /* preload register. TIM3_CCR3 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH2);
  
  /**************************/
  /* TIM3 interrupts set-up */
  /**************************/
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC3(TIM3);
  
  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel 3 */
  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
  
  /* Enable counter */
  LL_TIM_EnableCounter(TIM3);
  
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM3);
}

/**
  * @brief  Changes the duty cycle of the PWM signal.
  *         D = (T/P)*100
  *           where T is the pulse duration and P is the PWM signal period
  * @param  D Duty cycle
  * @retval None
  */
__STATIC_INLINE void Configure_DutyCycle(uint32_t D)
{
  uint32_t P;    /* Pulse duration */
  uint32_t T;    /* PWM signal period */
  
  /* PWM signal period is determined by the value of the auto-reload register */
  T = LL_TIM_GetAutoReload(TIM3) + 1;
  
  /* Pulse duration is determined by the value of the compare register.       */
  /* Its value is calculated in order to match the requested duty cycle.      */
  P = (D*T)/100;
  LL_TIM_OC_SetCompareCH2(TIM3, P);
}

/**
  * @brief  Initialize LED1.
  * @param  None
  * @retval None
  */
__STATIC_INLINE void LED_Init(void)
{
  /* Enable the LED1 Clock */
  LED1_GPIO_CLK_ENABLE();

  /* Configure IO in output push-pull mode to drive external LED1 */
  LL_GPIO_SetPinMode(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  //LL_GPIO_SetPinOutputType(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
  //LL_GPIO_SetPinSpeed(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPull(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_PULL_NO);
}

/**
  * @brief  Configures User push-button in GPIO or EXTI Line Mode.
  * @param  None  
  * @retval None
  */
__STATIC_INLINE void UserButton_Init(void)
{
  /* Enable the BUTTON Clock */
  USER_BUTTON_GPIO_CLK_ENABLE();
  
  /* Configure GPIO for BUTTON */
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_PULL_NO);

  /* Connect External Line to the GPIO*/
  USER_BUTTON_SYSCFG_SET_EXTI();
    
  /* Enable a rising trigger EXTI line 13 Interrupt */
  USER_BUTTON_EXTI_LINE_ENABLE();
  USER_BUTTON_EXTI_FALLING_TRIG_ENABLE();
    
  /* Configure NVIC for USER_BUTTON_EXTI_IRQn */
  NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn); 
  NVIC_SetPriority(USER_BUTTON_EXTI_IRQn,0x03);  
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  /* MSI configuration and activation */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  LL_RCC_MSI_Enable();
  while(LL_RCC_MSI_IsReady() != 1) 
  {
  };
  
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1) 
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) 
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 80MHz */
  /* This frequency can be calculated through LL RCC macro */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ(__LL_RCC_CALC_MSI_FREQ(LL_RCC_MSIRANGESEL_RUN, LL_RCC_MSIRANGE_6), 
                                  LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2)*/
  LL_Init1msTick(80000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(80000000);
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  User button interrupt processing
  * @note   When the user key button is pressed the PWM duty cycle is updated. 
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Set new duty cycle */
  iDutyCycle = (iDutyCycle + 1) % TIM_DUTY_CYCLES_NB;

  /* Change PWM signal duty cycle */
  Configure_DutyCycle(aDutyCycle[iDutyCycle]);
}

/**
  * @brief  Timer capture/compare interrupt processing
  * @param  None
  * @retval None
  */
void TimerCaptureCompare_Callback(void)
{
  uwMeasuredDutyCycle = (LL_TIM_GetCounter(TIM3) * 100) / ( LL_TIM_GetAutoReload(TIM3) + 1 );
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
