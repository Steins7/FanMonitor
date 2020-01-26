#include "dma.h"

//------------------------------------------------------------------------------
void DMA1_Channel1_IRQHandler(void) {
	DMA1->IFCR |= 0x1 << 3;
}

void DMA1_Channel2_IRQHandler(void) {
	DMA1->IFCR |= 0x1 << 7;
}

void DMA1_Channel3_IRQHandler(void) {
	DMA1->IFCR |= 0x1 << 11;
}

void DMA1_Channel4_IRQHandler(void) {
	DMA1->IFCR |= 0x1 << 15;
}

void DMA1_Channel5_IRQHandler(void) {
	DMA1->IFCR |= 0x1 << 19;
}

void DMA1_Channel6_IRQHandler(void) {
	DMA1->IFCR |= 0x1 << 23;
}

void DMA1_Channel7_IRQHandler(void) {
	DMA1->IFCR |= 0x1 << 27;
}


int dma_configure(DMA_Channel_TypeDef* dma_channel,  uint16_t config, 
		uint16_t number, uint32_t peripheral, uint32_t memory, OnTransfert cb) {	
	// configure data path
	dma_channel->CNDTR |= number;
	dma_channel->CPAR |= peripheral;
	dma_channel->CMAR |= memory;
	
	// apply options
	config |= 0x9; 	//activate error interrupt and enable DMA channel
	dma_channel->CCR |= (uint32_t)config;

	return 0;
}
