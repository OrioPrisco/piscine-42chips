# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: OrioPrisco <47635210+OrioPrisco@users      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/27 12:55:54 by OrioPrisc         #+#    #+#              #
#    Updated: 2023/03/27 12:55:54 by OrioPrisc        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS		=	main.c\
				aht20.c\
				uart.c\
				i2c.c\
				pca9555.c\
				gpio.c\
				utils.c\
				adc.c\

INCLUDE_DIR	=	.
CC			=	avr-gcc
OBJCOPY		=	avr-objcopy
CPU			=	m328p
MCU			=	atmega328p
BAUD_RATE	=	115200
F_CPU		=	16000000
PROGRAMMER	=	arduino
PORT		=	/dev/ttyUSB0
CFLAGS		=	-O2 -flto -g

OBJS		:=	$(patsubst %.c,%.o,$(SRCS))

all: hex flash

hex : main.hex

%.hex: %.bin
	$(OBJCOPY) -O ihex $< $@

flash: main.hex
	avrdude -p $(CPU) -c $(PROGRAMMER) -b $(BAUD_RATE) -P $(PORT) -U flash:w:$<:i

%.o: %.c Makefile
	$(CC) -c $< -o $@ -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -DBAUD=$(BAUD_RATE)UL $(CFLAGS)

main.bin : $(OBJS) Makefile
	$(CC) $(OBJS) -o $@ -mmcu=$(MCU) $(CFLAGS)

clean:
	rm -f main.hex main.bin $(OBJS)

.PHONY: all hex flash clean
.SUFFIXES:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules
