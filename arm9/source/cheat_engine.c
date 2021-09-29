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

#include <string.h>
#include <nds.h>

#include "load_bin.h"
#include "cheat_engine.h"
#define LCDC_BANK_C (vu16*)0x06840000

#define CHEAT_DATA_LOCATION ((u32*)0x06850000)
#define CHEAT_CODE_END	0xCF000000

static void vramset (volatile void* dst, u16 val, int len)
{
	vu32* dst32 = (vu32*)dst;
	u32 val32 = val | (val << 16);

	for ( ; len > 0; len -= 4) {
		*dst32++ = val32;
	}
}

static void vramcpy (volatile void* dst, const void* src, int len)
{
	vu32* dst32 = (vu32*)dst;
	const u32* src32 = (u32*)src;

	for ( ; len > 0; len -= 4) {
		*dst32++ = *src32++;
	}
}

void runCheatEngine (void* cheats, int cheatLength)
{
	irqDisable(IRQ_ALL);


	// Direct CPU access to VRAM bank C
	VRAM_C_CR = VRAM_ENABLE | VRAM_C_LCD;
	// Clear VRAM
	vramset (LCDC_BANK_C, 0x0000, 128 * 1024);
	// Load the loader/patcher into the correct address
	vramcpy (LCDC_BANK_C, load_bin, load_bin_size);
	// Put the codes 64KiB after the start of the loader
	vramcpy (CHEAT_DATA_LOCATION, cheats, cheatLength);
	// Mark the end of the code list
	CHEAT_DATA_LOCATION[cheatLength/sizeof(u32)] = CHEAT_CODE_END;
	CHEAT_DATA_LOCATION[cheatLength/sizeof(u32) + 1] = 0;	

	// Give the VRAM to the ARM7
	VRAM_C_CR = VRAM_ENABLE | VRAM_C_ARM7_0x06000000;	
	
	// Reset into a passme loop
	REG_EXMEMCNT = 0xffff;
	*((vu32*)0x027FFFFC) = 0;
	*((vu32*)0x027FFE04) = (u32)0xE59FF018;
	*((vu32*)0x027FFE24) = (u32)0x027FFE04;
	swiSoftReset(); 
}
