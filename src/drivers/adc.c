#include "adc.h"

extern Clock_t sysclks;

//------------------------------------------------------------------------------
int adc_disc_init(ADC_TypeDef* adc) {
	// enable adc clock	
	if(adc == ADC1) RCC->APB2ENR |= 0x1 << 9;
	else if(adc == ADC2) RCC->APB2ENR |= 0x1 << 10;
	else return -1; //no such adc

	// reset config
	adc->CR1 = 0;
	adc->CR2 = 0;
	
	// enable adc
	adc->CR2 |= 0x1;

	// configure regular channels
	//TODO support watchdog
	//adc->CR1 |= 0x1 << 23; //enable analog watchdog
	adc->CR1 |= 0x1 << 11; //discontinuous mode

	// set trigger to manual
	adc->CR1 |= 0x7 << 3;

	adc->SMPR2 |= 0x7;

	// calibrate
	adc->CR2 |= 0x1 << 2;
	while((adc->CR2 >> 2) & 0x1);

	return 0;
}

uint16_t adc_disc_read(ADC_TypeDef* adc, uint8_t channel) {

	adc->SQR1 &= ~(0xF << 20); //one conversion only
	adc->SQR3 = (adc->SQR3 & ~(0x1F)) | channel; //set channel

	adc->CR2 |= 0x1;
	while(!((adc->SR >> 1) & 0x1)); //waiting for convertion
	
	return adc->DR & 0xFFF;
}

//------------------------------------------------------------------------------
int adc_cont_init(ADC_TypeDef* adc, uint8_t* channels, uint16_t* data, 
		uint8_t size) {
	// enable adc clock	
	if(adc == ADC1) RCC->APB2ENR |= 0x1 << 9;
	else if(adc == ADC2) RCC->APB2ENR |= 0x1 << 10;
	else return -1; //no such adc

	// reset config
	adc->CR1 = 0;
	adc->CR2 = 0;

	// enable adc
	adc->CR2 |= 0x1;

	// enable EOC interrupt
	//adc->CR1 |= 0x1 << 5;

	// setup DMA
	adc->CR2 |= 0x1 << 8; //enable DMA

	// set convertion channels
	if(size > 16) return -1; //to much channels
	adc->SQR1 |= (size & 0xF) << 20; //number of convertions
	for(int i=0; i<size; ++i) {
		if(i < 7) adc->SQR3 |= (channels[i] & 0x1F) << 5*i;
		else if(i < 12) adc->SQR2 |= (channels[i] & 0x1F) << 5*(i-6);
		else if(i < 16) adc->SQR1 |= (channels[i] & 0x1F) << 5*(i-13);
	}

	// calibrate
	adc->CR2 |= 0x1 << 2;
	while((adc->CR2 >> 2) & 0x1);

	return 0;
}

void adc_cont_start(ADC_TypeDef* adc) {
	//start convertion in continuous mode
	adc->CR2 |= 0x1 << 1;
}

void adc_cont_stop(ADC_TypeDef* adc) {
	//stop convertion in continuous mode
	adc->CR2 &= ~(0x1 << 1);
}

