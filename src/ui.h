#ifndef UI_H
#define UI_H

// drivers headers
#include "drivers/lcd.h"

// standard headers
#include <stdint.h>

// project headers
#include "utils.h"

/** temp
 *  list of all the temperature ids on the sreen
 */
enum temp {
	T_EXT	= 0,
	T1		= 1,
	T2		= 2		
};

/** temp_pos
 *  coords of the temprature ids
 */
static uint8_t temp_pos[][2] = {
	{11, 0},
	{ 2, 1},
	{11, 1}};

/**	update_temp
 * 	update on the lcd the given value for the corresponding id
 */
void update_temp(uint8_t id, int16_t t_ext);


#endif

