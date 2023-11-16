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
#include "html_builder.h"

#ifdef __ICCARM__
#include <LowLevelIOInterface.h>
#endif

/* Private defines -----------------------------------------------------------*/
#define PORT           80

#define TERMINAL_USE


#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000
#define SOCKET                 0


#ifdef  TERMINAL_USE
#define LOG(a) printf a
#else
#define LOG(a)
#endif

#define SSID_SIZE     100
#define PASSWORD_SIZE 100
#define USER_CONF_MAGIC                 0x0123456789ABCDEFuLL

/* Private typedef------------------------------------------------------------*/

typedef struct {
  char ssid[SSID_SIZE];
  char password[PASSWORD_SIZE];
  uint8_t security;
} wifi_config_t;

typedef struct {
  uint64_t      wifi_config_magic;        /**< The USER_CONF_MAGIC magic word signals that the wifi config
                                               (wifi_config_t) is present in Flash. */
  wifi_config_t wifi_config;
} user_config_t;


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined (TERMINAL_USE)
extern UART_HandleTypeDef hDiscoUart;
#endif /* TERMINAL_USE */

static volatile uint8_t button_flag = 0;
static user_config_t user_config;

static  uint8_t http[5000];
static  uint8_t  IP_Addr[4];
static  int     LedState = 0;

/* Private function prototypes -----------------------------------------------*/
#if defined (TERMINAL_USE)
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
#endif /* TERMINAL_USE */

static void SystemClock_Config(void);
static WIFI_Status_t SendWebPage(uint8_t ledIsOn, uint8_t temperature);
static int wifi_server(void);
static int wifi_start(void);
static int wifi_connect(void);
static bool WebServerProcess(void);
static void Button_ISR(void);
static void Button_Reset(void);
static uint8_t Button_WaitForPush(uint32_t delay);

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

  /* Configure LED2 */
  BSP_LED_Init(LED2);

  /* USER push button is used to ask if reconfiguration is needed */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* WIFI Web Server demonstration */
#if defined (TERMINAL_USE)
  /* Initialize all configured peripherals */
  hDiscoUart.Instance = DISCOVERY_COM1;
  hDiscoUart.Init.BaudRate = 115200;
  hDiscoUart.Init.WordLength = UART_WORDLENGTH_8B;
  hDiscoUart.Init.StopBits = UART_STOPBITS_1;
  hDiscoUart.Init.Parity = UART_PARITY_NONE;
  hDiscoUart.Init.Mode = UART_MODE_TX_RX;
  hDiscoUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hDiscoUart.Init.OverSampling = UART_OVERSAMPLING_16;
  hDiscoUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hDiscoUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


  BSP_COM_Init(COM1, &hDiscoUart);
  BSP_TSENSOR_Init();
//  BSP_PSENSOR_Init();
//  BSP_HSENSOR_Init();

  printf("\n****** WIFI Web Server demonstration ******\n\r");

#endif /* TERMINAL_USE */

  wifi_server();
}

/**
  * @brief  Send HTML page
  * @param  None
  * @retval None
  */


