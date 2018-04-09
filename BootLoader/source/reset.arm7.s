/*
	Copyright 2015 Dave Murphy (WinterMute)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
	.text
	.align 4

	.arm
@---------------------------------------------------------------------------------
	.global arm7_reset
	.type	arm7_reset STT_FUNC
@---------------------------------------------------------------------------------
arm7_reset:
@---------------------------------------------------------------------------------
	mrs	r0, cpsr			@ cpu interrupt disable
	orr	r0, r0, #0x80			@ (set i flag)
	msr	cpsr, r0

	ldr	r0, =0x380FFFC         @ irq vector
	mov	r1, #0
	str 	r1, [r0]
	sub	r0, r0, #4             @ IRQ1 Check Bits
	str 	r1, [r0]
	sub	r0, r0, #4             @ IRQ2 Check Bits
	str 	r1, [r0]

	bic	r0, r0, #0x7f

	msr	cpsr_c, #0xd3      @ svc mode
	mov	sp, r0
	sub	r0, r0, #64
	msr	cpsr_c, #0xd2      @ irq mode
	mov	sp, r0
	sub	r0, r0, #512
	msr	cpsr_c, #0xdf      @ system mode
	mov	sp, r0

	mov	r12, #0x04000000
	add	r12, r12, #0x180

	@ while (ipcRecvState() != ARM9_RESET);
	mov	r0, #2
	bl	waitsync
	@ ipcSendState(ARM7_RESET)
	mov	r0, #0x200
	strh	r0, [r12]

	@ while(ipcRecvState() != ARM9_BOOT);
	mov	r0, #0
	bl	waitsync
	@ ipcSendState(ARM7_BOOT)
	strh	r0, [r12]
	
	ldr	r0,=0x2FFFE34

	ldr	r0,[r0]
	bx	r0

	.pool

@---------------------------------------------------------------------------------
waitsync:
@---------------------------------------------------------------------------------
	ldrh	r1, [r12]
	and	r1, r1, #0x000f
	cmp	r0, r1
	bne	waitsync
	bx	lr
