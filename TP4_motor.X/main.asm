CONFIG FOSC = INTRC_NOCLKOUT
CONFIG WDTE = ON
CONFIG MCLRE = ON
CONFIG LVP  = OFF

#include <xc.inc>

#define motor	PORTB, 1
#define sd1	PORTB, 4
#define sd2	PORTB, 5
#define display PORTC
    
global	tens, units, voltas, Wt, STATUSt

PSECT udata_shr
tens:
    DS 1
units:
    DS 1
voltas:
    DS 1
Wt:
    DS 1
STATUSt:
    DS 1
    
PSECT code, class=CODE, abs, delta=2
ORG 0x000
resetVec:
    PAGESEL start
    goto    start
    
ORG 0x004
    movwf   Wt
    swapf   STATUS, W
    movwf   STATUSt
    
    clrf    STATUS
    btfsc   TMR2IF
    goto    T2_ISR
    btfsc   T0IF
    goto    T0_ISR
    goto    ISR_END
    
; + interrupcao timer0
T0_ISR:
    movlw   0xFE
    subwf   voltas, w
    btfss   STATUS, 2
    incf    voltas, f
    bcf	    T0IF
    goto    ISR_END
    
; + interrupcao timer2 (MUX)
T2_ISR:
    bcf	    TMR2IF
    btfss   sd1
    goto    ligasd1
ligasd2:
    bcf	    sd1
    bsf	    sd2
    movf    voltas, W
    call    LUT
    movwf   display
    goto    ISR_END
    
ligasd1:
    bcf	    sd2
    bsf	    sd1
    swapf   voltas, W
    call    LUT
    movf    tens, W
    movwf   display
    goto    ISR_END
    
; restaurar contexto
ISR_END:
    swapf   STATUSt, W
    movwf   STATUS
    swapf   Wt, f
    swapf   Wt, W
    retfie
    
PSECT code, class=CODE, delta=2
ORG 0x100
start:
    BANKSEL PORTA
    clrf    PORTA
    clrf    PORTB
    clrf    PORTC   
    
    BANKSEL ANSEL
    clrf    ANSEL
    clrf    ANSELH
    
    BANKSEL TRISA
    movlw   0x07
    movwf   TRISA
    movlw   11001101B
    movwf   TRISB
    movlw   10000000B
    movlw   TRISC
    movlw   11100100B
    movlw   OPTION_REG
    movlw   11100000B
    movwf   INTCON
    movlw   0xFA
    movwf   PR2
    
    clrf    STATUS
    movlw   00100101B
    movwf   T2CON
    movlw   0x37
    movwf   TMR0

    bsf	    motor

loop:
    clrwdt
    goto loop
    
; + LUT digitos HEX
LUT:
    andlw   0x0F
    addwf   PCL, f
    retlw   00111111B
    retlw   00110000B
    retlw   01101101B
    retlw   01111001B
    retlw   01110010B
    retlw   01011011B
    retlw   01011111B
    retlw   00110001B
    retlw   01111111B
    retlw   01110011B
    retlw   01110111B
    retlw   01011110B
    retlw   01001100B
    retlw   01111100B
    retlw   01001111B
    retlw   01000111B
        