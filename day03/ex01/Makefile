# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: OrioPrisco <47635210+OrioPrisco@users      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/27 12:55:53 by OrioPrisc         #+#    #+#              #
#    Updated: 2023/03/27 12:55:53 by OrioPrisc        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS		= main.c
NAME		= main.hex
CC			= avr-gcc
OBJCOPY		= avr-objcopy
CPU			= m328p
MCU			= atmega328p
BAUD_RATE	= 115200
F_CPU		= 16000000UL
PROGRAMMER	= arduino
PORT		= /dev/ttyUSB0
CFLAGS		= -O2

all: hex flash

hex : main.hex

main.hex: main.bin
	$(OBJCOPY) -O ihex $< $(NAME)

flash: $(NAME)
	avrdude -p $(CPU) -c $(PROGRAMMER) -b $(BAUD_RATE) -P $(PORT) -U flash:w:$(NAME)

main.bin: main.c Makefile
	$(CC) -g $< -o $@ -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(CFLAGS)

clean:
	rm -f main.hex main.bin

.PHONY: all hex flash clean
