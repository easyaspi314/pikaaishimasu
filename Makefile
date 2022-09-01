ifeq (,$(ARMSDT))
$(error "Please define ARMSDT to point to SDT 2.51")
endif

ifeq (,$(DEVKITPRO))
$(error "Please define DEVKITPRO")
endif

ifneq (Windows_NT,$(OS))
WINE := wine
EXE :=
else
WINE :=
EXE := .exe
endif

ARMCC := $(WINE) $(ARMSDT)/Bin/armcc.exe
ARMAS := $(WINE) $(ARMSDT)/Bin/armasm.exe
ARMLD := $(WINE) $(ARMSDT)/Bin/armlink.exe

OBJCOPY := $(DEVKITARM)/bin/arm-none-eabi-objcopy
GBAFIX := $(DEVKITPRO)/tools/bin/gbafix

# -Wd disables "Warning: Deprecated declaration foo() - give arg types" triggered in the libGBA
# header. It still complains about an enum with a comma though.
ARMCFLAGS := -via options.txt -Otime -O2 -arch 4T -I include -I . -I $(DEVKITPRO)/libgba/include -Wd

ARMLDFLAGS := -elf -ro-base 0x08000000 -rw-base 0x03000000 -first 'src/rom_header.o(Start)'

ROM := pikaaishimasu.gba
ELF := pikaaishimasu.elf

OBJS := src/rom_header.o src/main.o
DATA := $(wildcard graphics/*.bin*)
C_DATA := $(patsubst %.bin8,%.c,$(patsubst %.bin32,%.c,$(DATA)))

BIN2C := tools/bin2c$(EXE)

ifneq ($(BUGFIX),)
        ARMCFLAGS += -DBUGFIX
endif

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@ || $(RM) $@
ifneq ($(BUGFIX),)
	# Fix the header
	$(GBAFIX) -t"Rinen Rulez" -m01 -r11 $@ || $(RM) $@
endif

$(ELF): $(OBJS)
	$(ARMLD) $(ARMLDFLAGS) -Output $@ src/rom_header.o src/main.o $(ARMSDT)/lib/armlib_cn.32l

src/rom_header.o: src/rom_header.s
	$(ARMAS) $< $@

$(BIN2C): tools/bin2c.c
	$(CC) $(CFLAGS) $< -o $@

src/rodata.c: $(C_DATA)

%.c: %.bin8 $(BIN2C)
	$(BIN2C) $< $@ 8

%.c: %.bin32 $(BIN2C)
	$(BIN2C) $< $@ 32

src/main.o: src/main.c src/rodata.c src/global_variables.c include/global.h
	$(ARMCC) $(ARMCFLAGS) -c $< -o $@

clean:
	$(RM) $(ROM) $(ELF) $(OBJS) $(BIN2C) 

.PHONY: clean
