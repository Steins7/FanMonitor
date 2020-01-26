#include "drivers/rcc.h"
#include "drivers/io.h"
#include "drivers/adc.h"
#include "drivers/lcd.h"

Clock_t sysclks;

#include "drivers/timer.h"

int val = 0;
uint16_t data = 0;
float voltage = 0;
float temp = 0;

static void timeout_cb(void) {
	io_write(GPIOC, val, PIN_13);
	io_write(GPIOB, val, PIN_12);
	val = !val;
}

int main(void) {

	rcc_config_clock(CLOCK_CONFIG_POWERSAVE, &sysclks);

	lcd_init(TIM1);

	// blink GPIOi (and relay)
	if(io_configure(GPIOC, PIN_13, IO_MODE_OUTPUT | IO_OUT_PUSH_PULL, 0)) 
		return 0;
	if(io_configure(GPIOB, PIN_12, IO_MODE_OUTPUT | IO_OUT_PUSH_PULL, 0)) 
		return 0;
	io_write(GPIOC, 1, PIN_13 | PIN_12);

	// blink timer
	timer_tick_init(TIM2, 1000, timeout_cb);
	timer_start(TIM2);

	// ADC GPIOs
	if(io_configure(GPIOA, PIN_2 | PIN_3, IO_MODE_INPUT | IO_IN_ANALOG, 0))
		return 0;
	if(adc_disc_init(ADC1)) return 0;

	for(;;){
		data = adc_disc_read(ADC1, 2);
		data -= adc_disc_read(ADC1, 3);
		voltage = data*4.0f/4095.0f;
		temp = (voltage-0.45)/0.04f;
	}

	return 0;
}

