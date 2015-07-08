/**
  ******************************************************************************
  * @file    LTDC/LTDC_Display_2Layers/Src/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-May-2015
  * @brief   This example describes how to configure the LTDC peripheral 
  *          to display two Layers at the same time.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#include "RGB565_320x240.h"
#include "L8_320x240.h"

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup LTDC_Display_2Layer
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LTDC_HandleTypeDef LtdcHandle;

/* Private function prototypes -----------------------------------------------*/
static void LCD_Config(void); 
static void SystemClock_Config(void);
static void Error_Handler(void);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  uint32_t toBottom = 0;
  uint32_t toTop = 0;

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();
	
  /* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();
  
  /* Configure the system clock to 216 MHz */
  SystemClock_Config();
  
  /*## LTDC Clock Configuration ###########################################*/  
  if(stmpe811_ts_drv.ReadID(TS_I2C_ADDRESS) == STMPE811_ID)
  {
    /* LCD clock configuration */
    /* LCD clock configuration */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
    /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);  
    
  }
  else
  {
    /* LCD clock configuration */
    /* LCD clock configuration */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/2 = 96 Mhz */
    /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 96/4 = 24Mhz */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);  
  }
  
  /* Configure LED3 */
  BSP_LED_Init(LED3);   
  
  /*##-1- LCD Configuration ##################################################*/ 
  /* Configure 2 layers w/ Blending and CLUT loading for layer 1 */
  LCD_Config(); 

  /*##-2- CLUT Configuration #################################################*/
  HAL_LTDC_ConfigCLUT(&LtdcHandle, (uint32_t *)L8_320x240_CLUT, 256, 0); 
  
  /*##-3- Enable CLUT For Layer 1 ############################################*/
  HAL_LTDC_EnableCLUT(&LtdcHandle, 0);   

  while (1)
  {
    for (toBottom = 0; toBottom < 32; toBottom++)
    {
      /* move the picture */
      /* reconfigure the layer1 position */
      HAL_LTDC_SetWindowPosition(&LtdcHandle, (toBottom*5), (toBottom), 0);
      /* reconfigure the layer2 position */
      HAL_LTDC_SetWindowPosition(&LtdcHandle, (160 - (toBottom*5)), (32 - toBottom), 1);
      HAL_Delay(50);
    }
    HAL_Delay(500);
    for (toTop = 0; toTop < 32; toTop++)
    {
      /* move the picture */
      /* reconfigure the layer1 position */
      HAL_LTDC_SetWindowPosition(&LtdcHandle, (160 - (toTop*5)), (32 - (toTop)), 0);
      /* reconfigure the layer2 position */
      HAL_LTDC_SetWindowPosition(&LtdcHandle, (toTop*5), toTop, 1);
      HAL_Delay(50);
    }
    HAL_Delay(500);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Activate the OverDrive to reach the 216 MHz Frequency */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
}

/**
  * @brief LCD Configuration.
  * @note  This function Configure tha LTDC peripheral :
  *        1) Configure the Pixel Clock for the LCD
  *        2) Configure the LTDC Timing and Polarity
  *        3) Configure the LTDC Layer 1 :
  *           - indirect color (L8) as pixel format  
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 320x240
  *        4) Configure the LTDC Layer 2 :
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 320x240  
  *        5) Load 256 colors in CLUT address for Layer 1                       
  * @retval
  *  None
  */
