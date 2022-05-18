#include <stdint.h>
#include <stdlib.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/flash.h>

#include <libopencm3/cm3/scb.h>

uint32_t SystemCoreClock = 80000000UL;

#define STDOUT_UART_BAUDRATE    115200UL

#define VECT_TAB_OFFSET         0x00000000

static void HWInit(void)
{
        rcc_periph_clock_enable(RCC_GPIOA);
        rcc_periph_clock_enable(RCC_USART2);

        gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
        gpio_set_af(GPIOA, GPIO_AF7, GPIO3);

	usart_set_baudrate(USART2, STDOUT_UART_BAUDRATE);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	usart_enable(USART2);
}

static void ClockInit(void)
{
        SCB_CPACR |= ((3UL << 10*2)|(3UL << 11*2));

	rcc_osc_on(RCC_HSI16);
        rcc_wait_for_osc_ready(RCC_HSI16);
	
	flash_prefetch_enable();
	flash_set_ws(4);
	flash_dcache_enable();
	flash_icache_enable();

        rcc_set_main_pll(RCC_PLLCFGR_PLLSRC_HSI16, 4, 40, 0, 0, RCC_PLLCFGR_PLLR_DIV2);
	rcc_osc_on(RCC_PLL);
        rcc_wait_for_osc_ready(RCC_PLL);

        rcc_periph_clock_enable(RCC_SYSCFG);

        rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
	rcc_wait_for_sysclk_status(RCC_PLL);

        rcc_ahb_frequency = SystemCoreClock;
	rcc_apb1_frequency = SystemCoreClock;
	rcc_apb2_frequency = SystemCoreClock;
}

void SystemInit(void)
{
        ClockInit();
        HWInit();
}

void system_pre_sleep(uint32_t *expected_time)
{        
        __asm volatile ( "dsb" ::: "memory" );
        __asm volatile ( "wfi" );
        __asm volatile ( "isb" );
}

void system_post_sleep(uint32_t *expected_time)
{
}
