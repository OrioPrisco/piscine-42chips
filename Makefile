# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: OrioPrisco <47635210+OrioPrisco@users      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/27 13:19:21 by OrioPrisc         #+#    #+#              #
#    Updated: 2023/03/27 15:32:20 by OrioPrisc        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS		=	aht20.c\
				uart.c\
				i2c.c\
				pca9555.c\
				gpio.c\
				utils.c\
				adc.c\
				spi.c\
				pcf8563.c\

MAIN		=	rush01/rush01.c
INCLUDE_DIR	=	.
CC			=	avr-gcc
OBJCOPY		=	avr-objcopy
CPU			=	m328p
MCU			=	atmega328p
BAUD_RATE	=	115200
F_CPU		=	16000000
PROGRAMMER	=	arduino
PORT		=	/dev/ttyUSB0
CFLAGS		=	-O2 -flto -g -Wall -Werror -Wextra

OBJS		:=	$(patsubst %.c,%.o,$(SRCS))
OBJ_MAIN	:=	$(patsubst %.c,%.o,$(MAIN))
HEX_MAIN	:=	$(patsubst %.c,%.hex,$(MAIN))
BIN_MAIN	:=	$(patsubst %.c,%.bin,$(MAIN))

all: hex flash

hex : $(HEX_MAIN)

%.hex: %.bin
	$(OBJCOPY) -O ihex $< $@

flash: $(HEX_MAIN)
	avrdude -p $(CPU) -c $(PROGRAMMER) -b $(BAUD_RATE) -P $(PORT) -U flash:w:$<:i

%.o: %.c Makefile
	$(CC) -c $< -o $@ $(addprefix -I,$(INCLUDE_DIR)) -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -DBAUD=$(BAUD_RATE)UL $(CFLAGS)


$(BIN_MAIN) : $(OBJS) $(OBJ_MAIN) Makefile
	$(CC) $(OBJS) $(OBJ_MAIN) -o $@ -mmcu=$(MCU) $(CFLAGS)

clean:
	rm -f main.hex main.bin $(OBJS)

.PHONY: all hex flash clean
.SUFFIXES:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules
