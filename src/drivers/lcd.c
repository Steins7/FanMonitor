#include "lcd.h"

enum mode {
	WRITE,
	READ,
	UNDEFINED
};

static int mode = UNDEFINED; 

//------------------------------------------------------------------------------
// internal functions
int set_mode_read(void) {
	// configure pins
	if(io_configure(GPIOA, PIN_8 | PIN_12 | PIN_15, IO_MODE_INPUT | 
				IO_IN_FLOATING, 0)) return -1;
	if(io_configure(GPIOB, PIN_3 | PIN_4 | PIN_13 | PIN_14 | PIN_15,
				IO_MODE_INPUT | IO_IN_FLOATING, 0)) return -1;

	// put lcd in read mode
	io_set(GPIOA, PIN_9); // after the pins are ready to recieve voltage
	mode = READ;

	return 0;
}

int set_mode_write(void) {
	// put lcd in write mode
	io_clear(GPIOA, PIN_9); //before the pin can send voltage

	// configure pins
	if(io_configure(GPIOA, PIN_8 | PIN_12 | PIN_15, IO_MODE_OUTPUT | 
				IO_OUT_PUSH_PULL, 0)) return -1;
	if(io_configure(GPIOB, PIN_3 | PIN_4 | PIN_13 | PIN_14 | PIN_15,
				IO_MODE_OUTPUT | IO_OUT_PUSH_PULL, 0)) return -1;
	mode = WRITE;

	return 0;
}

void wait_for_ready(void) {
	// configure the lcd
	if(mode != READ) if(set_mode_read()) return;

	// read D7 pin
	for(;;) {
		io_set(GPIOA, PIN_11);
		for(int i=0; i<1000; ++i); //timer_wait is overkill here
		if(!io_read(GPIOB, PIN_4)) break;
		io_clear(GPIOA, PIN_11);
		for(int i=0; i<1000; ++i); //same
	}
	io_clear(GPIOA, PIN_11);
}

void write_byte(uint8_t byte) {
	// put the lcd bus in write mode
	if(mode != WRITE) if(set_mode_write()) return;

	// start tranfert
	io_set(GPIOA, PIN_11);
	timer_wait_us(TIM1, 1, 0);
	
	// send the data
	io_write(GPIOA, (byte >> 0) & 0x1, PIN_8);
	io_write(GPIOA, (byte >> 1) & 0x1, PIN_12);
	io_write(GPIOB, (byte >> 2) & 0x1, PIN_15);
	io_write(GPIOA, (byte >> 3) & 0x1, PIN_15);
	io_write(GPIOB, (byte >> 4) & 0x1, PIN_14);
	io_write(GPIOB, (byte >> 5) & 0x1, PIN_3);
	io_write(GPIOB, (byte >> 6) & 0x1, PIN_13);
	io_write(GPIOB, (byte >> 7) & 0x1, PIN_4);

	// validate data
	io_clear(GPIOA, PIN_11);
}

//------------------------------------------------------------------------------
// API functions
int lcd_init(TIM_TypeDef* tim) {
	// disable JTAG, as it utilise needed pins, SWD remains usable in 
	// synchronous mode
	RCC->APB2ENR |= 0x1;
	AFIO->MAPR = (AFIO->MAPR & ~(0x8 << 24)) | 0x2 << 24;
	
	// configure the lcd control pins
	if(io_configure(GPIOA, PIN_9 | PIN_10 | PIN_11, IO_MODE_OUTPUT |
				IO_OUT_PUSH_PULL, 0)) return -1;

	// put the lcd bus in write mode
	if(set_mode_write()) return -1; //no check in case the pins were used 
									//somewhere else

	// select instrution register
	io_write(GPIOA, LCD_MODE_CMD, PIN_10);

	// begin initialisation sequence
	timer_wait_ms(tim, 15, 0);
	write_byte(LCD_FUNC_SET | LCD_FUNC_8BIT | LCD_FUNC_2LINE | 
			LCD_FUNC_5x8DOTS);
	timer_wait_ms(tim, 5, 0);
	write_byte(LCD_FUNC_SET | LCD_FUNC_8BIT | LCD_FUNC_2LINE | 
			LCD_FUNC_5x8DOTS);
	timer_wait_us(tim, 150, 0);
	write_byte(LCD_FUNC_SET | LCD_FUNC_8BIT | LCD_FUNC_2LINE | 
			LCD_FUNC_5x8DOTS);
	wait_for_ready();
	write_byte(LCD_FUNC_SET | LCD_FUNC_8BIT | LCD_FUNC_2LINE | 
			LCD_FUNC_5x8DOTS);
	wait_for_ready();
	write_byte(LCD_DISP_CTRL | LCD_CTRL_DISP_OFF | LCD_CTRL_CUR_OFF | 
			LCD_CTRL_BLINK_OFF);
	wait_for_ready();
	write_byte(LCD_CLEAR);
	wait_for_ready();
	write_byte(LCD_ENTRY | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DEC);
	wait_for_ready();

	// post initialisation setup
	write_byte(LCD_DISP_CTRL | LCD_CTRL_DISP_ON | LCD_CTRL_CUR_ON | 
			LCD_CTRL_BLINK_ON);
	wait_for_ready();
	write_byte(LCD_CLEAR);
	wait_for_ready();

	return 0;
}
