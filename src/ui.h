#ifndef UI_H
#define UI_H

// drivers headers
#include "drivers/lcd.h"
#include "drivers/timer.h"

// standard headers
#include <stdint.h>

// project headers
#include "utils.h"

//------------------------------------------------------------------------------

#define TIMEOUT_LENGHT 20000

/** vars_t
 *  structure that contains all the variables needed by the ui and used or
 *  created elsewhere
 */
typedef struct {
	int16_t temps[3];	
	uint8_t start_treshold;
	uint8_t stop_treshold;
	uint8_t facs[3][2];
	uint8_t silo;		
	uint8_t fan;	
} vars_t;

// ui enums, created to improve readability
enum temp {
	T_EXT	= 0,
	T1		= 1,
	T2		= 2		
};

enum ui_state {
	UI_DEFAULT,
	UI_MENU,
	UI_INTERACT,
	UI_SLEEP
};

enum ui_menu {	
	UI_RETURN 		= 0,
	UI_SILO			= 1,
	UI_START		= 2,
	UI_STOP			= 3,
	UI_CAL_T_EXT 	= 4,
	UI_CAL_T1		= 5,	
	UI_CAL_T2		= 6,
	UI_TEST			= 7
};

#define MENU_LENGHT 7

//------------------------------------------------------------------------------
/** ui_init
 *  init ui, using a timer for the encoder and another for the timeout delay
 */
int ui_init(TIM_TypeDef* encoder, TIM_TypeDef* timeout, vars_t* vars_p);

/** ui_update
 *  update the graphical elements and manages the button press
 *  should be called quite often for a smooth usage
 */
void ui_update(void);

/**	update_temp
 * 	update on the lcd the given value for the corresponding id
 */
void ui_update_temp(uint8_t id);

#endif

