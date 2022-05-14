#include <stdint.h>

#include <stm32f0xx.h>

#include <stdlib.h>

uint32_t SystemCoreClock = 48000000UL;
uint32_t SystemPeripheralClock = 24000000UL;

#define STDOUT_UART_BAUDRATE    115200UL

static void HWInit(void)
{
        RCC->AHBENR = RCC_AHBENR_GPIOAEN;
        RCC->APB1ENR = RCC_APB1ENR_USART2EN;

        // USART2 - GPIOA2 - tx
        GPIOA->MODER &= ~GPIO_MODER_MODER2;
        GPIOA->MODER |= GPIO_MODER_MODER2_1;
        GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
        GPIOA->AFR[0] |= (0x01 << GPIO_AFRL_AFSEL2_Pos);
        
        // USART2 - GPIOA3 - rx
        GPIOA->MODER &= ~GPIO_MODER_MODER3;
        GPIOA->MODER |= GPIO_MODER_MODER3_1;
        GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3;
        GPIOA->AFR[0] |= (0x01 << GPIO_AFRL_AFSEL3_Pos);

        // USART2 - config and enable
        USART2->CR1 = 0;
        USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
        USART2->BRR = SystemPeripheralClock / STDOUT_UART_BAUDRATE;
        USART2->CR1 |= USART_CR1_UE;
}

static void ClockInit(void)
{
        // Prepare and reset defaults
        RCC->CR |= RCC_CR_HSION;
        RCC->CFGR = 0;
        RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);
        RCC->CR &= ~RCC_CR_HSEBYP;
        RCC->CIR = 0x00000000;

        RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE)) | RCC_CFGR_HPRE_DIV1;       // set AHB = 48 MHz
        RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PPRE)) | RCC_CFGR_PPRE_DIV2;       // set APB = 24 MHz

        RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PLLMUL)) | ((12 - 2) << RCC_CFGR_PLLMUL_Pos);   // set PLLMUL = 12 (div = 2)
        RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PLLSRC)) | RCC_CFGR_PLLSRC_HSI_DIV2; // set PLL source to HSI

        RCC->CR |= RCC_CR_PLLON;                        // PLL on
        while ((RCC->CR & RCC_CR_PLLRDY) == 0) {};      // wait until PLL ready

        FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

        RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_SW)) | RCC_CFGR_SW_PLL; 	// select PLL output as system clock
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {};      // wait until clock switched to PLL
}

void SystemInit(void)
{
        ClockInit();
        HWInit();
}
