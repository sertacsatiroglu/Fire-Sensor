;***************************************************************
; Lab2 Question 3
;***************************************************************    
;***************************************************************
; EQU Directives
; These directives do not allocate memory
;***************************************************************
;LABEL      DIRECTIVE   VALUE       COMMENT
loopvar1 	EQU 0x927C0
loopvar2 	EQU 0x18
GPIO_PORTB_DATA EQU 0x400053FC
GPIO_PORTB_DIR EQU 0x40005400
GPIO_PORTB_AFSEL EQU 0x40005420
GPIO_PORTB_DEN EQU 0x4000551C
GPIO_PORTB_LOCK EQU 0x40005520
GPIO_PORTB_CR EQU 0x40005524
GPIO_PORTB_PUR EQU 0x40005510
GPIO_PORTB_ISR EQU 0x40005404
GPIO_PORTB_IBE EQU 0x40005408
GPIO_PORTB_IMR EQU 0x40005410
GPIO_PORTB_ICR EQU 0x4000541C	
GPIO_PORTB_IEV EQU 0x4000540C
GPIO_PORTB_DR2R EQU 0x40005500
GPIO_PORTB_PDR EQU 0x40005514
GPIO_PORTB_ODR EQU 0x40005518
SYSCTL_RCGCGPIO EQU 0x400FE608
NVIC_PRI0_R EQU 0xE000E400
Priority EQU 0xE04080E0
	
ROW4		EQU	0x07
ROW3		EQU	0x0B
ROW2		EQU	0x0D
ROW1		EQU	0x0E
COL4		EQU	0x70
COL3		EQU	0xB0
COL2		EQU	0xD0
COL1		EQU	0xE0
;***************************************************************
; Program section                        
;***************************************************************
;LABEL      DIRECTIVE   VALUE       COMMENT
            AREA        main, READONLY, CODE
            THUMB
            EXPORT      keypadfunc      ; Make available
			EXPORT		DELAY200
			EXPORT		keypadgpio
				
DELAY200 	PROC
			LDR R0, =loopvar1
loopd2 		SUBS R0, R0, #1
			BNE loopd2
			BX LR
			ENDP

keypadgpio PROC
		    LDR R1 , =SYSCTL_RCGCGPIO
			LDR R0, [R1]
			ORR R0, #0x02
			STR R0, [R1]
			NOP
			NOP
			NOP
			LDR R1 , =GPIO_PORTB_DIR
			LDR R0, [R1]
			BIC R0 , #0xFF
			ORR R0, R0, #0x0F
			STR R0, [R1]
			LDR R1,=GPIO_PORTB_AFSEL
			LDR R0, [R1]
			BIC R0, #0xFF
			STR R0, [R1]
			LDR R1,=GPIO_PORTB_DEN
			MOV R0, #0xFF
			STR R0, [R1]
			LDR R1,=GPIO_PORTB_PUR
			MOV	R0, #0xF0
			STR	R0, [R1]
			
			LDR R1,=GPIO_PORTB_ISR
			MOV	R0, #0x00
			STR	R0, [R1]
			
			LDR R1,=GPIO_PORTB_IBE
			STR	R0, [R1]
			
			LDR R1,=GPIO_PORTB_IEV
			MOV	R0, #0x00
			STR	R0, [R1]
			
			LDR R1,=GPIO_PORTB_IMR ; enable interrupts on falling edge 
			MOV	R0, #0xF0
			STR	R0, [R1]
			
			LDR R1,=GPIO_PORTB_ICR ; clear any pending interrupts 
			MOV	R0, #0xF0
			STR	R0, [R1]
			
			LDR R1 , =NVIC_PRI0_R
			LDR R0, [R1]
			LDR R2, =Priority
			ORR R0, R0, R2; set priority of PORTB to 2 and while here, set PORTC priority 1 "deepsleep"
			STR R0, [R1]
			
			
			LDR R1, =GPIO_PORTB_DATA
			MOV R0, #0x00
			STR R0, [R1]
			MOV R1, #0x00
			BX LR
			ENDP
			

keypadfunc	PROC
			PUSH {r4,r5,r6,r7,lr} 
			B start
			
			
			
       
start		LDR R1, =GPIO_PORTB_DATA
			MOV	R2, #0x00
			STR R2, [R1]
check		LDR R1, =GPIO_PORTB_DATA
			LDR R3, [R1]
			MOV	R2, #0xF0
			CMP R3, R2
			BEQ	check
			MOV	R0, R3
			BL checkcol
cont		MOV	R2, #0x0F
			STR R2, [R1]
			NOP
			NOP
			NOP
			MOV	R2, #ROW1
			STR	R2, [R1]
			NOP
			NOP
			NOP
			LDR R3, [R1]
			BIC	R3, #0x0F		
			MOV	R7,	#0x0
			ORR	R5, R5, R7
			CMP R3, R0
			BEQ	found
			MOV	R2, #ROW2
			STR	R2, [R1]
			NOP
			NOP
			NOP
			LDR R3, [R1]
			BIC	R3, #0x0F
			MOV	R7,	#0x4
			ORR	R5, R5, R7
			CMP R3, R0
			BEQ	found
			MOV	R2, #ROW3
			STR	R2, [R1]
			NOP
			NOP
			NOP
			LDR R3, [R1]
			BIC	R3, #0x0F
			BIC	R5, #0x0C
			MOV	R7,	#0x8
			ORR	R5, R5, R7
			CMP R3, R0
			BEQ	found
			MOV	R2, #ROW4
			STR	R2, [R1]
			NOP
			NOP
			NOP
			LDR R3, [R1]
			BIC	R3, #0x0F
			BIC	R5, #0x0C
			MOV	R7,	#0xC
			ORR	R5, R5, R7
			B found

checkcol	BIC	R0, #0x0F
			MOV	R3, #COL1
			MOV R5, #0x0
			CMP	R0, R3
			BEQ	cont
			MOV	R3, #COL2
			MOV R5, #0x1
			CMP	R0, R3
			BEQ	cont
			MOV	R3, #COL3
			MOV R5, #0x2
			CMP	R0, R3
			BEQ	cont
			MOV	R3, #COL4
			MOV R5, #0x3
			CMP	R0, R3
			BEQ	cont
found		MOV R0, R5
standby		LDR R1, =GPIO_PORTB_DATA
			MOV	R2, #0x00
			STR R2, [R1]
			BL	DELAY200
			LDR R3, [R1]
			MOV	R2, #0xF0
			CMP R3, R2
			BNE	standby
			MOV	R1, #0
			MOV	R2, #0
			MOV	R3, #0
			MOV R0, R5
			POP {r4,r5,r6,r7,pc} 
			BX LR
			ENDP
;***************************************************************
; End of the program  section
;***************************************************************
;LABEL      DIRECTIVE       VALUE                           COMMENT
            ALIGN
            END