PRG            = bigmotor
OBJ            = bigmotor_main.o can_msg.o calibrations.o motor.o pot.o encoder.o OS_Dispatch.o adc.o
CORE_OBJS	   = can.o can_instance.o can_eid.o can_board_msg.o can_buff.o \
				 leds.o pwm.o bk_system_defs.o OS_timers.o configuration.o
MCU_TARGET     = atmega16m1
OPTIMIZE       = -O2
INC			   = ../core/
# path back to here for core modules:
SELF_INC	   = ../BigMotorEn/
DEFS           =
LIBS           =

# You should not have to change anything below here.
CC             = avr-g++

# Override is only needed by avr-lib build system.
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)  $(addprefix ../core/,$(CORE_OBJS))
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# dependency:
#OBJS = tilt_main.c spi.c accelerometer.c gyro.c magnetometer.c
../core/%.o: ../core/%.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -I$(SELF_INC) -c -o  $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	rm -rf ../core/*.o

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images
text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images
eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
	|| { echo empty $@ not generated; exit 0; }

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@

