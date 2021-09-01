/* 
 * File:   ADC.h
 * Author: diego
 *
 * Created on 18 de julio de 2021
 */

#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#pragma config FOSC = INTRC_NOCLKOUT
#include <xc.h> // include processor files - each processor file is guarded.  

//defino el prototipo de la funcion
void config_ADC(char frec); //tomara el valor que deseamos de la frecuencia

#endif	/* XC_HEADER_TEMPLATE_H */