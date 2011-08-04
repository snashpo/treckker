NAME   = stm103_stk_blink_rom

CC      = arm-elf-gcc
LD      = arm-elf-ld -v
AR      = arm-elf-ar
AS      = arm-elf-as
CP      = arm-elf-objcopy
OD		  = arm-elf-objdump

CFLAGS  = -c -g  -I./ -I./include -Os -std=gnu99 
CFLAGS  += -mcpu=cortex-m3 -mthumb -mthumb-interwork -fno-common -fomit-frame-pointer 
CFLAGS  += -Wunused -pedantic -Wimplicit -Wpointer-arith 
CFLAGS  += -Wredundant-decls -Wcast-qual -Wcast-align -Wshadow  
CFLAGS  += -lm
CFLAGS  += -DDEBUG -DUSE_STDPERIPH_DRIVER -DPRINTF_BUFFER_SIZE=128 -DSTM32_SD_USE_DMA
AFLAGS  = -ahls -mapcs-32 -o crt.o
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
			sht1x.o

			#--------------------------------------------------
			# 			# LSM303.o \
			# vector.o \
			#-------------------------------------------------- 
					

LSOURCES        = $(patsubst %.o,%.c,$(LOBJECTS))
CSOURCES        = $(patsubst %.o,%.c,$(COBJECTS))
SSOURCES        = $(patsubst %.o,%.s,$(COBJECTS))

.SUFFIXES: .o

.c.o:
	@ echo ".compiling"
	$(CC) $(CFLAGS) $<

.s.o:
	@ echo ".compiling"
	$(CC) $(CFLAGS) $<

all: stm32_rtc


clean:
	-rm -f *~ main.dist main.list main.dis main.out main.hex main.map main.bin makefile.dep *.o .*.sw*

#--------------------------------------------------
# stm32.a: $(LOBJECTS)
# 	$(AR) $(ARLAGS) stm32.a $(LOBJECTS)
#-------------------------------------------------- 

stm32_rtc: main.out
	@ echo "...copying"
	$(CP) $(CPFLAGS) main.out main.bin
	$(OD) $(ODFLAGS) main.out > main.list
	$(OD) -d main.out > main.dis
	#--------------------------------------------------
	# ./Flash.sh
	#-------------------------------------------------- 

#--------------------------------------------------
# main.out: $(COBJECTS) stm32_flash.ld stm32.a
#-------------------------------------------------- 
main.out: $(COBJECTS) $(LOBJECTS) stm32_flash.ld 
	@ echo "..linking"
	$(LD) $(LFLAGS) -M -o main.out $(COBJECTS) $(LOBJECTS) > main.map
	#--------------------------------------------------
	# $(LD) $(LFLAGS) -M -o main.out $(COBJECTS) stm32.a > main.map
	#-------------------------------------------------- 


-include makefile.dep

dep: makefile.dep

makefile.dep: $(CSOURCES)
	rm -f makefile.dep
	for csrc in $(CSOURCES) ;\
	do \
	$(CC) $(CFLAGS) -MM $$csrc >>makefile.dep ;\
	done
