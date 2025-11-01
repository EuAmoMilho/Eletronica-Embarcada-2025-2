/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1827
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/


// CÓDIGO CORRIGIDO PELO PROFESSOR (31/10/2025)

#include "mcc_generated_files/mcc.h"
#include <math.h>
#include <pic16f1827.h>

uint8_t channel = 0;

union{
    uint16_t    value;
    uint8_t     byte[2];
}gainT;

union{
    uint16_t    value;
    uint8_t     byte[2];
}gainL;

#define RX_COUNT_MAX 6
char    rxBuffer[RX_COUNT_MAX];
uint8_t rxCount = 0;

void enviarDados(){
    float engineeringUnits;
    union{
        uint16_t    value;
        uint8_t     byte[2];
    }eU[2];
    
    FVRCON = 0b10000100;
    __delay_us(10);
    engineeringUnits = (float)ADC_GetConversion(channel_AN0)*((float)gainT.value)/1000;
    eU[0].value = round(engineeringUnits);
    
    FVRCON = 0b10001100;
    __delay_us(10);
    engineeringUnits = (float)ADC_GetConversion(channel_AN1)*((float)gainL.value)/1000;
    eU[1].value = round(engineeringUnits);//fazer for p reduzir codigo
    
    EUSART_Write(0x80);
    EUSART_Write(eU[0].byte[1]);
    EUSART_Write(eU[0].byte[0]);
    EUSART_Write(eU[1].byte[1]);
    EUSART_Write(eU[1].byte[0]);
}

uint16_t removeZeros(uint16_t entrada){
    uint16_t copia = entrada & 0b0000000001111111;
    return (((entrada>>1) & 0b0011111110000000) | copia);
   
}

uint16_t bigToLittle(uint16_t entrada){
    union{
        uint16_t full;
        uint8_t byte[2];
    }value;
    value.full = entrada;
    uint8_t temp = value.byte[1];
    value.byte[1] = value.byte[0];
    value.byte[0] = temp;
    
    return value.full;
}

void decodeMsg(){
        union{
            uint16_t full;
            uint8_t byte[2];
        }value;
                
        value.byte[0] = rxBuffer[1];
        value.byte[1] = rxBuffer[2];
    
        switch(rxBuffer[0]){
        case 0x80://toggle info display
            if(rxBuffer[2] == 0x00){
                TMR1_StopTimer();
            }else if(rxBuffer[2] == 0x01){
                TMR1_StartTimer();
            }
            break;
        
        case 0x81://duty cycle
            
            value.full = removeZeros(value.full);
            EPWM2_LoadDutyValue(bigToLittle(value.full));
            
            break;
        
        case 0x82://temp gain
            
            value.full = removeZeros(value.full);
            gainT.value = bigToLittle(value.full);
            
            break;
            
        case 0x83://lum gain

            
            value.full = removeZeros(value.full);
            gainL.value = bigToLittle(value.full);
            
            break;
    }
}

/*
                         Main application
 */
int main(){
    // initialize the device
    SYSTEM_Initialize();
    TMR1_SetInterruptHandler(enviarDados);
        
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();


    while (1)
    {
        if(EUSART_is_rx_ready()){
            uint8_t dados = EUSART_Read();
            if(dados & 0x80){
                rxCount = 0;
            }
            else if(rxCount > 2){
                decodeMsg();
            }
            else if(rxCount < RX_COUNT_MAX){
                rxBuffer[rxCount] = dados;
                rxCount++;
            }   
        }
    }
    return 0;
}
/**
 End of File
*/
