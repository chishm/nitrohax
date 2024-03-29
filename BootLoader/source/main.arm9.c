/*
 main.arm9.c

 By Michael Chisholm (Chishm)

 All resetMemory and startBinary functions are based
 on the MultiNDS loader by Darkain.
 Original source available at:
 http://cvs.sourceforge.net/viewcvs.py/ndslib/ndslib/examples/loader/boot/main.cpp

 License:
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

#define ARM9
#undef ARM7
#include <nds/memory.h>
#include <nds/arm9/video.h>
#include <nds/arm9/input.h>
#include <nds/interrupts.h>
#include <nds/dma.h>
#include <nds/timers.h>
#include <nds/system.h>
#include <nds/ipc.h>

#include "common.h"

volatile int arm9_stateFlag = ARM9_BOOT;
volatile u32 arm9_errorCode = 0xFFFFFFFF;
volatile bool arm9_errorClearBG = false;
volatile u32 arm9_BLANK_RAM = 0;

/*-------------------------------------------------------------------------
External functions
--------------------------------------------------------------------------*/
extern void arm9_clearCache (void);
extern void arm9_reset (void);

/*-------------------------------------------------------------------------
arm9_errorOutput
Displays an error code on screen.

Each box is 2 bits, and left-to-right is most-significant bits to least.
Red = 00, Yellow = 01, Green = 10, Blue = 11

Written by Chishm
--------------------------------------------------------------------------*/
static void arm9_errorOutput (u32 code) {
	int i, j, k;
	u16 colour;
	vu16 *const vram_a = (vu16*)VRAM_A;

	REG_POWERCNT = (u16)(POWER_LCD | POWER_2D_A);
	REG_DISPCNT = MODE_FB0;
	VRAM_A_CR = VRAM_ENABLE;

	// Clear display
	for (i = 0; i < 256*192; i++) {
		vram_a[i] = 0x0000;
	}

	// Draw boxes of colour, signifying error codes

	if ((code >> 16) != 0) {
		// high 16 bits
		for (i = 0; i < 8; i++) {						// Pair of bits to use
			if (((code>>(30-2*i))&3) == 0) {
				colour = 0x001F; // Red
			} else if (((code>>(30-2*i))&3) == 1) {
				colour = 0x03FF; // Yellow
			} else if (((code>>(30-2*i))&3) == 2) {
				colour = 0x03E0; // Green
			} else {
				colour = 0x7C00; // Blue
			}
			for (j = 71; j < 87; j++) { 				// Row
				for (k = 32*i+8; k < 32*i+24; k++) {	// Column
					vram_a[j*256+k] = colour;
				}
			}
		}
	}

	// Low 16 bits
	for (i = 0; i < 8; i++) {						// Pair of bits to use
		if (((code>>(14-2*i))&3) == 0) {
			colour = 0x001F; // Red
		} else if (((code>>(14-2*i))&3) == 1) {
			colour = 0x03FF; // Yellow
		} else if (((code>>(14-2*i))&3) == 2) {
			colour = 0x03E0; // Green
		} else {
			colour = 0x7C00; // Blue
		}
		for (j = 103; j < 119; j++) { 				// Row
			for (k = 32*i+8; k < 32*i+24; k++) {	// Column
				vram_a[j*256+k] = colour;
			}
		}
	}
}

/*-------------------------------------------------------------------------
arm9_main
Clears the ARM9's icahce and dcache
Clears the ARM9's DMA channels and resets video memory
Jumps to the ARM9 NDS binary in sync with the  ARM7
Written by Darkain, modified by Chishm
--------------------------------------------------------------------------*/
void arm9_main (void) {
	register int i;

	//set shared ram to ARM7
	WRAM_CR = 0x03;
	REG_EXMEMCNT = 0xE880;

	// Disable interrupts
	REG_IME = 0;
	REG_IE = 0;
	REG_IF = ~0;

	// Synchronise start
	ipcSendState(ARM9_START);
	while (ipcRecvState() != ARM7_START);

	ipcSendState(ARM9_MEMCLR);

	arm9_clearCache();

	for (i=0; i<16*1024; i+=4) {  //first 16KB
		(*(vu32*)(i+0x00000000)) = 0x00000000;      //clear ITCM
		(*(vu32*)(i+0x00800000)) = 0x00000000;      //clear DTCM
	}

	for (i=16*1024; i<32*1024; i+=4) {  //second 16KB
		(*(vu32*)(i+0x00000000)) = 0x00000000;      //clear ITCM
	}

	(*(vu32*)0x00803FFC) = 0;   //IRQ_HANDLER ARM9 version
	(*(vu32*)0x00803FF8) = ~0;  //VBLANK_INTR_WAIT_FLAGS ARM9 version

	// Clear out FIFO
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
	REG_IPC_FIFO_CR = 0;

	// Blank out VRAM
	VRAM_A_CR = 0x80;
	VRAM_B_CR = 0x80;
// Don't mess with the VRAM used for execution
//	VRAM_C_CR = 0;
	VRAM_D_CR = 0x80;
	VRAM_E_CR = 0x80;
	VRAM_F_CR = 0x80;
	VRAM_G_CR = 0x80;
	VRAM_H_CR = 0x80;
	VRAM_I_CR = 0x80;
	BG_PALETTE[0] = 0xFFFF;
	dmaFill((void*)&arm9_BLANK_RAM, BG_PALETTE+1, (2*1024)-2);
	dmaFill((void*)&arm9_BLANK_RAM, OAM,     2*1024);
	dmaFill((void*)&arm9_BLANK_RAM, VRAM_A,  256*1024);		// Banks A, B
	dmaFill((void*)&arm9_BLANK_RAM, VRAM_D,  272*1024);		// Banks D, E, F, G, H, I

	// Clear out display registers
	vu16 *mainregs = (vu16*)0x04000000;
	vu16 *subregs = (vu16*)0x04001000;
	for (i=0; i<43; i++) {
		mainregs[i] = 0;
		subregs[i] = 0;
	}

	// Clear out ARM9 DMA channels
	for (i=0; i<4; i++) {
		DMA_CR(i) = 0;
		DMA_SRC(i) = 0;
		DMA_DEST(i) = 0;
		TIMER_CR(i) = 0;
		TIMER_DATA(i) = 0;
	}

	REG_DISPSTAT = 0;
	videoSetMode(0);
	videoSetModeSub(0);
	VRAM_A_CR = 0;
	VRAM_B_CR = 0;
// Don't mess with the VRAM used for execution
//	VRAM_C_CR = 0;
	VRAM_D_CR = 0;
	VRAM_E_CR = 0;
	VRAM_F_CR = 0;
	VRAM_G_CR = 0;
	VRAM_H_CR = 0;
	VRAM_I_CR = 0;
	REG_POWERCNT  = 0x820F;

	// set ARM9 state to ready and wait for instructions from ARM7
	ipcSendState(ARM9_READY);
	while (ipcRecvState() != ARM7_BOOTBIN) {
		if (ipcRecvState() == ARM7_ERR) {
			arm9_errorOutput (arm9_errorCode);
			// Halt after displaying error code
			while(1);
		}
	}

	arm9_reset();
}