static void LCD_Config(void)
{  
  LTDC_LayerCfgTypeDef      pLayerCfg;
  LTDC_LayerCfgTypeDef      pLayerCfg1;
  
/* LTDC Initialization -------------------------------------------------------*/
  
  /* Polarity configuration */
  /* Initialize the horizontal synchronization polarity as active low */
  LtdcHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  /* Initialize the vertical synchronization polarity as active low */ 
  LtdcHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
  /* Initialize the data enable polarity as active low */ 
  LtdcHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL; 
  /* Initialize the pixel clock polarity as input pixel clock */  
  LtdcHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  
  /* Timing configuration */
  if(stmpe811_ts_drv.ReadID(TS_I2C_ADDRESS) == STMPE811_ID)
  {  
    /* The AMPIRE LCD 480x272 is selected */
    /* Timing Configuration */    
    LtdcHandle.Init.HorizontalSync = (AMPIRE480272_HSYNC - 1);
    LtdcHandle.Init.VerticalSync = (AMPIRE480272_VSYNC - 1);
    LtdcHandle.Init.AccumulatedHBP = (AMPIRE480272_HSYNC + AMPIRE480272_HBP - 1);
    LtdcHandle.Init.AccumulatedVBP = (AMPIRE480272_VSYNC + AMPIRE480272_VBP - 1);  
    LtdcHandle.Init.AccumulatedActiveH = (AMPIRE480272_HEIGHT + AMPIRE480272_VSYNC + AMPIRE480272_VBP - 1);
    LtdcHandle.Init.AccumulatedActiveW = (AMPIRE480272_WIDTH + AMPIRE480272_HSYNC + AMPIRE480272_HBP - 1);
    LtdcHandle.Init.TotalHeigh = (AMPIRE480272_HEIGHT + AMPIRE480272_VSYNC + AMPIRE480272_VBP + AMPIRE480272_VFP - 1);
    LtdcHandle.Init.TotalWidth = (AMPIRE480272_WIDTH + AMPIRE480272_HSYNC + AMPIRE480272_HBP + AMPIRE480272_HFP - 1); 
  }
  else
  {
    /* The LCD AMPIRE 640x480 is selected */
    /* Timing configuration */
    LtdcHandle.Init.HorizontalSync = (AMPIRE640480_HSYNC - 1);
    LtdcHandle.Init.VerticalSync = (AMPIRE640480_VSYNC - 1);
    LtdcHandle.Init.AccumulatedHBP = (AMPIRE640480_HSYNC + AMPIRE640480_HBP - 1);
    LtdcHandle.Init.AccumulatedVBP = (AMPIRE640480_VSYNC + AMPIRE640480_VBP - 1);  
    LtdcHandle.Init.AccumulatedActiveH = (AMPIRE640480_HEIGHT + AMPIRE640480_VSYNC + AMPIRE640480_VBP - 1);
    LtdcHandle.Init.AccumulatedActiveW = (AMPIRE640480_WIDTH + AMPIRE640480_HSYNC + AMPIRE640480_HBP - 1);
    LtdcHandle.Init.TotalHeigh = (AMPIRE640480_HEIGHT + AMPIRE640480_VSYNC + AMPIRE640480_VBP + AMPIRE640480_VFP - 1);
    LtdcHandle.Init.TotalWidth = (AMPIRE640480_WIDTH + AMPIRE640480_HSYNC + AMPIRE640480_HBP + AMPIRE640480_HFP - 1); 
  }
  
  /* Configure R,G,B component values for LCD background color */
  LtdcHandle.Init.Backcolor.Blue = 0;
  LtdcHandle.Init.Backcolor.Green = 0;
  LtdcHandle.Init.Backcolor.Red = 0;

  LtdcHandle.Instance = LTDC;
  
/* Layer1 Configuration ------------------------------------------------------*/
  
  /* Windowing configuration */ 
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 320;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 240;
  
  /* Pixel Format configuration*/ 
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_L8;
  
  /* Start Address configuration : frame buffer is located at FLASH memory */
  pLayerCfg.FBStartAdress = (uint32_t)&L8_320x240;
  
  /* Alpha constant (255 totally opaque) */
  pLayerCfg.Alpha = 255;
  
  /* Default Color configuration (configure A,R,G,B component values) */
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  
  /* Configure blending factors */
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  
  /* Configure the number of lines and number of pixels per line */
  pLayerCfg.ImageWidth = 320;
  pLayerCfg.ImageHeight = 240;

/* Layer2 Configuration ------------------------------------------------------*/
  
  /* Windowing configuration */ 
  pLayerCfg1.WindowX0 = 160;
  pLayerCfg1.WindowX1 = 480;
  pLayerCfg1.WindowY0 = 32;
  pLayerCfg1.WindowY1 = 272;
  
  /* Pixel Format configuration*/ 
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  
  /* Start Address configuration : frame buffer is located at FLASH memory */
  pLayerCfg1.FBStartAdress = (uint32_t)&RGB565_320x240;
  
  /* Alpha constant (255 totally opaque) */
  pLayerCfg1.Alpha = 200;
  
  /* Default Color configuration (configure A,R,G,B component values) */
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  
  /* Configure blending factors */
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  
  /* Configure the number of lines and number of pixels per line */
  pLayerCfg1.ImageWidth = 320;
  pLayerCfg1.ImageHeight = 240;  
   
  /* Configure the LTDC */  
  if(HAL_LTDC_Init(&LtdcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }

  /* Configure the Background Layer*/
  if(HAL_LTDC_ConfigLayer(&LtdcHandle, &pLayerCfg, 0) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
  
  /* Configure the Foreground Layer*/
  if(HAL_LTDC_ConfigLayer(&LtdcHandle, &pLayerCfg1, 1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }  
}  

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
    /* Turn LED3 on */
    BSP_LED_On(LED3);
    while(1)
    {
    }
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

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