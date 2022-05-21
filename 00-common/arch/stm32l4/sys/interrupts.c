
void NMI_Handler(void)
{

}

void HardFault_Handler(void)
{
        while (1) {}
}

void MemManage_Handler(void)
{
        while (1) {}
}

void BusFault_Handler(void)
{
        while (1) {}
}

void UsageFault_Handler(void)
{
        while (1) {}
}

// void SVC_Handler(void)
// {

// }

void DebugMon_Handler(void)
{

}

// void PendSV_Handler(void)
// {

// }

// void SysTick_Handler(void)
// {
//         HAL_IncTick();
// }

#include <libopencm3/stm32/lptimer.h>

void LPTIM1_IRQHandler(void)
{
        lptimer_clear_flag(LPTIM1, LPTIM_ICR_CMPMCF);
}
