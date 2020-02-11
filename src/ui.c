#include "ui.h"

//------------------------------------------------------------------------------
/* control variables */
static int state;
static volatile int button_update;
static int sel, prev_sel;
static TIM_TypeDef* enc;
static TIM_TypeDef* tim;
static vars_t* vars;

/* data variables */
static const char* ui_default[] = {
	"SILO:   Te:   ""\xDF""C",
	"1:   ""\xDF""C  2:   ""\xDF""C"
};
static const char* ui_menu[] = {
	" 1-Retour",
	" 2-Silo:",
	" 3-Start:",
	" 4-Stop:",
	" 5-Calibrer Te",
	" 6-Calibrer T1",
	" 7-Calibrer T2",
	" 8-Test ventil",
	" 1-Retour"
};

//------------------------------------------------------------------------------
/* internal functions */
void show_bg(const char** bg) {
	lcd_send_cmd(LCD_CLEAR);
	lcd_send_cmd(LCD_CUR_HOME);
	lcd_print(bg[0]);
	lcd_set_cursor(0,1);
	lcd_print(bg[1]);
}

//------------------------------------------------------------------------------
/* timeout timer cb */
static void ui_sleep(void) {
	state = UI_SLEEP;
}

/* encoder button gpio cb */
static void ui_button_cb(void) {
	for(int i=0; i<10000; ++i); //avoid double trigger
	timer_start(tim);
	button_update = 1;
}

//------------------------------------------------------------------------------
/* main functions */
int ui_init(TIM_TypeDef* encoder, TIM_TypeDef* timeout, vars_t* vars_p) {
	// save parameters
	enc = encoder;
	tim = timeout;
	vars = vars_p;

	// configure lcd
	lcd_init(TIM1, 16, 2);
	lcd_send_cmd(LCD_CUR_HOME);
	lcd_print(ui_default[0]);
	lcd_set_cursor(0, 1);
	lcd_print(ui_default[1]);
	lcd_send_cmd(LCD_DISP_CTRL | LCD_CTRL_DISP_ON | LCD_CTRL_CUR_OFF | 
			LCD_CTRL_BLINK_OFF);

	// configure encoder
	io_configure(GPIOB, PIN_6 | PIN_7, IO_MODE_INPUT | IO_IN_FLOATING, 0);
	timer_enc_init(enc); //TODO fix weird behaviour around 0

	io_configure(GPIOB, PIN_8, IO_MODE_INPUT | IO_IN_PULL_UP | 
			IO_IRQ_EDGE_FALL, ui_button_cb);

	// setup state variables
	state = UI_SLEEP;
	sel = 0;
	prev_sel = -1;

	return 0;
}

void ui_update_entry(uint8_t prev_entry, uint8_t value) {
	// get vlaue in str form
	char str[8]; //too long to be sure
	num2str(str, value, 10);

	// print value in the correct position
	if(sel == prev_entry) lcd_set_cursor(14, 1);
	else lcd_set_cursor(14, 0);
	lcd_print(str);

}

void ui_change_value(uint8_t entry, uint8_t* entry_val, int max) {
	int local_val = *entry_val;
	lcd_set_cursor(14,0);
	while(!button_update) {
		local_val = ((enc->CNT/2)%max) + 1;
		char str[8];
		num2str(str, local_val, 10);
		lcd_print(str);
		lcd_set_cursor(14,0);
		timer_wait_ms(TIM1, 200, 0);
	}
	*entry_val = local_val;
}
	


void ui_update(void) {
	// manage button press
	if(button_update) {
		button_update = 0;

		switch(state) {
			//-------------------------
			case UI_DEFAULT:
				// reset encoder and previous values
				timer_start(enc);
				sel = 0;
				prev_sel = -1;

				// go to menu screen
				state = UI_MENU;
				break;
			//-------------------------
			case UI_MENU:
				// go back to default screen whith return
				if(sel == UI_RETURN) {
					show_bg(ui_default);
					lcd_set_cursor(5,0);
					char str[4];
					num2str(str, vars->silo, 10);
					lcd_print(str);

					state = UI_DEFAULT;
					for(int i=0; i<3; ++i) ui_update_temp(i);
					break;
				}

				// otherwise interact with option
				lcd_send_cmd(LCD_DISP_CTRL | LCD_CTRL_DISP_ON | 
							LCD_CTRL_CUR_ON | LCD_CTRL_BLINK_OFF);
				timer_start(enc);
				state = UI_INTERACT;
				break;
			//-------------------------
			case UI_INTERACT:
				state = UI_MENU;
				lcd_send_cmd(LCD_DISP_CTRL | LCD_CTRL_DISP_ON | 
							LCD_CTRL_CUR_OFF | LCD_CTRL_BLINK_OFF);
				break;
			//-------------------------
			case UI_SLEEP:
				// re-enable screen
				lcd_send_cmd(LCD_DISP_CTRL | LCD_CTRL_DISP_ON | LCD_CTRL_CUR_OFF
					| LCD_CTRL_BLINK_OFF);
	

				// re-enable timeout system
				timer_wait_ms(tim, TIMEOUT_LENGHT, ui_sleep);

				// go back to default screen
				show_bg(ui_default);
				lcd_set_cursor(5,0);
				char str[4];
				num2str(str, vars->silo, 10);
				lcd_print(str);

				state = UI_DEFAULT;
				for(int i=0; i<3; ++i) ui_update_temp(i);
				break;
		}
	}

	// manage dynamic draw
	switch(state) {
		//---------------------
		case UI_MENU:
			// get current encoder value
			sel = (enc->CNT/2)%(MENU_LENGHT+1);

			// refresh menu if selected item changed
			if(sel != prev_sel) {
				prev_sel = sel;
				timer_start(tim); //refresh timeout timer
				show_bg(ui_menu + sel);
				lcd_send_cmd(LCD_CUR_HOME);			
				lcd_print_c(0x7E);

				// show silo value if needed
				if(sel == UI_RETURN || sel == UI_SILO) {
					ui_update_entry(UI_RETURN, vars->silo);
				}
				// show start value if needed
				if(sel == UI_SILO || sel == UI_START) {
					ui_update_entry(UI_SILO, vars->start_treshold);
				}
				// show start value if needed
				if(sel == UI_START || sel == UI_STOP) {
					ui_update_entry(UI_START, vars->stop_treshold);
				}
			}
			break;
		//---------------------
		case UI_INTERACT:
			switch(sel) {
				case UI_SILO:
					ui_change_value(UI_SILO, &vars->silo, 2);
					break;
				case UI_START:
					ui_change_value(UI_START, &vars->start_treshold, 9);
					break;
				case UI_STOP:
					ui_change_value(UI_STOP, &vars->stop_treshold, 9);
					break;


			}
		//---------------------
		case UI_DEFAULT:
			break;
		//--------------------
		case UI_SLEEP:
			//TODO implement backlight off
			lcd_send_cmd(LCD_DISP_CTRL | LCD_CTRL_DISP_OFF | LCD_CTRL_CUR_OFF
					| LCD_CTRL_BLINK_OFF);
	}
}

static uint8_t temp_pos[][2] = {
	{11, 0},
	{ 2, 1},
	{11, 1}};

void ui_update_temp(uint8_t id) {
	if(state != UI_DEFAULT) return;

	if(id > 2) return; //protect from overflow
	uint8_t* pos = temp_pos[id];

	// prepare data
	char str[16]; //longer, in case of error
	
	// convert int into str
	uint32_t nb = num2str(str, (vars->temps[id]), 10);

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

