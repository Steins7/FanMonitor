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
/* static variables */;
int val = 0; //debug led
int read_flag = 0; 
int sensor_id = 0;

// low_pass filter
#define FILTER_LENGTH 10
int16_t temp_filter[3][FILTER_LENGTH] = {};

// adc channels
uint8_t channels[3][2] = {
	{4, 5},
	{0, 1},
	{2, 3}
};

vars_t vars = {
	{},
	5,
	2,
	{},
	1,
	0
};

//------------------------------------------------------------------------------
/* Timer IRQ */
static void timeout_cb(void) {
	io_write(GPIOC, val, PIN_13);
	val = !val;

	// set temp read flag
	read_flag = 1;
	}

int16_t read_temp(uint8_t id) {
	// get ADC values
	uint32_t data = 0;	
	data = adc_read(ADC1, channels[id][0]);
	data -= adc_read(ADC1, channels[id][1]);

	int16_t voltage = ((data*4) << 8)/4095 ;
	
	return ((voltage - 0x73) << 8)/0x9 + vars.facs[id][0];
}

//------------------------------------------------------------------------------
/* main function */
int main(void) {

	//initialize variables
	val = 0; //debug led
	read_flag = 0; 
	sensor_id = 0;

	// configure clocks (necessary before using timers)
	rcc_config_clock(CLOCK_CONFIG_PERFORMANCE, &sysclks);

	// configure GPIO for LED
	if(io_configure(GPIOC, PIN_13, IO_MODE_OUTPUT | IO_OUT_PUSH_PULL, 0)) 
		return -1;
	io_write(GPIOC, 1, PIN_13);

	// configure GPIO for relay
	if(io_configure(GPIOB, PIN_12, IO_MODE_OUTPUT | IO_OUT_PUSH_PULL, 0)) 
		return -1;
	io_write(GPIOB, 1, PIN_12);

	// configure GPIOS for temperature sensors
	if(io_configure(GPIOA, PIN_0 | PIN_1 | PIN_2 | PIN_3 | PIN_4 | PIN_5, 
		IO_MODE_INPUT | IO_IN_ANALOG, 0)) return -1;
	if(adc_init(ADC1)) return -1;

	// initilize filter
	for(int i=0; i<3; ++i) {
		int16_t tmp = read_temp(i);
		for(int j=0; j<FILTER_LENGTH; ++j) {
			temp_filter[i][j] = tmp;
		}
	}

	// setup ui
	if(ui_init(TIM4, TIM3, &vars)) return -1;
	
	// start timed interruption
	timer_tick_init(TIM2, 1000, timeout_cb);
	timer_start(TIM2);

	// main loop
	while(1){

		// process sensor values
		if(read_flag) {

			// clear flag
			read_flag = 0;

			// shift filter queue
			int32_t sum = 0;
			for(int i=0; i<(FILTER_LENGTH-2); i++) {
				temp_filter[sensor_id][i] = temp_filter[sensor_id][i+1];
				sum += temp_filter[sensor_id][i];
			}

			// calculate temp value
			temp_filter[sensor_id][FILTER_LENGTH-1] = read_temp(sensor_id);

			// apply filter
			sum += temp_filter[sensor_id][FILTER_LENGTH-1];
			vars.temps[sensor_id] = (sum/FILTER_LENGTH) >> 8;

			// print new value
			ui_update_temp(sensor_id);

			// switch to next sensor
			sensor_id++;
			sensor_id = sensor_id%3;

			// set fan state
			if((vars.temps[vars.silo] - vars.temps[T_EXT]) >= 
					vars.start_treshold) {
				vars.fan = 1;
				io_set(GPIOB, PIN_12);
			} else if((vars.temps[vars.silo] - vars.temps[T_EXT]) <= 
					vars.stop_treshold) {
				vars.fan = 0;
				io_clear(GPIOB, PIN_12);
			}
		}

		// update every 0.2 seconds
		ui_update();
		//timer_wait_ms(TIM1, 200, 0);
	}

	return 0;
}
