// standard headers
#include <stdint.h>

// driver includes
#include "drivers/rcc.h"
#include "drivers/io.h"
#include "drivers/lcd.h"
#include "drivers/adc.h"

Clock_t sysclks;
#include "drivers/timer.h"

// project headers
#include "ui.h"

//------------------------------------------------------------------------------
/* static variables */
int val = 0;
uint16_t data = 0;
int16_t voltage = 0;
int16_t temp = 0;

//------------------------------------------------------------------------------
/* Timer IRQ */
static void timeout_cb(void) {
	io_write(GPIOC, val, PIN_13);
	val = !val;
}

//------------------------------------------------------------------------------
/* main function */
int main(void) {

	rcc_config_clock(CLOCK_CONFIG_PERFORMANCE, &sysclks);

	// configure GPIO for LED
	if(io_configure(GPIOC, PIN_13, IO_MODE_OUTPUT | IO_OUT_PUSH_PULL, 0)) 
		return 0;
	io_write(GPIOC, 1, PIN_13);

	// configure GPIOS for temperature sensors
	if(io_configure(GPIOA, PIN_0 | PIN_1 | PIN_2 | PIN_3 | PIN_4 | PIN_5, 
		IO_MODE_INPUT | IO_IN_ANALOG, 0)) return 0;
	if(adc_init(ADC1)) return 0;

	// configure lcd
	lcd_init(TIM1, 16, 2);
	lcd_send_cmd(LCD_CUR_HOME);
	lcd_print("SILO:1  Te: 18""\xDF""C");
	lcd_set_cursor(0,1);
	lcd_print("1: 25""\xDF""C  2: 26""\xDF""C");
	lcd_send_cmd(LCD_DISP_CTRL | LCD_CTRL_DISP_ON | LCD_CTRL_CUR_OFF | 
			LCD_CTRL_BLINK_OFF);

	// start timed interruption
	timer_tick_init(TIM2, 1000, timeout_cb);
	timer_start(TIM2);

	// main loop
	while(1){
		// update T1
		data = adc_read(ADC1, 0);
		data -= adc_read(ADC1, 1);

		voltage = ((data*4) << 8)/4095;
		temp = ((voltage - 0x73) << 8)/0x9;

		update_temp(T1, temp);

		// update T2
		data = adc_read(ADC1, 2);
		data -= adc_read(ADC1, 3);

		voltage = ((data*4) << 8)/4095;
		temp = ((voltage - 0x73) << 8)/0x9;

		update_temp(T2, temp);

		// update T_ext
		data = adc_read(ADC1, 4);
		data -= adc_read(ADC1, 5);

		voltage = ((data*4) << 8)/4095;
		temp = ((voltage - 0x73) << 8)/0x9;

		update_temp(T_EXT, temp);

		// update every 0.2 seconds
		timer_wait_ms(TIM1, 200, 0);
	}

	return 0;
}