static int wifi_start(void)
{
  uint8_t  MAC_Addr[6];

 /*Initialize and use WIFI module */
  if(WIFI_Init() ==  WIFI_STATUS_OK)
  {
    printf("eS-WiFi Initialized.\n\r");
    if(WIFI_GetMAC_Address(MAC_Addr, sizeof(MAC_Addr)) == WIFI_STATUS_OK)
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

  memset(&user_config, 0, sizeof(user_config));

//  Set wifi config
  printf("Configuring SSID and password.\n\r");
  strcpy(user_config.wifi_config.ssid, "Philippe");
  char c = '3';
  user_config.wifi_config.security = c - '0';
  strcpy(user_config.wifi_config.password, "hahahaha");
  user_config.wifi_config_magic = USER_CONF_MAGIC;
// Try to connect to wifi
  printf("Connecting to %s\n\r", user_config.wifi_config.ssid);
  WIFI_Ecn_t security =  WIFI_ECN_WPA2_PSK;

  if (WIFI_Connect(user_config.wifi_config.ssid, user_config.wifi_config.password, security) == WIFI_STATUS_OK)
  {
    if(WIFI_GetIP_Address(IP_Addr, sizeof(IP_Addr)) == WIFI_STATUS_OK)
    {
      LOG(("eS-WiFi module connected: got IP Address : %d.%d.%d.%d\n\r",
               IP_Addr[0],
               IP_Addr[1],
               IP_Addr[2],
               IP_Addr[3]));
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
  if (wifi_connect()!=0) return -1;


  if (WIFI_STATUS_OK!=WIFI_StartServer(SOCKET, WIFI_TCP_PROTOCOL, 1, "", PORT))
  {
    LOG(("ERROR: Cannot start server.\n"));
  }

  LOG(("Server is running and waiting for an HTTP  Client connection to %d.%d.%d.%d\n\r",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));

  do
  {
    uint8_t RemoteIP[4];
    uint16_t RemotePort;

    LOG(("Waiting connection to http://%d.%d.%d.%d\n\r",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));
    while (WIFI_STATUS_OK != WIFI_WaitServerConnection(SOCKET, 1000, RemoteIP, sizeof(RemoteIP), &RemotePort))
    {
        LOG(("."));
    }

    LOG(("\nClient connected %d.%d.%d.%d:%d\n\r",RemoteIP[0],RemoteIP[1],RemoteIP[2],RemoteIP[3],RemotePort));

    StopServer = WebServerProcess();

    if (WIFI_CloseServerConnection(SOCKET) != WIFI_STATUS_OK)
    {
      LOG(("ERROR: failed to close current Server connection\n\r"));
      return -1;
    }
  }
  while(StopServer == false);

  if (WIFI_STATUS_OK!=WIFI_StopServer(SOCKET))
  {
    LOG(("ERROR: Cannot stop server.\n\r"));
  }

  LOG(("Server is stop\n"));
  return 0;
}


static bool WebServerProcess(void)
{
  uint8_t temp;
  uint8_t pres;
  uint16_t  respLen;
  static   uint8_t resp[1024];
  bool    stopserver=false;

  if (WIFI_STATUS_OK == WIFI_ReceiveData(SOCKET, resp, 1000, &respLen, WIFI_READ_TIMEOUT))
  {
   LOG(("get %d byte from server\n\r",respLen));

   if( respLen > 0)
   {
      if(strstr((char *)resp, "GET")) /* GET: put web page */
      {
        temp = (uint8_t) BSP_TSENSOR_ReadTemp();
//        pres = (uint8_t) BSP_PSENSOR_ReadPres();
        if(SendWebPage(LedState, temp) != WIFI_STATUS_OK)
        {
          LOG(("> ERROR : Cannot send web page\n\r"));
        }
        else
        {
          LOG(("Send page after  GET command\n\r"));
        }
       }
       else if(strstr((char *)resp, "POST"))/* POST: received info */
       {
         LOG(("Post request\n\r"));

         if(strstr((char *)resp, "radio"))
         {
           if(strstr((char *)resp, "radio=0"))
           {
             LedState = 0;
             BSP_LED_Off(LED2);
           }
           else if(strstr((char *)resp, "radio=1"))
           {
             LedState = 1;
             BSP_LED_On(LED2);
           }
           temp = (int) BSP_TSENSOR_ReadTemp();
         }
         if(strstr((char *)resp, "stop_server"))
         {
           if(strstr((char *)resp, "stop_server=0"))
           {
             stopserver = false;
           }
           else if(strstr((char *)resp, "stop_server=1"))
           {
             stopserver = true;
           }
         }
         temp = (int) BSP_TSENSOR_ReadTemp();
         if(SendWebPage(LedState, temp) != WIFI_STATUS_OK)
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
static WIFI_Status_t SendWebPage(uint8_t ledIsOn, uint8_t temp)
{
  /* construct web page content */
  strcpy((char *)http, (char *)"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");
  strcat((char *)http, inject(0, temp, 50));

  /* http is the buffer which contains the data to send. */
  /* httpDataLength is the length of the data to be sent. */
  WIFI_Status_t ret;
  uint32_t httpDataLength = strlen(http);
  uint32_t dataLengthToSend;
  uint32_t dataLengthSent = 0;

  while( httpDataLength > 0 )
  {
      if(httpDataLength > 500)
      {
          dataLengthToSend = 500;
      }
      else
      {
          dataLengthToSend = httpDataLength;
      }

      uint16_t curDataLengthSent;
      ret = WIFI_SendData(0, &http[dataLengthSent], dataLengthToSend, &curDataLengthSent, WIFI_WRITE_TIMEOUT );

      if( ret != WIFI_STATUS_OK)
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
//  ret = WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT);

//  if((ret == WIFI_STATUS_OK) && (SentDataLength != strlen((char *)http)))
//  {
//    ret = WIFI_STATUS_ERROR;
//  }

  return ret;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
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
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief Reset button state
  *        To be called before Button_WaitForPush()
  */
void Button_Reset()
{
  button_flag = 0;
}

/**
  * @brief Waiting for button to be pushed
  */
uint8_t Button_WaitForPush(uint32_t delay)
{
  uint32_t time_out = HAL_GetTick() + delay;

  do
  {
    if (button_flag > 0)
    {
      return button_flag;
    }
    HAL_Delay(100);
  }
  while (HAL_GetTick() < time_out);

  return 0;
}

#if defined (TERMINAL_USE)
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&hDiscoUart, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


#ifdef __ICCARM__
/**
  * @brief
  * @param
  * @retval
  */
size_t __read(int handle, unsigned char * buffer, size_t size)
{
  int nChars = 0;

  /* handle ? */

  for (/* Empty */; size > 0; --size)
  {
    uint8_t ch = 0;
    while (HAL_OK != HAL_UART_Receive(&hDiscoUart, (uint8_t *)&ch, 1, 30000))
    {
      ;
    }

    *buffer++ = ch;
    ++nChars;
  }

  return nChars;
}
#elif defined(__CC_ARM) || defined(__GNUC__)
/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. read a character on USART and loop until the end of read */
  uint8_t ch = 0;
  while (HAL_OK != HAL_UART_Receive(&hDiscoUart, (uint8_t *)&ch, 1, 30000))
  {
    ;
  }
  return ch;
}
#endif /* defined(__CC_ARM)  */
#endif /* TERMINAL_USE */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif


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
  button_flag++;
}

