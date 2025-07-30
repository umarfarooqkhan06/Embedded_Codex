/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : MAX30102 Heart Rate and SpO2 Monitor - Production Version
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MAX30102/MAX30102.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Printf redirection to UART
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

// Convenience helper for sending strings over UART2
void UART2_SendString(const char *str)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  // Indicate that UART2 is working
  UART2_SendString("UART2 initialized\r\n");

  /* USER CODE BEGIN 2 */

  // Wait for system to stabilize
  HAL_Delay(1000);

  printf("\r\n");
  printf("===========================================\r\n");
  printf("     MAX30102 Heart Rate & SpO2 Monitor   \r\n");
  printf("              Version 1.0                 \r\n");
  printf("===========================================\r\n");
  printf("Initializing system...\r\n\r\n");

  // Initialize MAX30102
  printf("Connecting to MAX30102 sensor...\r\n");
  if (HAL_I2C_IsDeviceReady(&hi2c1, MAX30102_ADDRESS, 3, 1000) != HAL_OK) {
    printf("ERROR: MAX30102 not found!\r\n");
    printf("Please check your connections and restart.\r\n");
    while(1) {
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
      HAL_Delay(200);
    }
  }

  printf("MAX30102 sensor connected successfully!\r\n");

  if (Max30102_Init(&hi2c1) != MAX30102_OK) {
    printf("ERROR: Sensor initialization failed!\r\n");
    while(1) {
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
      HAL_Delay(100);
    }
  }

  printf("Sensor initialized and ready!\r\n\r\n");

  printf("Instructions:\r\n");
  printf("   - Place your finger gently on the sensor\r\n");
  printf("   - Cover both RED and IR LEDs completely\r\n");
  printf("   - Keep your finger still during measurement\r\n");
  printf("   - Wait 10-15 seconds for stable readings\r\n\r\n");

  printf("Measurement Display Format:\r\n");
  printf("   [MM:SS] Heart Rate: XXX bpm | SpO2: XX%% | Finger: YES/NO\r\n");

  printf("\r\n===========================================\r\n");
  printf("Starting continuous monitoring...\r\n\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t last_measurement_time = 0;
  uint32_t led_toggle_time = 0;
  uint32_t startup_time = HAL_GetTick();
  uint8_t measurement_count = 0;
  uint8_t last_finger_state = 0;
  int32_t hr_history[5] = {0};  // Store last 5 HR readings for averaging
  int32_t spo2_history[5] = {0}; // Store last 5 SpO2 readings for averaging
  uint8_t history_index = 0;

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // Run MAX30102 state machine
    Max30102_Task();

    // Display measurements every 2 seconds
    if (HAL_GetTick() - last_measurement_time >= 2000) {
        last_measurement_time = HAL_GetTick();

        int32_t heart_rate = Max30102_GetHeartRate();
        int32_t spo2 = Max30102_GetSpO2Value();
        uint8_t finger_detected = Max30102_IsFingerOnSensor();

        // Print timestamp
        uint32_t seconds = (HAL_GetTick() - startup_time) / 1000;
        printf("[%02lu:%02lu] ", seconds / 60, seconds % 60);

        // Check for finger state changes
        if (finger_detected != last_finger_state) {
            if (finger_detected) {
                printf("Finger detected! Starting measurement...\r\n");
                measurement_count = 0;
                // Clear history when finger is newly detected
                for(int i = 0; i < 5; i++) {
                    hr_history[i] = 0;
                    spo2_history[i] = 0;
                }
                history_index = 0;
            } else {
                printf("Finger removed. Place finger to continue...\r\n");
            }
            last_finger_state = finger_detected;
        }

        if (finger_detected) {
            // Store valid readings in history for averaging
            if (heart_rate > 40 && heart_rate < 200) {
                hr_history[history_index] = heart_rate;
            }
            if (spo2 > 80 && spo2 <= 100) {
                spo2_history[history_index] = spo2;
            }
            history_index = (history_index + 1) % 5;

            // Calculate averages for more stable readings
            int32_t hr_avg = 0, spo2_avg = 0;
            int hr_count = 0, spo2_count = 0;

            for(int i = 0; i < 5; i++) {
                if(hr_history[i] > 0) {
                    hr_avg += hr_history[i];
                    hr_count++;
                }
                if(spo2_history[i] > 0) {
                    spo2_avg += spo2_history[i];
                    spo2_count++;
                }
            }

            printf("Heart Rate: ");
            if (hr_count > 0) {
                hr_avg /= hr_count;
                printf("%3ld bpm", hr_avg);

                // Add heart rate status indicator
                if (hr_avg < 60) printf(" (Low)");
                else if (hr_avg > 100) printf(" (High)");
                else printf(" (Normal)");
            } else {
                printf("--- bpm");
            }

            printf(" | SpO2: ");
            if (spo2_count > 0) {
                spo2_avg /= spo2_count;
                printf("%2ld", spo2_avg);
                printf("%%");

                // Add SpO2 status indicator
                if (spo2_avg >= 98) printf(" (Excellent)");
                else if (spo2_avg >= 95) printf(" (Good)");
                else if (spo2_avg >= 90) printf(" (Fair)");
                else printf(" (Low)");
            } else {
                printf("--");
                printf("%%");
            }

            printf(" | Finger: YES");

            if (measurement_count < 5) {
                printf(" | Stabilizing (%d/5)", measurement_count + 1);
                measurement_count++;
            } else {
                printf(" | Stable");
            }

        } else {
            printf("Heart Rate: --- bpm | SpO2: --");
            printf("%% | Finger: NO | Place finger on sensor");
        }

        printf("\r\n");
    }

    // Toggle status LED every 1 second to show system is alive
    if (HAL_GetTick() - led_toggle_time >= 1000) {
        led_toggle_time = HAL_GetTick();
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    }

    // Small delay to prevent overwhelming the system
    HAL_Delay(10);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */

/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 (MAX30102 Interrupt) */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 (Status LED) */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */

/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// GPIO External Interrupt Callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_0)
  {
    // MAX30102 interrupt triggered
    Max30102_InterruptCallback();
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  printf("FATAL ERROR: System halted!\r\n");
  while (1)
  {
    // Blink LED rapidly to indicate fatal error
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(50);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  printf("Assert failed: %s on line %lu\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
