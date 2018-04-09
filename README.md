Nitro Hax
=========

By Chishm

Nitro Hax is a cheat tool for the Nintendo DS. 
It works with original games only.

The newest release is available to
[download](https://www.chishm.com/NitroHax/NitroHax.zip) from the
[Nitro Hax web page](https://www.chishm.com/NitroHax/index.html).

Usage
=====

1. Patch NitroHax.nds with a DLDI file if you need to.
2. Copy the NitroHax.nds file to your media device.
3. Place an Action Replay XML file on your media device.
4. Start NitroHax.nds from your media device
   1. One of the following will be loaded automatically if it is found (in order
   of preference):
      * "cheats.xml" in the current directory
      * "/NitroHax/cheats.xml"
      * "/data/NitroHax/cheats.xml"
      * "/cheats.xml"
   2. If no file is found, browse for and select a file to open.
5. Remove your media device if you want to.
6. Remove any card that is in Slot-1
7. Insert the DS game into Slot-1
8. Choose the cheats you want to enable.
   1. Some cheats are enabled by default and others may be always on. This is
   specified in the XML file.
   2. The keys are:
      * **A**: Open a folder or toggle a cheat enabled
      * **B**: Go up a folder or exit the cheat menu if at the top level
      * **X**: Enable all cheats in current folder
      * **Y**: Disable all cheats in current folder
      * **L**: Move up half a screen
      * **R**: Move down half a screen
      * **Up**: Move up one line
      * **Down**: Move down one line
      * **Start**: Start the game
9. When you are done, exit the cheat menu.
10. The game will then start with cheats running.


Copyright
=========

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


Acknowledgements
================

Thanks to (in no particular order):
* Pink-Lightning - Original skin (v0.5-0.82)
* bLAStY - Memory dumps
* cReDiAr - Last crucial step for running DS Cards
* Parasyte - Tips for hooking the game automatically
* kenobi - Action Replay code document
* Darkain - Memory and cache clearing code
* Martin Korth - GBAtek
* Deathwind / WinterMute - File menu code (v0.2 - v0.4)
* WinterMute - dslink source, reset code
* Everyone else who helped me along the way

Big thanks to Datel (CodeJunkies) for creating the original Action Replay and
its cheats.


Custom Code Types
=================

In addition to the standard Action Replay DS code types described on
[EnHacklopedia's Action Replay DS page](http://doc.kodewerx.org/hacking_nds.html#arcodetypes),
Nitro Hax supports the following custom codes.

`CF000000 00000000`: Code list end
----------------------------------
Code used internally to specify the end of the cheat code list. It does not need
to be specified manually.

`CF000001 xxxxxxxx`: Relocate cheat engine
------------------------------------------
Relocate the cheat engine to address `xxxxxxxx`. The cheat engine and all data
are moved to the given address, which should be accessible from the ARM7.

`CF000002 xxxxxxxx`: Hook address
---------------------------------
Change the hook address to `xxxxxxxx`. The hook should be a function pointer
that is called regularly. By default the ARM7's VBlank interrupt handler is
hooked. This code overrides the default.

`C100000x yyyyyyyy`: Call function with arguments
-------------------------------------------------
Call a function with between 0 and 4 arguments. The argument list follows this
code, which has the parameters:
* `x`: Number of arguments (0 - 4)
* `yyyyyyyy`: Address of function

For example, to call a function at `0x02049A48` with the three arguments
`r0 = 0x00000010`, `r1 = 0x134CBA9C`, and `r2 = 0x12345678`, you would use:
```
C1000003 02049A48
00000010 134CBA9C
12345678 00000000
```

`C200000x yyyyyyyy`: Run ARM/THUMB code
---------------------------------------
Run ARM or THUMB code stored in the cheat list.
* `x`: `0` = ARM mode, `1` = THUMB mode
* `yyyyyyyy`: length of function in bytes

For example:
```
C2000000 00000010
AAAAAAAA BBBBBBBB
CCCCCCCC E12FFF1E
```
This will run the code `AAAAAAAA BBBBBBBB CCCCCCCC` in ARM mode.
The `E12FFF1E` (`bx lr`) is needed at the end to return to the cheat engine.

The above instructions are based on those written by kenobi.

`C4000000 xxxxxxxx`: Scratch space
----------------------------------
Provide 4 bytes of scratch space to safely store data. Sets the offset register
to point to the first word of this code. Storing data at `[offset+4]` will save
over the top of `xxxxxxxx`.

This is based on a Trainer Toolkit code.

`C5000000 xxxxyyyy`: Counter
----------------------------
Each time the cheat engine is executed, the counter is incremented by 1.
If `(counter & yyyy) == xxxx` then execution status is set to true, else it is
set to false.

This is based on a Trainer Toolkit code.

`C6000000 xxxxxxxx`: Store offset
---------------------------------
Stores the offset register to the address `xxxxxxxx`.

This is based on a Trainer Toolkit code.

`D400000x yyyyyyyy`: Dx Data operation
--------------------------------------
Performs the operation `Data = Data ? yyyyyyyy` where `?` is determined by `x`
as follows:
* `0`: add
* `1`: or
* `2`: and
* `3`: xor
* `4`: logical shift left
* `5`: logical shift right
* `6`: rotate right
* `7`: arithmetic shift right
* `8`: multiply

If-type codes
-------------
For codes begining with `3`-`9` or `A` (if-type codes), the offset register can
be used for address calculations. If the lowest bit of the code's address is set
then the offset is added to the address. If the address is `0x00000000` then the
offset is used instead.

For example, if the code `32009001 00001000` is run and the offset register is
currently `0x000000AC`, then:
1. the address is taken from the first code word as `0x02009000`,
2. the offset is added (because the lowest bit of the first codeword is set to
`1`) to give an address of `0x020090AC`,
3. the memory at address `0x020090AC` is read, and the value compared to
`0x00001000` (the second codeword).
4. Now (since the code starts with `3`) if the codeword value is greater than
the value read from memory, then the rest of the codes in the list will be
executed, otherwise they will be skipped until the next `D0000000 00000000` code
is reached.
