#include "timer.h"

extern Clock_t sysclks;

/* 
 * timerX_isr
 * timerX ISR (Interrupt Service Routine)
 */
static OnTick callback2 = 0;
static OnTick callback3 = 0;
static OnTick callback4 = 0;

void TIM2_IRQHandler() {
	if (callback2) callback2();
	TIM2->SR &= ~0x1F;
}

void TIM3_IRQHandler() {
	if (callback3) callback3();
	TIM3->SR &= ~0x1F;
}

void TIM4_IRQHandler() {
	if (callback4) callback4();
	TIM4->SR &= ~0x1F;
}


/* 
 * timer_tick_init
 * setup timer to call cb function periodically, each tick_ms
 */
int timer_tick_init(TIM_TypeDef *tmr, uint32_t tick_ms, OnTick cb) {
	IRQn_Type irqn;
	uint32_t irq_priority, clk;
	//get back the clock frequency
	clk = sysclks.apb1_timer_freq;

	if (tmr == TIM2) {
		// register callback function
		callback2 = cb;
		irqn = TIM2_IRQn;
		irq_priority = TIM2_IRQ_PRIORITY;

		// enable timer clocking
		RCC->APB1ENR |=1<<0;

	} else if (tmr == TIM3) {
		// register callback function
		callback3 = cb;
		irqn = TIM3_IRQn;
		irq_priority = TIM3_IRQ_PRIORITY;

		// enable timer clocking
		RCC->APB1ENR |=1<<1;

	} else if (tmr == TIM4) {
		// register callback function
		callback4 = cb;
		irqn = TIM4_IRQn;
		irq_priority = TIM4_IRQ_PRIORITY;

		// enable timer clocking
		RCC->APB1ENR |=1<<2;
	
	} else {
		return -1;
	}

	// clear pending interrupts
	tmr->SR &= !1;

	// set mode
	tmr->CR1 = (1<<7);  	//buffering
	tmr->DIER = (1<<0); 	//Enable interrupts

	// set prescaler 100us
	tmr->PSC = clk/(10000-1); //100µs = (PSC+1)*Tclk

	// set period
	if(timer_tick_period(tmr,tick_ms)) return -1; 

	//enable counter
	tmr->CR1= (1<<0)| tmr->CR1;

	if (cb) {
		NVIC_SetPriority(irqn,irq_priority);	//enable interuptions
		NVIC_EnableIRQ(irqn);
	}

	return 0;
}

/* 
 * timer_tick_period
 * change the tick_ms period
 */
int timer_tick_period(TIM_TypeDef *tmr, uint32_t tick_ms) {
	// set period
	tmr->ARR = tick_ms*10-1; //tickms = (ARR+1)Tpsc

	// force update to reset counter and prescaler
	tmr->EGR |= 1;
	return 0;
}

/* 
 * timer_start
 * reset & enable counting
 */
void timer_start(TIM_TypeDef *tmr) {
	// force update to reset counter and prescaler
	tmr->EGR |= 1;

	// enable counting
	tmr->CR1 |= 1;
}

/* 
 * timer_stop
 * stop counting
 */
void timer_stop(TIM_TypeDef *tmr) {
	// disable counting
	tmr->CR1 &= !1;

}

