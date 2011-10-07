NAME    	=stm32_trecker


#--------------------------------------------------
# DIR		=/usr/local/stm32
# BIN_DIR	=$(DIR)/bin
# INC_DIR	=$(DIR)/include
# BASENAME =arm-none-eabi
#-------------------------------------------------- 

####################################################################
#				OLD CONFIGURATION
#
DIR		 =/opt/arm-cortex
BIN_DIR	 =$(DIR)/bin
INC_DIR	 =$(DIR)/include
BASENAME =arm-elf
####################################################################



CC      =$(BIN_DIR)/$(BASENAME)-gcc
LD      =$(BIN_DIR)/$(BASENAME)-gcc
#LD      =$(BIN_DIR)/$(BASENAME)-ld -v 
AR      =$(BIN_DIR)/$(BASENAME)-ar
AS      =$(BIN_DIR)/$(BASENAME)-as
CP      =$(BIN_DIR)/$(BASENAME)-objcopy
OD		  =$(BIN_DIR)/$(BASENAME)-objdump

CFLAGS  = -c -g -I./ -I./include -I$(INC_DIR) -Os -std=gnu99 
CFLAGS  += -mcpu=cortex-m3 -mthumb -fno-common -mthumb-interwork
CFLAGS  += -Wunused -pedantic -Wimplicit -Wpointer-arith 
CFLAGS  += -Wredundant-decls -Wcast-qual -Wcast-align -Wshadow  
CFLAGS  += -DDEBUG -DUSE_STDPERIPH_DRIVER -DPRINTF_BUFFER_SIZE=128 -DSTM32_SD_USE_DMA

AFLAGS  = -ahls -mapcs-32 -o crt.o -mthumb
LFLAGS  = -Tstm32_flash.ld -nostartfiles 
CPFLAGS = -Obinary
ODFLAGS = -S
ARLAGS  = -rcs 

LOBJECTS =  core_cm3.o \
			stm32f10x_rcc.o \
			stm32f10x_bkp.o \
			stm32f10x_pwr.o \
			stm32f10x_spi.o \
			stm32f10x_gpio.o \
			stm32f10x_usart.o \
			stm32f10x_exti.o \
			stm32f10x_adc.o \
			stm32f10x_tim.o \
			stm32f10x_dma.o \
			stm32f10x_flash.o \
			stm32f10x_wwdg.o \
			stm32f10x_rtc.o \
			stm32f10x_i2c.o \
			stm32f10x_it.o

COBJECTS = printf.o \
			main.o \
			timer.o \
			clock_calendar.o \
			hw_config.o \
			system_stm32f10x.o \
			misc.o \
			startup_stm32f10x_md.o \
			eeprom.o \
			button.o	\
			buzzer.o \
			lcd.o \
			crc.o \
			battery.o \
			sim18.o \
			nmea.o \
			sirf.o \
			tools.o \
			MS5607.o \
			ccsbcs.o \
			fattime.o \
			ff.o \
			rtc.o \
			sd_spi_stm32.o \
			sht1x.o \
			LSM303.o \
			vector.o 
					
LSOURCES        = $(patsubst %.o,%.c,$(LOBJECTS))
CSOURCES        = $(patsubst %.o,%.c,$(COBJECTS))
SSOURCES        = $(patsubst %.o,%.s,$(COBJECTS))

.SUFFIXES: .o

.c.o:
	@ echo ".compiling $<"
	$(CC) $(CFLAGS) $<

.s.o:
	@ echo ".compiling  $<"
	$(CC) $(CFLAGS) $<

all: $(NAME)

clean:
	-rm -f *~ main.dist main.list main.dis main.out main.hex main.map main.bin makefile.dep *.o .*.sw*

stm32.a: $(LOBJECTS)
	$(AR) $(ARLAGS) stm32.a $(LOBJECTS)

$(NAME): main.out
	@ echo "...copying"
	$(CP) $(CPFLAGS) main.out main.bin
	$(CP) $(CPFLAGS) main.bin $(NAME).bin
	$(OD) $(ODFLAGS) main.out > main.list
	$(OD) -d main.out > main.dis
	#--------------------------------------------------
	# ./Flash.sh
	#-------------------------------------------------- 

main.out: $(COBJECTS) stm32_flash.ld stm32.a
	@ echo "..linking"
	$(LD) $(LFLAGS) -o main.out $(COBJECTS) stm32.a -lm > main.map

-include makefile.dep

dep: makefile.dep

makefile.dep: $(CSOURCES)
	rm -f makefile.dep
	for csrc in $(CSOURCES) ;\
	do \
		$(CC) $(CFLAGS) -MM $$csrc >>makefile.dep ;\
	done
