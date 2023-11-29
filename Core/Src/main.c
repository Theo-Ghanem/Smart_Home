/**
 ******************************************************************************
 * @file    Wifi/WiFi_HTTP_Server/src/main.c
 * @author  MCD Application Team
 * @brief   This file provides main program functions
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2017 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "html_builder.h"

#ifdef __ICCARM__
#include <LowLevelIOInterface.h>
#endif
#define ARM_MATH_CM4
#include "arm_math.h"

/* Private defines -----------------------------------------------------------*/
#define PORT 80

#define TERMINAL_USE

#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT 10000
#define SOCKET 0
#define LOG(a) printf a

#define SSID_SIZE 100
#define PASSWORD_SIZE 100

/* Private typedef------------------------------------------------------------*/

typedef struct
{
    char ssid[SSID_SIZE];
    char password[PASSWORD_SIZE];
    uint8_t security;
} wifi_config_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

volatile uint8_t button_flag = 0;
static uint16_t distance = 8190;
static statInfo_t_VL53L0X distanceStr;
static wifi_config_t wifi_config;
volatile uint8_t temp;
volatile uint16_t pres;
volatile uint8_t humd;

volatile uint8_t alarmEnabled = 0;
volatile uint8_t intruderDetected = 0;

static uint8_t http[5000];
static uint8_t IP_Addr[4];

DAC_HandleTypeDef hdac1;
UART_HandleTypeDef huart1;
I2C_HandleTypeDef hi2c2;

osThreadId taskWifiHandle;
osThreadId taskAlarmHandle;
osThreadId taskSensorsHandle;
osThreadId taskButtonHandle;

/* Private function prototypes -----------------------------------------------*/

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

static void SystemClock_Config(void);
static WIFI_Status_t SendWebPage(uint8_t alarmEnabled, uint8_t intruderDetected, uint8_t temp, uint16_t pres, uint8_t humd);
static int wifi_server(void);
static int wifi_start(void);
static int wifi_connect(void);
static bool WebServerProcess(void);
static void Button_ISR(void);
static void MX_GPIO_Init(void);
static void MX_DAC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C2_Init(void);
static void MX_VL53L0X_Init(void);

void StartTaskWifi(void const *argument)
{
    wifi_server();
}

void StartTaskAlarm(void const *argument)
{
    float32_t angle = 0;
    for(;;)
    {
        osDelay(1);

        if(alarmEnabled && intruderDetected){
            float32_t sin = arm_sin_f32(angle);
            uint32_t val = (uint32_t) ((sin + 1) * 100);
            HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_8B_R, val);
            angle += 30;
        }
    }
}

void StartTaskSensors(void const *argument)
{
    for (;;)
    {
        osDelay(500);

        // Turn on and off RED LED
        if(alarmEnabled){
            distance = readRangeSingleMillimeters(&distanceStr);
            // LOG(("Distance: %d\n\r", distance));  // Uncomment to debug distance
            if (distance > 3000) {
            	if (intruderDetected != 0){
                	// No intruder
                	intruderDetected = 0;
                	HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, 1);	// Red LED off
            	}
            } else {
            	if (intruderDetected != 1){
                	// Intruder
                	intruderDetected = 1;
                	HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, 0);	// Red LED on
            	}
            }
        }

        temp = (uint8_t)BSP_TSENSOR_ReadTemp();
        pres = (uint16_t)BSP_PSENSOR_ReadPressure();
        humd = (uint8_t)BSP_HSENSOR_ReadHumidity();

    }
}

void StartTaskButton(void const *argument)
{
    for (;;)
    {
    	osDelay(500);
    	if (button_flag == 1) {
    		if (alarmEnabled == 1) {
    			alarmEnabled = 0;
    			HAL_GPIO_WritePin(GREEN2_LED_GPIO_Port, GREEN2_LED_Pin, 0); // LED2 off
    		} else {
    			alarmEnabled = 1;
    			HAL_GPIO_WritePin(GREEN2_LED_GPIO_Port, GREEN2_LED_Pin, 1); // LED2 on
    		}
    		button_flag = 0;
    	}
    }

}

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* USER push button is used to ask if reconfiguration is needed */
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

    BSP_TSENSOR_Init();
    BSP_PSENSOR_Init();
    BSP_HSENSOR_Init();

    // Init peripherals
    MX_GPIO_Init();
    MX_DAC1_Init();
    MX_USART1_UART_Init();
    MX_I2C2_Init();
    MX_VL53L0X_Init();
    BSP_COM_Init(COM1, &huart1);

    // Start peripherals
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);

    printf("\n****** Smart Home Secure Server ******\n\r");

    osThreadDef(taskWifi, StartTaskWifi, osPriorityNormal, 0, 512);
    taskWifiHandle = osThreadCreate(osThread(taskWifi), NULL);

    osThreadDef(taskSensors, StartTaskSensors, osPriorityNormal, 0, 128);
    taskSensorsHandle = osThreadCreate(osThread(taskSensors), NULL);

    osThreadDef(taskAlarm, StartTaskAlarm, osPriorityAboveNormal, 0, 128);
    taskAlarmHandle = osThreadCreate(osThread(taskAlarm), NULL);

    osThreadDef(taskButton, StartTaskButton, osPriorityNormal, 0, 128);
    taskAlarmHandle = osThreadCreate(osThread(taskButton), NULL);

    osKernelStart();

    while (1)
    {
        // We should never be here
    }
}

