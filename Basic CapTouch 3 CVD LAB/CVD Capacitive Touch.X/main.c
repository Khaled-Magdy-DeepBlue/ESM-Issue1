/*
 * File:   main.c
 * Author: Khaled Magdy
 */
#include <xc.h>
#include "config.h"
#include <stdint.h>
#include <stdio.h>

#define _XTAL_FREQ    48000000
#define SensorLine_D  TRISC0
#define SensorLine    RC0
#define LED_D         TRISB0
#define LED           RB0
uint16_t AD_RES = 0;
char Buffer[10];
//------------------------------------
void ADC_Init();
void CVD_Touch_Sample();
void UART_TX_Init();
void UART_Write(uint8_t data);
void UART_Write_String(uint8_t *text);
//====================================
void main(void) {
    
    ADC_Init();
    UART_TX_Init();
    SensorLine_D = 0;
    SensorLine = 0;
    LED_D = 0;
    LED = 0;
    while(1)
    {
       CVD_Touch_Sample();
       __delay_ms(10);
    }
    return;
}
//====================================
void ADC_Init()
{
   TRISA = 0xFF;
   ADCON1 = 0x0A;	// Vref+ = Vdd & Vref- = Vss & AN0:AN4 Only Analog
   ADCON2 = 0x3E;	// Left Justification & 20Tad & Fosc/64
   ADCON0 = 0x01;   // Select Vdd Channel (AN0)
   __delay_us(30);
}
void UART_TX_Init()
{
  BRGH = 1; 
  SPBRG = 155;  // Baud Rate = 19200 bps
  //--[ Enable The Asynchronous Serial Port ]--
  SYNC = 0;
  SPEN = 1;
  BRG16 = 0;   // 8-Bit Mode
  //--[ Set The RX-TX Pins to be in UART mode (not io) ]--
  TRISC6 = 0;  // As stated in the data sheet
  TRISC7 = 1;  // As stated in the data sheet
  TXEN = 1;    // Enable UART Transmission   
}
void UART_Write(uint8_t data)
{
  while(!TRMT);
  TXREG = data;
}
void UART_Write_String(uint8_t *text)
{
  uint16_t i;
  for(i=0;text[i]!='\0';i++)
    UART_Write(text[i]);
}

void CVD_Touch_Sample()
{
    ADRESH = 0x00;
    ADRESL = 0x00;
    ADCON0 = 0x01;   // Select Vdd Channel (AN0)
    SensorLine_D = 1;
    ADCON0 = 0x05;   // Select Sensor Channel (AN1)
    GO_nDONE = 1; 
    while(GO_nDONE);
    AD_RES = (ADRESH<<2)+(ADRESL>>6);
    sprintf(Buffer, "%d \n\r", AD_RES);
    UART_Write_String(Buffer);
    if(AD_RES < 200)
    {
        LED = ~LED;
        __delay_ms(200);
    }
    ADCON0 = 0x01;   // Select Vdd Channel (AN0)
    SensorLine_D = 0;
    SensorLine = 0;
}