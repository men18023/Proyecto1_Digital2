/*
 * File:   main_master.c
 * Author: Diego Estrada 19264, Jonathan Menéndez 18023.
 *
 * Created on 20 de agosto de 2021, 10:17 AM
 */

//------------------------------- LIBRERIAS ------------------------------------
#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdint.h>
#include <stdio.h>  // Para usar printf
#include <string.h> // Concatenar
#include <stdlib.h>

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


//-------------------------------- Prototipos ----------------------------------
void setup(void);

//------------------------------ Interrupciones --------------------------------


//----------------------------------- Main -------------------------------------
void main(void) {
    
    setup();                                    // Configuración principal

    while(1)  
    {
        //Obtener informacion del primer slave
        I2C_Master_Start();
        I2C_Master_Write(0x51);                 // 51, se escribe el 1 para que lea en el puerto de leds
        PORTB = I2C_Master_Read(0);             
        I2C_Master_Stop();
        __delay_ms(200);
        
        //Obtener informacion del segundo slave
        I2C_Master_Start();
        I2C_Master_Write(0x61);                 // 51, se escribe el 1 para que lea en el puerto de leds
        PORTA = I2C_Master_Read(0);             
        I2C_Master_Stop();
        __delay_ms(200);
    }
    return;
}

//--------------------------------- Funciones ----------------------------------
// Configuración principal
void setup(void){
    // Puertos digitales
    ANSEL = 0;
    ANSELH = 0;
    
    // Inputs y outputs de los puertos
    TRISD = 0;
    TRISE = 0;
        
    // Se limpian los puertos
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    
    // Configuración reloj interno
    OSCCONbits.IRCF0 = 0;                       // 4mhz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.SCS = 1;  
    
    // I2C configuracion Maestro
    I2C_Master_Init(100000);                    // Inicia comuncación I2C

    // Configuraciones TX y RX
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCTLbits.BRG16 = 0;
    
    SPBRG = 25;
    SPBRGH = 1;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    TXSTAbits.TX9 = 0;                          // 8 bits
    
    TXSTAbits.TXEN = 1;
    
    PIR1bits.RCIF = 0;  
    PIR1bits.TXIF = 0;  
    
    // Configuración de interrupciones
    INTCONbits.GIE = 1;     
    INTCONbits.PEIE = 1;    
    PIE1bits.RCIE = 0;      
    PIE1bits.TXIE = 0;      
    INTCONbits.RBIF = 1;    
    INTCONbits.RBIE = 1;    

}

// Funcion de stdio.h
void putch(char data){      
    while(TXIF == 0);
    TXREG = data; 
    return;
}   
