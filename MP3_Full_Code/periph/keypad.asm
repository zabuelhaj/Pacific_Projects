;; Boilerplate code for keypad procedures														
;; Keypad pinouts, left to right: PC7 PC6 PC5 PC4 PH3 PH2 PH1 PH0

        .thumb                          ; generate Thumb-2 code
        .text                           ; switch to code (ROM) section

;; Place declarations and references for variables here
		.bss			col, 1, 1		; Static variable for getKey procedure.
		.bss			table, 4, 4		; Constant for lookup table.
;; Place definitions for peripheral registers (base addresses, 
;;  register offsets, and field constants here).  Follow the 
;;  example for the SYSCTL RCGCCPIO register
        .align  4                       ; force word alignment

COLADR:
						.word	col		; Address for the static column variable.

LOOKUP:					.word	table	; Address for the lookup table.

SYSCTL:                 .word   0x400fe000
SYSCTL_RCGCCPIO         .equ    0x608
SYSCTL_RCGCGPIO_PORTC   .equ    (1<<2)
SYSCTL_RCGCGPIO_PORTH   .equ    (1<<7)

GPIO_PORTC:				.word	0x4005a000
GPIO_PORTH:				.word	0x4005f000

PIN3_0					.equ	0x0f
PIN7_4					.equ	0xf0
PIN4					.equ	(1<<4)
PIN5					.equ	(1<<5)
PIN6					.equ	(1<<6)
PIN7					.equ	(1<<7)

GPIO_DIR				.equ	0x400
GPIO_DEN				.equ	0x51c
GPIO_PUR				.equ	0x510



;; Private procedure uint8_t doLookup( uint8_t row, uint8_t col);
        .align  4                       ; force word alignment
doLookup:   .asmfunc

		push	{R2-R3, LR}

		mov		R2, #4
		mul		R2, R0, R2				; R2 = R0 * R2 -> R2 = row * 4.
		add		R2, R2, R1				; R2 = R2 + R1, our index -> R2 = R2 + column.

		ldr		R3, LOOKUP
		ldr		R3, [R3]
		ldrb		R0, [R3, R2]

		pop 	{R2-R3, PC}

;;		mov		PC, LR
        .endasmfunc

;; Global procedure void initKeypad( const uint8_t lookup[4][4] );
        .align  4                       ; force word alignment
