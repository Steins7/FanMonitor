#ifndef DMA_H
#define DMA_H

#include "../target/stm32f103xb.h"
#include "../config.h"
#include "rcc.h"

typedef void (*OnTransfert)(void);

enum dma_config {
	DMA_MEM2MEM		= (0x1 << 14),
	DMA_PL_LOW		= (0x0 << 12),
	DMA_PL_MED		= (0x1 << 12),
	DMA_PL_HIGH		= (0x2 << 12),
	DMA_PL_VHIGH	= (0x3 << 12),
	DMA_MSIZE_8		= (0x0 << 10),
	DMA_MSIZE_16	= (0x1 << 10),
	DMA_MSIZE_32	= (0x2 << 10),
	DMA_PSIZE_8		= (0x0 << 8),
	DMA_PSIZE_16	= (0x1 << 8),
	DMA_PSIZE_32	= (0x2 << 8),
	DMA_MEM_INC		= (0x1 << 7),
	DMA_PER_INC		= (0x1 << 6),
	DMA_CIRC		= (0x1 << 5),
	DMA_FROM_P		= (0x0 << 4),
	DMA_FROM_M		= (0x1 << 4),
	DMA_ERR_IRQ		= (0x1 << 3),
	DMA_HALF_IRQ	= (0x1 << 2),
	DMA_FULL_IRQ	= (0x1 << 1)
};

int dma_configure(DMA_Channel_TypeDef* dma_channel, uint16_t config, 
		uint16_t number, uint32_t peripheral, uint32_t memory, OnTransfert cb);

#endif

