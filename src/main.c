#include "drivers/rcc.h"
#include "drivers/io.h"
#include "drivers/lcd.h"
#include "drivers/adc.h"

Clock_t sysclks;
#include "drivers/timer.h"

int val = 0;
uint16_t data = 0;

static void timeout_cb(void) {
	io_write(GPIOC, val, PIN_13);
	val = !val;
	data = adc_read(ADC1, 2);
	data -= adc_read(ADC1, 3);
}

int main(void) {

	rcc_config_clock(CLOCK_CONFIG_PERFORMANCE, &sysclks);

	if(io_configure(GPIOC, PIN_13, IO_MODE_OUTPUT | IO_OUT_PUSH_PULL, 0)) 
		return 0;
	io_write(GPIOC, 1, PIN_13);

	if(io_configure(GPIOA, PIN_2 | PIN_3, IO_MODE_INPUT | IO_IN_ANALOG, 0))
		return 0;
	if(adc_init(ADC1)) return 0;

	//int test = 0;

	timer_tick_init(TIM2, 1000, timeout_cb);
	timer_start(TIM2);

	lcd_init(TIM1);
	lcd_print(" et paf");
	lcd_print("Hello world!");
	lcd_send_cmd(LCD_CUR_HOME);
	lcd_send_cmd(LCD_CUR_HOME);
	lcd_set_cursor(5, 1);


	while(1){
		lcd_print("loop");
		lcd_print("loop2");
		lcd_print("loop3");
		lcd_print("loop4");
		//voltage = data*4.0f/4095.0f;
		//temp = (voltage-0.45)/0.04f;
		timer_wait_ms(TIM1, 200, 0);
	}

	return 0;
}
