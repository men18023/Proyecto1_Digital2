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
#include "LCD.h"

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

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RB5
#define D6 RD6
#define D7 RD7
//--------------------------------- Variables ----------------------------------
uint8_t R1, R2;
char temp, stat, cont;
char valor, centenas, residuo, decenas, unidades;
char cen, dec, uni;

//-------------------------------- Prototipos ----------------------------------
void setup(void);
void Text(void);
char division (char valor);

//------------------------------ Interrupciones --------------------------------
void __interrupt() isr(void){  
    // Interrupcion del Puerto B
    if (RBIF == 1)                              // Verificar bandera de la interrupcion del puerto b
    {
        if (PORTBbits.RB0 == 0)                 // Si oprimo el boton 1
        {
            PORTDbits.RD0 = 1;    
            stat = 1;
        }
        else if (PORTBbits.RB0 == 1)
        {
            PORTDbits.RD0 = 0;
            stat = 0;
        }
        INTCONbits.RBIF = 0;                    // Se limpia la bandera de la interrupcion
    }
}

//----------------------------------- Main -------------------------------------
void main(void) {
    
    setup();                                    // Configuración principal
    Lcd_Init();                                 // LCD esta apagada
    char buffer[20];                            // Se guarda el voltaje en un string
    char buffer1[20];
    char buffer2[20];
    char val2;
    char val1;
    char val;                                   // Valor que deseo almacenar en un str
    
    while(1)  
    {
        temp = R2;
        cont = R1;
        val = cont; 
        val1 = stat;
        val2 = temp;
        
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("CONT: STAT: TEMP:");
        sprintf(buffer, "%d ", val); 
        sprintf(buffer2, "%d ", val2); 
        Lcd_Set_Cursor(2,2);
        Lcd_Write_String(buffer);
        Lcd_Set_Cursor(2,14);
        Lcd_Write_String(buffer2);
    
        if (val1==1){
            Lcd_Set_Cursor(2,8);
        Lcd_Write_String("ON");
        }
        else{
            Lcd_Set_Cursor(2,8);
        Lcd_Write_String("OFF");
        }
        __delay_ms(2000);
        
        //Obtener informacion del primer slave
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        R1 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        
        //Obtener informacion del segundo slave
        I2C_Master_Start();
        I2C_Master_Write(0x61);                 // 51, se escribe el 1 para que lea en el puerto de leds
        R2 = I2C_Master_Read(0);             
        I2C_Master_Stop();
        __delay_ms(200);
        
        Text();

    }
    return;
}

//--------------------------------- Funciones ----------------------------------
// Configuración principal
void setup(void){
    // Configuración reloj interno
    OSCCONbits.IRCF0 = 0;                       // 4mhz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.SCS = 1;  
    
    // Puertos digitales
    ANSEL = 0;
    ANSELH = 0;
    
    // Inputs y outputs de los puertos
    TRISBbits.TRISB0 = 1;
    TRISCbits.TRISC0 = 0;
    TRISD = 0x00;
    TRISA = 0x00;
        
    // Se limpian los puertos
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    
    // Configuracion de pull-up interno
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0b00000001;
    IOCBbits.IOCB0 = 1;

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

    // I2C configuracion Maestro
    I2C_Master_Init(100000);                    // Inicia comuncación I2C
}

// Función para escribir en el UART
void Text(void){
    __delay_ms(50);                           //Tiempos para el despliegue de los caracteres
     division(cont);
    __delay_ms(50);
    TXREG = decenas;
    __delay_ms(50);
    TXREG = unidades;
    __delay_ms(50);
    //printf("\r");
    
    __delay_ms(50);                           //Tiempos para el despliegue de los caracteres
    division(stat);
    //printf("Valor del agua:\r");
    __delay_ms(50);
    TXREG = unidades;
    __delay_ms(50);
    //printf("\r");
    
    __delay_ms(50);                           //Tiempos para el despliegue de los caracteres
     division(temp);
    //printf("Valor del temperatura:\r");
    __delay_ms(50);
    TXREG = centenas;
    __delay_ms(50);
    TXREG = decenas;
    __delay_ms(50);
    TXREG = unidades;
    __delay_ms(50);
    //printf("\r");
}

// Función para despliegue de valores
char division (char valor){
    centenas = valor/100;
    residuo = valor%100; 
    decenas = residuo/10; 
    unidades = residuo%10; 
    // Se le suma 47 para que entregue el valor requerido
    centenas = centenas + 48;
    decenas = decenas + 48;
    unidades = unidades + 48;
} 

// Funcion de stdio.h
void putch(char data){      
    while(TXIF == 0);
    TXREG = data; 
    return;
}   
