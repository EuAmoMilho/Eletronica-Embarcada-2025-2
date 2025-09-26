CONFIG FOSC = INTRC_NOCLKOUT
CONFIG WDTE = ON
CONFIG MCLRE = ON
CONFIG LVP  = OFF

#include <xc.inc>
    
#define LedR	PORTA, 0
#define LedG	PORTA, 1
    
CTMR0 EQU 0x0B
 
PSECT code, class=CODE, abs, delta=2
ORG 0x000
resetVec:
    PAGESEL start
    goto    start

PSECT code, class=CODE, delta=2
ORG 0x010
start:
    BANKSEL PORTA
    clrf    PORTA
    
    BANKSEL ANSEL
    clrf    ANSEL
    
    BANKSEL WDTCON
    movlw   0x21
    movwf   WDTCON
    clrf    TMR0
    
    BANKSEL TRISA
    movlw   0xF8
    movwf   TRISA
    
    movlw   0x31
    movwf   OSCCON
    
    movlw   0xC7
    movwf   OPTION_REG
        
    clrf    STATUS
    movlw   0x07
    movwf   PORTA
    
loop:
    clrwdt
    bcf	    LedR
    call    LdTMR
    clrwdt
    sleep
    nop
    bsf	    LedR
    
    bcf	    LedG
    call    LdTMR
    clrwdt
    sleep
    nop
    bsf	    LedG
    
    goto    loop
    
LdTMR:
    movlw   CTMR0
    movwf   TMR0
    bcf	    T0IF
    
TMRLOOP:
    clrwdt
    btfss   T0IF
    goto    TMRLOOP
    
    return
    