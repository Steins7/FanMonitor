//target header
#include "../target/stm32f103xb.h"

//custom header
#include "../config.h"

//std headers
#include <stdlib.h>

//driver header
#include "io.h"

static OnIO io_cb[16]={
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

void EXTI0_IRQHandler() {
	if (io_cb[0]) (io_cb[0])();

	EXTI->PR = 1<<0;
}

void EXTI1_IRQHandler() {
	if (io_cb[1]) (io_cb[1])();

	EXTI->PR = 1<<1;
}

void EXTI2_IRQHandler() {
	if (io_cb[2]) (io_cb[2])();

	EXTI->PR = 1<<2;
}

void EXTI3_IRQHandler() {
	if (io_cb[3]) (io_cb[3])();

	EXTI->PR = 1<<3;
}

void EXTI4_IRQHandler() {
	if (io_cb[4]) (io_cb[4])();

	EXTI->PR = 1<<4;
}

void EXTI9_5_IRQHandler() {
	if (EXTI->PR & (1<<5)) {
		if (io_cb[5]) (io_cb[5])();
		EXTI->PR = 1<<5;
	} else if (EXTI->PR & (1<<6)) {
		if (io_cb[6]) (io_cb[6])();
		EXTI->PR = 1<<6;
	} else if (EXTI->PR & (1<<7)) {
		if (io_cb[7]) (io_cb[7])();
		EXTI->PR = 1<<7;
	} else if (EXTI->PR & (1<<8)) {
		if (io_cb[8]) (io_cb[8])();
		EXTI->PR = 1<<8;
	} else if (EXTI->PR & (1<<9)) {
		if (io_cb[9]) (io_cb[9])();
		EXTI->PR = 1<<9;
	}
}

void EXTI15_10_IRQHandler() {
	if (EXTI->PR & (1<<10)) {
		if (io_cb[10]) (io_cb[10])();
		EXTI->PR = 1<<10;
	} else if (EXTI->PR & (1<<11)) {
		if (io_cb[11]) (io_cb[11])();
		EXTI->PR = 1<<11;
	} else if (EXTI->PR & (1<<12)) {
		if (io_cb[12]) (io_cb[12])();
		EXTI->PR = 1<<12;
	} else if (EXTI->PR & (1<<13)) {
		if (io_cb[13]) (io_cb[13])();
		EXTI->PR = 1<<13;
	} else if (EXTI->PR & (1<<14)) {
		if (io_cb[14]) (io_cb[14])();
		EXTI->PR = 1<<14;
	} else if (EXTI->PR & (1<<15)) {
		if (io_cb[15]) (io_cb[15])();
		EXTI->PR = 1<<15;
	}
}

/* Definitions for EXTI configuration */
#define SYSCFG_EXTI_PA_MASK		0
#define SYSCFG_EXTI_PB_MASK		1
#define SYSCFG_EXTI_PC_MASK		2
#define SYSCFG_EXTI_PD_MASK		3
#define SYSCFG_EXTI_PE_MASK		4
#define SYSCFG_EXTI_PH_MASK		7


int io_configure(GPIO_TypeDef *gpio, uint16_t pin, uint8_t pin_cfg, OnIO cb) {

	// enable GPIOx subsystem clocking
	if (gpio == GPIOA) RCC->APB2ENR |= 1<<2;
	else if (gpio == GPIOB) RCC->APB2ENR |= 1<<3;
	else if (gpio == GPIOC) RCC->APB2ENR |= 1<<4;
	else if (gpio == GPIOD) RCC->APB2ENR |= 1<<5;
	else if (gpio == GPIOE) RCC->APB2ENR |= 1<<6;

	// setup pin mask for crx registers
	uint64_t pin_mask = 0;
	for(int i=0; i<16; ++i) {
		if((pin >> i) & 0x1) {
			pin_mask |= 0x1ll << 4*i;
		}
	}

	// clear previous data
	uint64_t crx = pin_mask * 0x4; //reset value is 0x4
	uint16_t odr = pin;
	gpio->CRH &= ~(crx >> 32);
	gpio->CRL &= ~(crx & 0xFFFF);
	gpio->BSRR |= odr << 16;

	// set up the new configuration
	crx = pin_mask * (pin_cfg & 0xF);
	odr = pin_cfg & 0x10;
	gpio->CRH |= crx >> 32;
	gpio->CRL |= crx & 0xFFFF;
	gpio->BSRR |= odr;

	//if (!cb) return -1; //no callback attached

	return 0;

	//TODO manage alternate functions
//	if (pin_cfg & 0x3) return -1; //callback set, but not in input mode
//
//	// ************* Input GPIO + External IRQ *************
//
//	uint32_t port_mask = 0;
//	uint32_t pin_mask = 0;
//
//	if (gpio == GPIOA) port_mask = SYSCFG_EXTI_PA_MASK;
//	else if (gpio == GPIOB) port_mask = SYSCFG_EXTI_PB_MASK;
//	else if (gpio == GPIOC) port_mask = SYSCFG_EXTI_PC_MASK;
//	else if (gpio == GPIOD) port_mask = SYSCFG_EXTI_PD_MASK;
//	else if (gpio == GPIOE) port_mask = SYSCFG_EXTI_PE_MASK;
//
//	uint32_t bit_mask = 0x1;
//
//	for (int i=0; i<16; i++) {
//	if (pin_mask & bit_mask) {
//	// enable clock for SYSCFG, no need for EXTI (interface not clocked)
//	RCC->APB2ENR = RCC->APB2ENR | (1<<14);
//
//	// configure pin Px_i (4 pin config per EXTICR[] register, 4 bits per pin)
//	//   use port Px and bind Px_i --> EXTIi
//	// i>>2 = i/4 ; i & 0x3 = i%4
//	gpio->EXTICR[i>>2] = (SYSCFG->EXTICR[i>>2] & 
//	~(0x0000000F << ((i & 3)<<2))) | 
//	(port_mask << ((i & 3)<<2));
//
//	// allow pin EXTIi to send an IRQ
//	EXTI->IMR = EXTI->IMR | bit_mask;
//	//   not a wakeup event
//	EXTI->EMR = EXTI->EMR & (~bit_mask);
//
//	// Configure pin event IRQ on rising (RTSR)/falling (FTSR) edge (rising only here)
//	if (pin_cfg & PIN_OPT_IRQ_EDGE_RISE) {
//	EXTI->RTSR = EXTI->RTSR | bit_mask;
//	} else {
//	EXTI->RTSR = EXTI->RTSR & (~bit_mask);
//	}
//
//	if (pin_cfg & PIN_OPT_IRQ_EDGE_FALL) {
//	EXTI->FTSR = EXTI->FTSR | bit_mask;
//	} else {
//	EXTI->FTSR = EXTI->FTSR & (~bit_mask);
//	}
//
//	io_cb[i] = cb;
//
//	// reset any pending IRQ on PC13
//	EXTI->PR = bit_mask;
//
//	// Setup NVIC
//	switch (i) {
//	case 0:
//	NVIC_SetPriority(EXTI0_IRQn, EXTI0_IRQ_PRIORITY);
//	NVIC_EnableIRQ(EXTI0_IRQn);
//	break;
//	case 1:
//	NVIC_SetPriority(EXTI1_IRQn, EXTI1_IRQ_PRIORITY);
//	NVIC_EnableIRQ(EXTI1_IRQn);
//	break;
//	case 2:
//	NVIC_SetPriority(EXTI2_IRQn, EXTI2_IRQ_PRIORITY);
//	NVIC_EnableIRQ(EXTI2_IRQn);
//	break;
//	case 3:
//	NVIC_SetPriority(EXTI3_IRQn, EXTI3_IRQ_PRIORITY);
//	NVIC_EnableIRQ(EXTI3_IRQn);
//	break;
//	case 4:
//	NVIC_SetPriority(EXTI4_IRQn, EXTI4_IRQ_PRIORITY);
//	NVIC_EnableIRQ(EXTI4_IRQn);
//	break;
//	case 5:
//	case 6:
//	case 7:
//	case 8:
//	case 9:
//	NVIC_SetPriority(EXTI9_5_IRQn, EXTI9_5_IRQ_PRIORITY);
//	NVIC_EnableIRQ(EXTI9_5_IRQn);
//	break;
//	case 10:
//	case 11:
//	case 12:
//	case 13:
//	case 14:
//	case 15:
//	NVIC_SetPriority(EXTI15_10_IRQn, EXTI15_10_IRQ_PRIORITY);
//	NVIC_EnableIRQ(EXTI15_10_IRQn);
//	break;
//	default:
//	return 0;
//}
//}
//bit_mask = bit_mask<<1;
//}
}


uint32_t io_read(GPIO_TypeDef *gpio, uint16_t mask)
{	
	return gpio->IDR & mask;	
}

void io_write(GPIO_TypeDef *gpio, uint16_t val, uint16_t mask)
{
	gpio->BSRR = (uint32_t)(mask) << (val ? 0 : 16);  
}

void io_write_n(GPIO_TypeDef *gpio, uint16_t val, uint16_t mask)
{
	gpio->BSRR = (uint32_t)(mask) << (val ? 16 : 0);  
}

void io_set(GPIO_TypeDef *gpio, uint16_t mask)
{
	gpio->BSRR = mask;  
}

void io_clear(GPIO_TypeDef *gpio, uint16_t mask)
{
	gpio->BSRR = (uint32_t)(mask) << 16;  
}
