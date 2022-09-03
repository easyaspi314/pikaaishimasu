ifeq (,$(ARMSDT))
$(error "Please define ARMSDT")
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

# todo: fromelf.exe if available
OBJCOPY := $(DEVKITARM)/bin/arm-none-eabi-objcopy
GBAFIX := $(DEVKITPRO)/tools/bin/gbafix

# -Wd disables "Warning: Deprecated declaration foo() - give arg types" triggered in the libGBA
# header. It still complains about an enum with a comma though.
ARMCFLAGS := -DONE_TRANSLATION_UNIT -Otime -O2 -arch 4T -I include -I . -I $(DEVKITPRO)/libgba/include -Wd

ARMLDFLAGS := -elf -ro-base 0x08000000 -rw-base 0x03000000 -first 'src/rom_header.o(Start)'

ARMLIB := $(ARMSDT)/lib/armlib_cn.32l
ROM := pikaaishimasu.gba
ELF := pikaaishimasu.elf

OBJS := src/rom_header.o src/main.o
HEADERS := include/constants.h include/global.h include/graphics.h include/stdbool.h include/variables.h 

DATA := $(wildcard graphics/*.bin*)
C_DATA := $(patsubst %.bin8,%.c,$(patsubst %.bin32,%.c,$(DATA)))
# For organization the source code is split but it is compiled together
GLOBBED_SRCS := src/global_variables.c src/rodata.c

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
	$(ARMLD) $(ARMLDFLAGS) -Output $@ src/rom_header.o src/main.o $(ARMLIB)

src/rom_header.o: src/rom_header.s
	$(ARMAS) $< $@

$(BIN2C): tools/bin2c.c
	$(CC) $(CFLAGS) $< -o $@

src/rodata.c: $(C_DATA)

%.c: %.bin8 $(BIN2C)
	$(BIN2C) $< $@ 8

%.c: %.bin32 $(BIN2C)
	$(BIN2C) $< $@ 32

src/main.o: src/main.c $(HEADERS) $(GLOBBED_SRCS)
	$(ARMCC) $(ARMCFLAGS) -c $< -o $@

clean:
	$(RM) $(ROM) $(ELF) $(OBJS) $(BIN2C) $(C_DATA)

.PHONY: clean
