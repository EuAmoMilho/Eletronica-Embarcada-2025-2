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

#include "mcc_generated_files/mcc.h"
#include <math.h>

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

void comecarAnalog(){
    EUSART_Write(0x80);
    channel = 0;
    ADC_SelectChannel(channel_AN0);
    __delay_us(5);
    ADC_StartConversion();
    
    channel = 1;
    ADC_SelectChannel(channel_AN1);
    __delay_us(5);
    ADC_StartConversion();
}

void enviarDados(){
    float engineeringUnits;
    union{
        uint16_t    value;
        uint8_t     byte[2];
    }eU;
    
    if(channel == 0){
        engineeringUnits = ADC_GetConversionResult()*(float)gainT.value/1000;
    }else if(channel == 1){
        engineeringUnits = ADC_GetConversionResult()*(float)gainL.value/1000;
    }
    
    eU.value = round(engineeringUnits);
    
    EUSART_Write(eU.byte[1]);
    EUSART_Write(eU.byte[0]);
}

void decodeMsg(){
    switch(rxBuffer[0]){
        case 0x80://toggle info display
            if(rxBuffer[2] == 0x00){
                PIR1bits.TMR1IF = 1;//ao habilitar ou desabilitar a
                                    //flag de interrupcao, controlamos
                                    //o atendimento das interrupcoes
            }else if(rxBuffer[2] == 0x01){
                PIR1bits.TMR1IF = 0;
            }
            
            break;
        
        case 0x81://duty cycle
            union{
                uint16_t fullDuty;
                uint8_t byteDuty[1];
            }dutyValue;
            
            //ESTRUTURA QUE REMOVE OS ZEROS DOS BIT[7]. EXEMPLO: 
            //1101111111(37F) = 111111111(1FF)
            if(dutyValue.byteDuty[1]>0 && dutyValue.byteDuty[1]%2 == 0){
                dutyValue.byteDuty[0] = rxBuffer[1] >> 1;
                dutyValue.byteDuty[1] = rxBuffer[2];
                
            }else if(dutyValue.byteDuty[1]>0 && dutyValue.byteDuty[1]%2 == 1){
                dutyValue.byteDuty[0] = rxBuffer[1] >> 1;
                dutyValue.byteDuty[1] = 0x80 + rxBuffer[2];
            
            }else{
                dutyValue.byteDuty[0] = rxBuffer[1];
                dutyValue.byteDuty[1] = rxBuffer[2];
            }
            
            if(dutyValue.fullDuty <= 1023){
                EPWM2_LoadDutyValue(dutyValue.fullDuty);
            }
            
            break;
        
        case 0x82://temp gain
            if(gainT.byte[1]>0 && gainT.byte[1]%2 == 0){
                gainT.byte[0] = rxBuffer[1] >> 1;
                gainT.byte[1] = rxBuffer[2];
                
            }else if(gainT.byte[1]>0 && gainT.byte[1]%2 == 1){
                gainT.byte[0] = rxBuffer[1] >> 1;
                gainT.byte[1] = 0x80 + rxBuffer[2];
            
            }else{
                gainT.byte[0] = rxBuffer[1];
                gainT.byte[1] = rxBuffer[2];
            }
            break;
            
        case 0x83://lum gain
            if(gainL.byte[1]>0 && gainL.byte[1]%2 == 0){
                gainL.byte[0] = rxBuffer[1] >> 1;
                gainL.byte[1] = rxBuffer[2];
                
            }else if(gainL.byte[1]>0 && gainL.byte[1]%2 == 1){
                gainL.byte[0] = rxBuffer[1] >> 1;
                gainL.byte[1] = 0x80 + rxBuffer[2];
            
            }else{
                gainL.byte[0] = rxBuffer[1];
                gainL.byte[1] = rxBuffer[2];
            }
            break;
    }
}

/*
                         Main application
 */
int main(){
    // initialize the device
    SYSTEM_Initialize();
    TMR1_SetInterruptHandler(comecarAnalog);
    ADC_SetInterruptHandler(enviarDados);
        
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();


    while (1)
    {
        if(EUSART_is_rx_ready()){
            uint8_t dados = EUSART_Read();
            if(dados >= 0x80 || rxCount > RX_COUNT_MAX-1){
                rxCount = 0;
            }
            else if(rxCount > 2){
                decodeMsg();
            }
            else{
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