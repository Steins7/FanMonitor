#ifndef ADC_H
#define ADC_H

#include "../target/stm32f103xb.h"
#include "../config.h"
#include "rcc.h"

//------------------------------------------------------------------------------
/** adc_disc_init
 *  initialize adc in discontinuous mode
 */
int adc_disc_init(ADC_TypeDef* adc);

/** adc_disc_read
 *	use adc in discontinuous mode to read a channel
 */
uint16_t adc_disc_read(ADC_TypeDef* adc, uint8_t channel);

//------------------------------------------------------------------------------
/** adc_cont_init
 *  initialize adc in continuous mode
 */
int adc_cont_init(ADC_TypeDef* adc, uint8_t* channels, uint16_t* data, 
		uint8_t size);

/** adc_cont_start
 *  start convertion in continuous mode
 */
void adc_cont_start(ADC_TypeDef* adc);

/** adc_cont_stop
 *  stop convertion in continuous mode
 */
void adc_cont_stop(ADC_TypeDef* adc);

#endif

