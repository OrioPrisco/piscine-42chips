#ifndef TIMER_H
#define TIMER_H

#define CLOCK1_MASK  0b111
#define CLOCK1_0    0b000
#define CLOCK1_1    0b001
#define CLOCK1_8    0b010
#define CLOCK1_64   0b011
#define CLOCK1_256  0b100
#define CLOCK1_1024 0b101

#define FREQUENCY_REG(prescaler, frequency) ((F_CPU)/((2 * (prescaler) * (frequency))))

//helper to determine best clock for a giver frequency ?

#endif
