#include<xc.inc>

CONFIG FOSC = INTRC_NOCLKOUT

global n
global d
global q
global r
    
global nibble
    
global i
    
PSECT udata_shr
 n:	    DS 1
 d:	    DS 1
 q:	    DS 1
 r:	    DS 1
 nibble:    DS 1
 i:	    DS 1
 address:   DS 1
    
PSECT resetVec, class=CODE, delta=2
resetVec:
    PAGESEL  start
    goto start
    
PSECT code, CLASS=CODE, abs, delta=2
 
start:
    
    BANKSEL PORTA
    clrf    PORTC
    clrf    PORTA
    bcf	    RE1
    
    BANKSEL ANSEL
    clrf    ANSEL
    clrf    ANSELH
    
    BANKSEL TRISA
    clrf    TRISC
    clrf    TRISA
    bcf	    TRISE, 1
    bcf	    OPTION_REG, 7
    clrf    STATUS
    
loop:
    clrwdt
    
    movlw   0x20
    movwf   FSR
    
    clrf    q
    
    movf    PORTB, W
    movwf   n
    movwf   r
    movf    PORTD, W
    movwf   d
    
checkZ:
    btfss   STATUS, 2
    goto    operation
    bsf	    RE1
    goto    result
    
operation:
    subwf   n, W
    btfss   STATUS, 0
    goto result
    
select:
    btfsc   RE0
    call    mode2
    btfss   RE0
    call    mode1
     
result:
    movf    q, W
    movwf   PORTA
    movf    r, W
    movwf   PORTC
    goto    saveresp
    
mode1:
    
    movf    d, W
    subwf   r, W
    btfss   STATUS, 0
    return
    incf    q, f
    movwf   r
    movf    r, W
    btfsc   STATUS, 2
    return
    goto mode1
    
    
mode2:

    clrf    r
    movlw   0x08
    movwf   i
    bcf	    STATUS, 0
    
subdesloc:
    rlf	    n, f
    rlf	    r, f
    movf    d, W
    subwf   r, W
    btfsc   STATUS, 0
    movwf   r
    rlf	    q, f
    decfsz  i, f
    goto    subdesloc
    return
    
saveresp:
    bcf	    RE1
    swapf   q, W
    call    table
    movwf   INDF
    incf    FSR, f
    
    movf    q, W
    call    table
    movwf   INDF
    incf    FSR, f
    
    swapf   r, W
    call    table
    movwf   INDF    
    incf    FSR, f
    
    movf    r, W
    call    table
    movwf   INDF
    
    goto    loop
    
    
ORG 0x060
table:
    andlw   0x0F
    addwf   PCL, f
    retlw   0x30
    retlw   0x31
    retlw   0x32
    retlw   0x33    
    retlw   0x34
    retlw   0x35
    retlw   0x36
    retlw   0x37
    retlw   0x38
    retlw   0x39
    retlw   0x41
    retlw   0x42
    retlw   0x43
    retlw   0x44
    retlw   0x45
    retlw   0x46
    
    