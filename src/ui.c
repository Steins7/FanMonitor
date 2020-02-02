#include "ui.h"

static uint8_t temp_pos[][2] = {
	{11, 0},
	{ 2, 1},
	{11, 1}};

void update_temp(uint8_t id, int16_t t) {
	
	if(id > 2) return; //protect from overflow
	uint8_t* pos = temp_pos[id];

	// prepare data
	char str[16]; //longer, in case of error
	//t = t >> 8;
	
	// convert int into str
	uint32_t nb = num2str(str, t, 10);

	// clear previous text
	lcd_set_cursor(pos[0],pos[1]);
	lcd_print("   ");

	// prepare lcd for write
	switch(nb) {
		case 1:
			lcd_set_cursor(pos[0]+2,pos[1]);
			break;
		case 2:
			lcd_set_cursor(pos[0]+1,pos[1]);
			break;
		case 3:
			lcd_set_cursor(pos[0],pos[1]);
			break;
		default: // something went wrong
			lcd_set_cursor(pos[0],pos[1]);
			lcd_print("Err");
			return;
	}

	// write value
	lcd_print(str);
}
