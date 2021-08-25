/*
 * File:   main_slave1.c
 * Author: Diego Estrada 19264, Jonathan Menéndez 18023.
 *
 * Created on 20 de agosto de 2021, 10:36 AM
 */

//------------------------------- LIBRERIAS ------------------------------------
#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdint.h>
#include <stdio.h>  // Para usar printf

// Librerias propias
#include "I2C.h"


//-------------------------- Bits de configuraciÓn -----------------------------
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT            // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF                       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF                      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF                      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF                         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF                        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF                      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF                       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF                      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF                        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V                   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF                        // Flash Program Memory Self Write Enable bits (Write protection off)

//--------------------------------- Variables ----------------------------------
uint8_t z;
uint8_t dato1;

//-------------------------------- Prototipos ----------------------------------
void setup(void);

//------------------------------ Interrupciones --------------------------------
void __interrupt() isr(void){  
    // Interrupcion del Puerto B
    if (RBIF == 1)                              // Verificar bandera de la interrupcion del puerto b
    {
        if (PORTBbits.RB0 == 0)                 // Si oprimo el boton 1
        {
            PORTD = 0b00000011;    
            __delay_ms(500);
            PORTD = 0b00000110;    
            __delay_ms(500);
            PORTD = 0b00001100;    
            __delay_ms(500);
            PORTD = 0b00001001;    
            __delay_ms(500);
            dato1 = 1;
        }
        else if (PORTBbits.RB0 == 1)
        {
            PORTD = 0;
            dato1 = 0;
        }
        INTCONbits.RBIF = 0;                    // Se limpia la bandera de la interrupcion
    }
    
    // Interrupción I2C
    if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                         // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;               // Clear the overflow flag
            SSPCONbits.WCOL = 0;                // Clear the collision bit
            SSPCONbits.CKP = 1;                 // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            z = SSPBUF;                         // Lectura del SSBUF para limpiar el buffer y la bandera BF
            PIR1bits.SSPIF = 0;                 // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;                 // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);             // Espera a que la recepción se complete
            dato1 = SSPBUF;                      // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = dato1;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}
//----------------------------------- Main -------------------------------------
void main(void) {
    setup();

    while(1){
        
             
    }
    return;
}
//--------------------------------- Funciones ----------------------------------
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISBbits.TRISB0 = 1;
    TRISD = 0;
    
    //limpiar puertos
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    
    //Configurar reloj interno
    OSCCONbits.IRCF0 = 0;                       // 4mhz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.SCS = 1; 
    
    // Configuracion de pull-up interno
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0b00000001;
    IOCBbits.IOCB0 = 1;
    
    //configuracion de interrupciones                       
    INTCONbits.GIE = 1;                         
    INTCONbits.PEIE = 1; 
    INTCONbits.RBIF = 1;    
    INTCONbits.RBIE = 1;   
    
    // I2C configuracion esclavo
    I2C_Slave_Init(0x50);   
    
}