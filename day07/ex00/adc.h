#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

#define ADC_POT 0b0000
#define ADC_LDR 0b0001
#define ADC_NTC 0b0010
#define ADC_INT 0b0011
#define ADC_TMP 0b1000
#define ADC_GND 0b1111
#define ADC_1V1 0b1110

void adc_init();
void adc_select_device(char address);
void adc_trigger_measure();
void adc_wait();
char adc_measure();

#endif
