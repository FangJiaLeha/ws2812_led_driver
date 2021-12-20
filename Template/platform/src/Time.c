#include "Time.h"




void timer15_config(uint16_t pres,uint16_t per)
{
    /* ----------------------------------------------------------------------------
    TIMER2 Configuration: 
    TIMER2CLK = SystemCoreClock/8400(GD32F330)or 10800(GD32F350) = 10KHz.
    the period is 1s(10000/10000 = 1s).
    ---------------------------------------------------------------------------- */
    //timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER15);

    /* deinit a TIMER */
    timer_deinit(TIMER15);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
#ifdef GD32F330
    timer_initpara.prescaler         = pres;
#endif /* GD32F330 */
#ifdef GD32F350
    timer_initpara.prescaler         = 10799;
#endif /* GD32F350 */
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = per;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER15, &timer_initpara);

    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER15, TIMER_INT_FLAG_UP);
    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER15, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER15);
		
		nvic_irq_enable(TIMER15_IRQn, 15,0);
}
