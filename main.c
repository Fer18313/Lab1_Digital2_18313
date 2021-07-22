/*
 * File:   lab1.c
 * Author: Fernando Sandoval
 * CARNE: 18313
 * DIGITAL 2
 *
 * Created on July 21, 2021, 9:54 AM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
#define _XTAL_FREQ 4000000 

#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "adc.h"
#include "segment.h"

// VAR
uint8_t UNIT = 0;
uint8_t DEC = 0;
uint8_t ADC_R;
uint8_t x = 0;
uint8_t y = 0;

//FUNCTION PROTOTYPES
void initCONFIG(void);
void configTMR0(void); 
// MAIN PROGRAM CODE
void main(void) {
    initCONFIG();      // CALL FUNCTION FOR PIC SETUP
    ADCON0bits.GO = 1; // START ADC READING
    configTMR0();
    while (1){         // WHILE ALWAYS RUNS
        ADC_START();
        x = ADC_R & 0x0F;
        y = (ADC_R >> 4) & 0x0F;
        UNIT = SEG_HEX(x);
        DEC = SEG_HEX(y);
        PORTEbits.RE2 = (ADC_R > PORTC)?  1:0; // THIS HOW WE CONTROL THE OVERFLOW ALARM
        }
    //}
    return;
}

// INTERRUPT CODE
void __interrupt() ISR(void){
    if (INTCONbits.RBIF){
        if (PORTBbits.RB5 == 0){
            PORTC++;
        }
        if (PORTBbits.RB6 == 0){
            PORTC--;
        }
        INTCONbits.RBIF = 0;
    }
    if (INTCONbits.T0IF){
        PORTE = 0;
        if (PORTAbits.RA1 == 0){
            PORTD = UNIT;
            PORTEbits.RE0 = 1;
            PORTAbits.RA1 = 1;
        }
        else if(PORTAbits.RA1 == 1){
            PORTD = DEC;
            PORTEbits.RE1 = 1;
            PORTAbits.RA1 = 0;
        }
        TMR0 = 217;
        INTCONbits.T0IF = 0;
    }
        if(PIR1bits.ADIF){
            ADC_R = ADRESH;
            PIR1bits.ADIF = 0;
        }
    return;
}

// FUNCTIONS
void initCONFIG(){
    OSCCONbits.IRCF2 = 1; // 4MHZ
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCCONbits.SCS = 1;
    // CONFIG ADC
    ADCON1bits.ADFM=0;
    ADCON1bits.VCFG0=0;
    ADCON1bits.VCFG1=0;
    ADCON0bits.ADCS= 0b010;
    ADCON0bits.CHS = 0;
    __delay_us(50); // ESPERAMOS UN TIEMPO PARA EL CAPACITOR
    ADCON0bits.ADON = 1;
    
    // MAIN INTERRUPTIONS
    INTCONbits.GIE = 1;
    INTCONbits.PEIE =1;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF =0;
    
    // ADC INTERRUPT 
    PIE1bits.ADIE=1;
    PIR1bits.ADIF=0;
    
    // PORT B INTERRUPT
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    IOCB = 0b01100000;
    TRISA = 0b00000001; // READING: ADC // PORT: AN0/RA0
    TRISB = 0b01100000; // INPUT PUSHBUTTON: RB5> UP , RB6> DOWN
    TRISC = 0;          // LED OUTPUT 
    TRISD = 0;          // 7 SEGMENT OUTPUT
    TRISE = 0;          // OUTPUT FOR SIGNAL CHANGE RE0 > UNITS | RE1 > CENTS
                        // RE2 > ALARM
    PORTA = 0;          
    PORTB = 0b01100000;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;          
    ANSELH = 0;
    ANSEL = 0b00000001; // ACTIVAMOS EL PUERTO ANALOGICO 0
    OPTION_REGbits.nRBPU = 0; // PARA FACILITARNOS HAREMOS USO DE LA FUNCIONALIDAD DE PULL UP DEL PIC EN EL PUERTO B
    WPUB = 0b01100000;
    return;
}

void configTMR0(){
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    TMR0 = 217;     // TMR0 CALCULADO PARA 5 ms
    return;
}