initKeypad:   .global	initKeypad
		.asmfunc

		push	{R0, LR}				; Accept lookup table argument.
		ldr		R1, LOOKUP
		str		R0, [R1]

		;; Set up the SYSCTL, GPIO PORT C and H.

		ldr     R1, SYSCTL
		ldr     R0, [R1, #SYSCTL_RCGCCPIO]
		orr     R0, #SYSCTL_RCGCGPIO_PORTC|SYSCTL_RCGCGPIO_PORTH
		str     R0, [R1, #SYSCTL_RCGCCPIO]
		ldr     R0, [R1, #SYSCTL_RCGCCPIO]
		orr     R0, #SYSCTL_RCGCGPIO_PORTC|SYSCTL_RCGCGPIO_PORTH
		str     R0, [R1, #SYSCTL_RCGCCPIO]

		ldr     R1, GPIO_PORTH
		ldr     R0, [R1, #GPIO_DIR]
		orr     R0, #PIN3_0
		str     R0, [R1, #GPIO_DIR]
		ldr     R0, [R1, #GPIO_DEN]
		orr     R0, #PIN3_0
		str     R0, [R1, #GPIO_DEN]

		ldr     R1, GPIO_PORTC
		ldr     R0, [R1, #GPIO_DIR]
		bic     R0, #PIN7_4
		str     R0, [R1, #GPIO_DIR]
		ldr     R0, [R1, #GPIO_PUR]
		orr     R0, #PIN7_4
		str     R0, [R1, #GPIO_PUR]
		ldr     R0, [R1, #GPIO_DEN]
		orr     R0, #PIN7_4
		str     R0, [R1, #GPIO_DEN]

		ldr		R1, COLADR				; Load the address for the static variable current_col.
		mov		R0, #0
		strb 	R0, [R1]

		mov		R0, #0111b
		ldr		R1, GPIO_PORTH
		str		R0, [R1, #PIN3_0 << 2]

		pop		{R0, PC}
        .endasmfunc

;; Global procedure bool getKey( uint8_t* key );
getKey:		.global getKey
		.asmfunc
		push	{R4-R7, LR}			; Push the argument "key".
		mov		R4, #0					; R4 = 0 for row = 0

		;; if((PC[0x0F]!=0x0F){
		ldr     R5, GPIO_PORTC
		ldr		R6, [R5, #PIN7_4 << 2]

		cmp		R6, #PIN7_4
		beq		else4if1

		cmp		R6, #0xf0
		bne		if1

if1:
		;; if(PC[1<<4]==0){
		ldr		R6, GPIO_PORTC
		ldr		R7, [R6, #PIN7_4 << 2]

		cmp		R7, #0x70
		bne		if2
		mov		R4, #0

		b		callDL

if2:
		;; else if(PC[1<<5]==0){
		ldr		R6, GPIO_PORTC
		ldr		R7, [R6, #PIN7_4 << 2]

		cmp		R7, #0xB0
		bne		if3
		mov		R4, #1

		b		callDL

if3:
		;; else if(PC[1<<6]==0){
		ldr		R6, GPIO_PORTC
		ldr		R7, [R6, #PIN7_4 << 2]

		cmp		R7, #0xD0
		bne		if4
		mov		R4, #2

		b		callDL

if4:
		;; else{
		ldr		R6, GPIO_PORTC
		ldr		R7, [R6, #PIN7_4 << 2]

		cmp		R7, #0xE0
		mov		R4, #3

		b		callDL

callDL:
		mov		R5, R0
		mov		R0, R4					; Move the row into R0 so that it can be passed as a parameter into doLookup.
		ldr		R1, COLADR				; Load the address for the static variable current_col.
		ldrb 	R1, [R1]				; R1 = column & R0 is row.
		bl		doLookup				; Row is R0 and Column is R1													;; WHY THO
		strb 	R0, [R5]				; doLookup returns R0, which needs to be stored into R4 for the return value.

		;return true
		mov		R0, #1					; The procedure returns R0 by default.

		pop 	{R4-R7, PC}

else4if1:
		ldr		R0, COLADR
		ldrb	R1, [R0]

switch:
		cmp		R1, #0
		beq		case0
		cmp		R1, #1
		beq		case1
		cmp		R1, #2
		beq		case2
		cmp		R1, #3
		beq		case3


case0:
		ldr		R1, COLADR				; Load the address for the static variable current_col.
		mov		R0, #1
		strb 	R0, [R1]

		mov		R0, #1011b
		ldr		R1, GPIO_PORTH
		str		R0, [R1, #PIN3_0 << 2]

		b returnFalse

case1:
		ldr		R1, COLADR				; Load the address for the static variable current_col.
		mov		R0, #2
		strb 	R0, [R1]

		mov		R0, #1101b
		ldr		R1, GPIO_PORTH
		str		R0, [R1, #PIN3_0 << 2]

		b		returnFalse

case2:
		ldr		R1, COLADR				; Load the address for the static variable current_col.
		mov		R0, #3
		strb 	R0, [R1]

		mov		R0, #1110b
		ldr		R1, GPIO_PORTH
		str		R0, [R1, #PIN3_0 << 2]

		b		returnFalse

case3:
		ldr		R1, COLADR				; Load the address for the static variable current_col.
		mov		R0, #0
		strb 	R0, [R1]

		mov		R0, #0111b
		ldr		R1, GPIO_PORTH
		str		R0, [R1, #PIN3_0 << 2]

		b		returnFalse

returnFalse:

		;return false
		mov		R0, #0

		pop 	{R4-R7, PC}

		.endasmfunc


        .end
