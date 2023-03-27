#ifndef I2C_H
#define I2C_H

#include <util/twi.h>

#define SCL_FREQ 100000L //100Khz
#define SCL_PRESCALER 1
#define MY_TWBR (((F_CPU/SCL_FREQ*SCL_PRESCALER)-16)/2) //page 180
#define ACK 1
#define NACK 0

#define I2C_1  0b00
#define I2C_4  0b01
#define I2C_16 0b10
#define I2C_64 0b11

void error(unsigned char expected, unsigned char got);
void i2c_init();
void i2c_wait();
void i2c_assert_status(unsigned char check);
void i2c_assert_statuses(unsigned char check1, unsigned char check2);
void i2c_start(unsigned char sla_addr, unsigned char write_or_read);
void i2c_write(unsigned char data);
unsigned char i2c_read_n(int bytes, unsigned char *data, _Bool ack_at_end);
unsigned char i2c_read_status_ack(unsigned char status, _Bool ack);
unsigned char i2c_read_status(unsigned char status);
unsigned char i2c_read_ack(_Bool ack);
unsigned char i2c_read();
void i2c_stop();

#endif