/**
 * @brief  Send HTML page
 * @param  None
 * @retval None
 */

static int wifi_start(void)
{
    uint8_t MAC_Addr[6];

    /*Initialize and use WIFI module */
    if (WIFI_Init() == WIFI_STATUS_OK)
    {
        printf("eS-WiFi Initialized.\n\r");
        if (WIFI_GetMAC_Address(MAC_Addr, sizeof(MAC_Addr)) == WIFI_STATUS_OK)
        {
            LOG(("eS-WiFi module MAC Address : %02X:%02X:%02X:%02X:%02X:%02X\n\r",
                 MAC_Addr[0],
                 MAC_Addr[1],
                 MAC_Addr[2],
                 MAC_Addr[3],
                 MAC_Addr[4],
                 MAC_Addr[5]));
        }
        else
        {
            LOG(("> ERROR : CANNOT get MAC address\n\r"));
            return -1;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

int wifi_connect(void)
{
    wifi_start();

    memset(&wifi_config, 0, sizeof(wifi_config));

    //  Set wifi config
    printf("Configuring SSID and password.\n\r");
    strcpy(wifi_config.ssid, "Raph iPhone");
    char c = '3';
    wifi_config.security = c - '0';
    strcpy(wifi_config.password, "goodmorning");
    // Try to connect to wifi
    printf("Connecting to %s\n\r", wifi_config.ssid);
    WIFI_Ecn_t security = WIFI_ECN_WPA2_PSK;

    if (WIFI_Connect(wifi_config.ssid, wifi_config.password, security) == WIFI_STATUS_OK)
    {
        if (WIFI_GetIP_Address(IP_Addr, sizeof(IP_Addr)) == WIFI_STATUS_OK)
        {
            LOG(("eS-WiFi module connected: got IP Address : %d.%d.%d.%d\n\r",
                 IP_Addr[0],
                 IP_Addr[1],
                 IP_Addr[2],
                 IP_Addr[3]));
            HAL_GPIO_WritePin(GREEN1_LED_GPIO_Port, GREEN1_LED_Pin, 1);	// Turn LED GREEN1 on
        }
        else
        {
            LOG((" ERROR : es-wifi module CANNOT get IP address\n\r"));
            return -1;
        }
    }
    else
    {
        LOG(("ERROR : es-wifi module NOT connected\n\r"));
        return -1;
    }
    return 0;
}

int wifi_server(void)
{
    bool StopServer = false;

    LOG(("\nRunning HTML Server test\n\r"));
    if (wifi_connect() != 0)
        return -1;

    if (WIFI_STATUS_OK != WIFI_StartServer(SOCKET, WIFI_TCP_PROTOCOL, 1, "", PORT))
    {
        LOG(("ERROR: Cannot start server.\n"));
    }

    LOG(("Server is running and waiting for an HTTP  Client connection to %d.%d.%d.%d\n\r", IP_Addr[0], IP_Addr[1], IP_Addr[2], IP_Addr[3]));

    do
    {
        uint8_t RemoteIP[4];
        uint16_t RemotePort;

        LOG(("Waiting connection to http://%d.%d.%d.%d\n\r", IP_Addr[0], IP_Addr[1], IP_Addr[2], IP_Addr[3]));
        while (WIFI_STATUS_OK != WIFI_WaitServerConnection(SOCKET, 100, RemoteIP, sizeof(RemoteIP), &RemotePort))
        {
            osDelay(300);
            LOG(("."));
        }

        LOG(("\nClient connected %d.%d.%d.%d:%d\n\r", RemoteIP[0], RemoteIP[1], RemoteIP[2], RemoteIP[3], RemotePort));

        StopServer = WebServerProcess();

        if (WIFI_CloseServerConnection(SOCKET) != WIFI_STATUS_OK)
        {
            LOG(("ERROR: failed to close current Server connection\n\r"));
            return -1;
        }
    } while (StopServer == false);

    if (WIFI_STATUS_OK != WIFI_StopServer(SOCKET))
    {
        LOG(("ERROR: Cannot stop server.\n\r"));
    }

    LOG(("Server is stop\n"));
    return 0;
}

static bool WebServerProcess(void)
{
    uint16_t respLen;
    static uint8_t resp[1024];
    bool stopserver = false;

    if (WIFI_STATUS_OK == WIFI_ReceiveData(SOCKET, resp, 1000, &respLen, WIFI_READ_TIMEOUT))
    {
        LOG(("get %d byte from server\n\r", respLen));

        if (respLen > 0)
        {
            if (strstr((char *)resp, "GET")) /* GET: put web page */
            {
                if (SendWebPage(alarmEnabled, intruderDetected, temp, pres, humd) != WIFI_STATUS_OK)
                {
                    LOG(("> ERROR : Cannot send web page\n\r"));
                }
                else
                {
                    LOG(("Send page after  GET command\n\r"));
                }
            }
            else if (strstr((char *)resp, "POST")) /* POST: received info */
            {
                LOG(("Post request\n\r"));

                if (strstr((char *)resp, "radio"))
                {
                    if (strstr((char *)resp, "radio=0"))
                    {
                        alarmEnabled = 0;
                        HAL_GPIO_WritePin(GREEN2_LED_GPIO_Port, GREEN2_LED_Pin, 0); // LED2 off
                    }
                    else if (strstr((char *)resp, "radio=1"))
                    {
                        alarmEnabled = 1;
                        HAL_GPIO_WritePin(GREEN2_LED_GPIO_Port, GREEN2_LED_Pin, 1);	// LED2 on
                    }
                    temp = (int)BSP_TSENSOR_ReadTemp();
                }
                if (strstr((char *)resp, "stop_server"))
                {
                    if (strstr((char *)resp, "stop_server=0"))
                    {
                        stopserver = false;
                    }
                    else if (strstr((char *)resp, "stop_server=1"))
                    {
                        stopserver = true;
                    }
                }
                if (SendWebPage(alarmEnabled, intruderDetected, temp, pres, humd) != WIFI_STATUS_OK)
                {
                    LOG(("> ERROR : Cannot send web page\n\r"));
                }
                else
                {
                    LOG(("Send Page after POST command\n\r"));
                }
            }
        }
    }
    else
    {
        LOG(("Client close connection\n\r"));
    }
    return stopserver;
}

/**
 * @brief  Send HTML page
 * @param  None
 * @retval None
 */
static WIFI_Status_t SendWebPage(uint8_t alarmEnabled, uint8_t intruderDetected, uint8_t temp, uint16_t pres, uint8_t humd)
{
    /* construct web page content */
    strcpy((char *)http, (char *)"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");
    strcat((char *)http, inject(alarmEnabled, intruderDetected, temp, pres, humd));

    /* http is the buffer which contains the data to send. */
    /* httpDataLength is the length of the data to be sent. */
    WIFI_Status_t ret;
    uint32_t httpDataLength = strlen(http);
    uint32_t dataLengthToSend;
    uint32_t dataLengthSent = 0;

    while (httpDataLength > 0)
    {
        if (httpDataLength > 500)
        {
            dataLengthToSend = 500;
        }
        else
        {
            dataLengthToSend = httpDataLength;
        }

        uint16_t curDataLengthSent;
        ret = WIFI_SendData(0, &http[dataLengthSent], dataLengthToSend, &curDataLengthSent, WIFI_WRITE_TIMEOUT);

        if (ret != WIFI_STATUS_OK)
        {
            /* Handle failure (probably print a log). */
            break;
        }
        else
        {
            /* Update what is left to send based on the length of the data actually sent. */
            dataLengthSent += curDataLengthSent;
            httpDataLength -= curDataLengthSent;
        }
    }

    return ret;
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
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_DAC1_Init(void)
{

    DAC_ChannelConfTypeDef sConfig = {0};

    /** DAC Initialization
     */
    hdac1.Instance = DAC1;
    if (HAL_DAC_Init(&hdac1) != HAL_OK)
    {
        Error_Handler();
    }

    /** DAC channel OUT1 config
     */
    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_ABOVE_80MHZ;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x10909CEC;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

static void MX_VL53L0X_Init(void)
{
  	// Initialize the VL53L0X
    // Configure the sensor for high accuracy and speed in 20 cm.
	initVL53L0X(1, &hi2c2);
	setSignalRateLimit(200);
	setVcselPulsePeriod(VcselPeriodPreRange, 10);
	setVcselPulsePeriod(VcselPeriodFinalRange, 14);
	setMeasurementTimingBudget(300 * 1000UL);
}

static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOE_CLK_ENABLE();
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin : RED_LED_Pin */
    GPIO_InitStruct.Pin = RED_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RED_LED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : GREEN1_LED_Pin */
    GPIO_InitStruct.Pin = GREEN1_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GREEN1_LED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : GREEN2_LED_Pin */
    GPIO_InitStruct.Pin = GREEN2_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GREEN2_LED_GPIO_Port, &GPIO_InitStruct);

    // Reset LEDs states
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, 1);
    HAL_GPIO_WritePin(GREEN1_LED_GPIO_Port, GREEN1_LED_Pin, 0);
    HAL_GPIO_WritePin(GREEN2_LED_GPIO_Port, GREEN2_LED_Pin, 0);
}

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
    case (USER_BUTTON_PIN):
    {
        Button_ISR();
        break;
    }
    case (GPIO_PIN_1):
    {
        SPI_WIFI_ISR();
        break;
    }
    default:
    {
        break;
    }
    }
}

/**
 * @brief  SPI3 line detection callback.
 * @param  None
 * @retval None
 */
void SPI3_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi);
}

/**
 * @brief Update button ISR status
 */
static void Button_ISR(void)
{
    button_flag = 1;
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
}

#if defined(TERMINAL_USE)
/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART1 and Loop until the end of transmission */
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}
#endif

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}

#endif
