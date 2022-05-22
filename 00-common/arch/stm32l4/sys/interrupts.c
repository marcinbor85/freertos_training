
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
#include <libopencm3/stm32/timer.h>

void LPTIM1_IRQHandler(void)
{
        lptimer_clear_flag(LPTIM1, LPTIM_ICR_CMPMCF);
}

extern uint32_t g_runtime_stat_timer_overflow_cntr;

void TIM2_IRQHandler(void)
{
        if (timer_get_flag(TIM2, TIM_SR_UIF) == false)
                return;
        
        timer_clear_flag(TIM2, TIM_SR_UIF);
        g_runtime_stat_timer_overflow_cntr++;
}
