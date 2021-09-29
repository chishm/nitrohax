/*
    NitroHax -- Cheat tool for the Nintendo DS
    Copyright (C) 2008  Michael "Chishm" Chisholm

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _COMMON_H
#define _COMMON_H

#include <nds/dma.h>
#include <nds/ipc.h>
#include <stdlib.h>

#define resetCpu() \
		__asm volatile("swi 0x000000")

enum ERROR_CODES {
	ERR_NONE = 0x00,
	ERR_STS_CLR_MEM = 0x01,
	ERR_STS_LOAD_BIN = 0x02,
	ERR_STS_HOOK_BIN = 0x03,
	ERR_STS_START = 0x04,
	// initCard error codes:
	ERR_LOAD_NORM = 0x11,
	ERR_LOAD_OTHR = 0x12,
	ERR_SEC_NORM = 0x13,
	ERR_SEC_OTHR = 0x14,
	ERR_LOGO_CRC = 0x15,
	ERR_HEAD_CRC = 0x16,
	// hookARM7Binary error codes:
	ERR_NOCHEAT = 0x21,
	ERR_HOOK = 0x22,
};

// Values fixed so they can be shared with ASM code
enum ARM9_STATE {
	ARM9_BOOT = 0,
	ARM9_START = 1,
	ARM9_RESET = 2,
	ARM9_READY = 3,
	ARM9_MEMCLR = 4
};

enum ARM7_STATE {
	ARM7_BOOT = 0,
	ARM7_START = 1,
	ARM7_RESET = 2,
	ARM7_READY = 3,
	ARM7_MEMCLR = 4,
	ARM7_LOADBIN = 5,
	ARM7_HOOKBIN = 6,
	ARM7_BOOTBIN = 7,
	ARM7_ERR = 8
};

extern volatile u32 arm9_errorCode;

static inline void dmaFill(const void* src, void* dest, uint32 size) {
	DMA_SRC(3)  = (uint32)src;
	DMA_DEST(3) = (uint32)dest;
	DMA_CR(3)   = DMA_COPY_WORDS | DMA_SRC_FIX | (size>>2);
	while(DMA_CR(3) & DMA_BUSY);
}

static inline void copyLoop (u32* dest, const u32* src, size_t size) {
	do {
		*dest++ = *src++;
	} while (size -= 4);
}

static inline void ipcSendState(uint8_t state) {
	REG_IPC_SYNC = (state & 0x0f) << 8;
}

static inline uint8_t ipcRecvState(void) {
	return (uint8_t)(REG_IPC_SYNC & 0x0f);
}

#endif // _COMMON_H

