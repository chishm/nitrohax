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

#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <string.h>
#include <malloc.h>
#include <list>

#include "cheat.h"
#include "ui.h"
#include "nds_card.h"
#include "cheat_engine.h"
#include "crc.h"
#include "version.h"

const char TITLE_STRING[] = "Nitro Hax " VERSION_STRING "\nWritten by Chishm";
const char* defaultFiles[] = {"cheats.xml", "/DS/NitroHax/cheats.xml", "/NitroHax/cheats.xml", "/data/NitroHax/cheats.xml", "/cheats.xml"};


static inline void ensure (bool condition, const char* errorMsg) {
	if (false == condition) {
		ui.showMessage (errorMsg);
		while(1) swiWaitForVBlank();
	}

	return;
}	

//---------------------------------------------------------------------------------
int main(int argc, const char* argv[])
{
	u32 ndsHeader[0x80];
	u32* cheatDest;
	int curCheat = 0;
	char gameid[4];
	uint32_t headerCRC;
	std::string filename;
	int c;
	FILE* cheatFile;
	
	ui.showMessage (UserInterface::TEXT_TITLE, TITLE_STRING);

#ifdef DEMO
	ui.demo();
	while(1);
#endif

	ensure (fatInitDefault(), "FAT init failed");

	// Read cheat file
	for (u32 i = 0; i < sizeof(defaultFiles)/sizeof(const char*); i++) {
		cheatFile = fopen (defaultFiles[i], "rb");
		if (NULL != cheatFile) break;
	}
	if (NULL == cheatFile) {
		filename = ui.fileBrowser (".xml");
		ensure (filename.size() > 0, "No file specified");
		cheatFile = fopen (filename.c_str(), "rb");
		ensure (cheatFile != NULL, "Couldn't load cheats"); 
	}
	
	ui.showMessage (UserInterface::TEXT_TITLE, TITLE_STRING);
	ui.showMessage ("Loading codes");
	
	c = fgetc(cheatFile);
	ensure (c != 0xFF && c != 0xFE, "File is in an unsupported unicode encoding");
	fseek (cheatFile, 0, SEEK_SET);
	
	CheatCodelist* codelist = new CheatCodelist();
	ensure (codelist->load(cheatFile), "Can't read cheat list\n");
	fclose (cheatFile);

	ui.showMessage (UserInterface::TEXT_TITLE, TITLE_STRING);
	
	sysSetCardOwner (BUS_OWNER_ARM9);

	ui.showMessage ("Loaded codes\nYou can remove your flash card\nRemove DS Card");
	do {
		swiWaitForVBlank();
		getHeader (ndsHeader);
	} while (ndsHeader[0] != 0xffffffff);

	ui.showMessage ("Insert Game");
	do {
		swiWaitForVBlank();
		getHeader (ndsHeader);
	} while (ndsHeader[0] == 0xffffffff);

	// Delay half a second for the DS card to stabilise
	for (int i = 0; i < 30; i++) {
		swiWaitForVBlank();
	}	
	
	getHeader (ndsHeader);

	ui.showMessage ("Finding game");

	memcpy (gameid, ((const char*)ndsHeader) + 12, 4);
	headerCRC = crc32((const char*)ndsHeader, sizeof(ndsHeader));
	CheatFolder *gameCodes = codelist->getGame (gameid, headerCRC);
	
	if (!gameCodes) {
		gameCodes = codelist;
	}
	
	ui.cheatMenu (gameCodes, gameCodes);
	

	cheatDest = (u32*) malloc(CHEAT_MAX_DATA_SIZE);
	ensure (cheatDest != NULL, "Bad malloc\n");
	
	std::list<CheatWord> cheatList = gameCodes->getEnabledCodeData();
	
	for (std::list<CheatWord>::iterator cheat = cheatList.begin(); cheat != cheatList.end(); cheat++) {
		cheatDest[curCheat++] = (*cheat);
	}
	
	ui.showMessage (UserInterface::TEXT_TITLE, TITLE_STRING);
	ui.showMessage ("Running game");

	runCheatEngine (cheatDest, curCheat * sizeof(u32));

	while(1) {

	}

	return 0;
}
