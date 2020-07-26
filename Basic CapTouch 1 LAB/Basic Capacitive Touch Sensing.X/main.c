/*
 * File:   main.c
 * Author: Khaled Magdy
 */
#include <xc.h>
#include "config.h"
#include <stdint.h>
#include <stdio.h>

#define _XTAL_FREQ       48000000
#define LED_D            TRISB0
#define LED              RB0
#define CHRG_PIN         RC2
#define CHRG_PIN_D       TRISC2
#define TouchSensePin1   RC1
#define TouchSensePin1_D TRISC1
#define THR              1500

char Buffer[10];
uint8_t TMR1_C=0;
uint32_t Ticks=0;
//-------------------------------------
void TMR1_Init(void);
void UART_TX_Init(void);
void UART_Write(uint8_t data);
void UART_Write_String(uint8_t *text);
void Touch_Sense();
//-------------------------------------
void main(void) {
    
    LED_D = 0;
    LED = 0;
    CHRG_PIN_D = 0;
    CHRG_PIN   = 0;
    TouchSensePin1_D = 1;
    TMR1_Init();
    UART_TX_Init();

    while(1)
    { 
       Touch_Sense();
       __delay_ms(10);
    }
    return;
}
//=================================================================
void __interrupt() ISR(void)
{
    if(TMR1IF)
    {
        TMR1_C++;
        TMR1IF = 0;
    }
}
//=================================================================
void UART_TX_Init(void)
{
  BRGH = 1; 
  SPBRG = 51;  // Baud Rate = 57600 bps
  //--[ Enable The Asynchronous Serial Port ]--
  SYNC = 0;
  SPEN = 1;
  BRG16 = 0; // 8-Bit Mode
  //--[ Set The RX-TX Pins to be in UART mode (not io) ]--
  TRISC6 = 0;  // As stated in the data sheet
  TRISC7 = 1;  // As stated in the data sheet
  TXEN = 1; // Enable UART Transmission   
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
void TMR1_Init(void)
{
  // -- [[ Configure Timer1 To Operate In Timer Mode ]] --
  // Clear The Timer1 Register. To start counting from 0
  TMR1 = 0;
  // Choose the local clock source (timer mode)
  TMR1CS = 0;
  // Choose the desired prescaler ratio (1:1)
  T1CKPS0 = 0;
  T1CKPS1 = 0;
  // Switch OFF Timer1 Module!
  TMR1ON = 0;
  // Config TMR1 Interrupt
  TMR1IE = 1;
  TMR1IF = 0;
  PEIE = 1;
  GIE = 1;
}
void Touch_Sense()
{
    TMR1L = 0x00;
    TMR1H = 0x00;
    CHRG_PIN = 1;
    TMR1ON = 1;
    while(!TouchSensePin1);
    TMR1ON = 0;
    Ticks = TMR1;
    if(TMR1_C)
    {
        Ticks += (TMR1_C*65536);
        TMR1_C = 0;
    }
    sprintf(Buffer, "%d\n\r", Ticks);
    UART_Write_String(Buffer);
    if(Ticks>THR)
    {
        LED = ~LED;
        __delay_ms(200);
    }
    CHRG_PIN = 0;
}
