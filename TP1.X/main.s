#include <xc.inc>

global v1
global v2

PSECT udata_shr

v1:
    DS 1
v2: 
    DS 1

psect resetVec, class=CODE, delta=2
resetVec:
    PAGESEL start
    goto start
    
psect code, class=CODE, delta=2
    start:
    BANKSEL PORTA
    clrf PORTA
    
    BANKSEL ANSEL
    clrf ANSEL
    clrf ANSELH
    
    BANKSEL TRISA
    movlw 11100000B
    movwf TRISA
    bcf OPTION_REG, 7
    clrf STATUS
    
loop:
    clrwdt		; clearwatchdogtimer
    movf PORTB, W	; leitura de entradas
    movwf v1
    movf PORTC, W
    movwf v2
    
    movlw 0x0F		; preparando os dados
    andwf v1, f
    andwf v2, f
    
    btfsc PORTA, 5
    goto adicao
    
    movf v2, W
    subwf v1, W
    movwf PORTA
    goto loop
    
    adicao:
    movf v1, W
    addwf v2, W
    movwf PORTA

    goto loop
    