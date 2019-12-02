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

	if(io_configure(GPIOC, PIN_13, PIN_MODE_OUTPUT | PIN_OPT_OUTPUT_PUSHPULL, 0)) return 0;
	io_write(GPIOC, 1, PIN_13);

	timer_tick_init(TIM2, 1000, timeout_cb);
	timer_start(TIM2);

	for(;;){
	}

	return 0;
}
