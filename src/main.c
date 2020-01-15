#include "drivers/rcc.h"
#include "drivers/io.h"

Clock_t sysclks;

#include "drivers/timer.h"

int val = 0;

static void timeout_cb(void) {
	io_write(GPIOC, val, PIN_13);
	val = !val;
}

int main(void) {

	rcc_config_clock(CLOCK_CONFIG_PERFORMANCE, &sysclks);

	if(io_configure(GPIOC, PIN_13, IO_MODE_OUTPUT | IO_OUT_PUSH_PULL, 0)) 
		return 0;
	io_write(GPIOC, 1, PIN_13);

	//timer_tick_init(TIM2, 1000, timeout_cb);
	//timer_start(TIM2);

	for(;;){
		timer_wait_ms(TIM1, 500, 0);
		timer_wait_ms(TIM2, 500, timeout_cb);
	}

	return 0;
}
