# Written by Burton Sumner with help from Tonc Library (https://www.coronac.com/tonc/text/setup.htm)


# If project name not explicitly specified during make command execution, use default name, "main."
ifeq '$(PROJ_NAME)' ''
	PROJ_NAME=$(shell pwd | sed 's-[a-zA-Z/]*/--g')
endif

#-------------------------------Project Files and Dirs-----------------------------------------------------

TARGET=$(PROJ_NAME)
SRC=./src
IWRAM_SRC=./iwsrc
BIN=./bin
INC=./include

ROM_C_OBJS=$(shell find $(SRC) -type f -iname '*.c' | sed 's-\./src-\./bin-g' | sed 's/\.c/\.o/g')
ROM_CXX_OBJS=$(shell find $(SRC) -type f -iname '*.cpp' | sed 's-\./src-\./bin-g' | sed 's/\.cpp/\.o/g')
IWRAM_C_OBJS=$(shell find $(IWRAM_SRC) -type f -iname '*.c' | sed 's-\./iwsrc-\./bin-g' | sed 's/\.c/\.o/g')
IWRAM_CXX_OBJS=$(shell find $(IWRAM_SRC) -type f -iname '*.cpp' | sed 's-\./iwsrc-\./bin-g' | sed 's/\.cpp/\.o/g')
C_OBJS=$(ROM_C_OBJS) $(IWRAM_C_OBJS)
CXX_OBJS=$(ROM_CXX_OBJS) $(IWRAM_CXX_OBJS)

#-------------------------------Build Definitions----------------------------------------------------------

PREFIX=arm-none-eabi-
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
LD=$(PREFIX)g++
OBJ_CPY=$(PREFIX)objcopy

ARCH=-mthumb-interwork -mthumb
IARCH=-mthumb-interwork -march
SPECS=-specs=gba.specs
CFLAGS_BASE=-O2 -Wall -Wextra -fno-strict-aliasing -I$(INC)
ROM_CFLAGS=$(CFLAGS_BASE) $(ARCH)
IWRAM_CFLAGS=$(CFLAGS_BASE) $(IARCH) -mlong-calls
LDFLAGS=$(ARCH) $(SPECS)

.PHONY: build clean

#-------------------------------Build----------------------------------------------------------------------

.SILENT:

test: clean build
	mgba-qt $(BIN)/$(TARGET).gba

build: $(IWRAM_SRC) $(SRC) $(BIN) $(TARGET).gba

$(IWRAM_SRC) $(SRC) $(BIN):
	mkdir -p $@

$(TARGET).gba: $(TARGET).elf
	$(OBJ_CPY) -v -O binary $(BIN)/$< $(BIN)/$@
	-@gbafix $(BIN)/$@

$(TARGET).elf: $(C_OBJS) $(CXX_OBJS)
	$(LD) $^ $(LDFLAGS) -o $(BIN)/$@



$(ROM_CXX_OBJS): $(BIN)/%.o : $(SRC)/%.cpp
	$(CXX) -c $< $(ROM_CFLAGS) -o $@

$(ROM_C_OBJS): $(BIN)/%.o : $(SRC)/%.c
	$(CC) -c $< $(ROM_CFLAGS) -o $@

$(IWRAM_C_OBJS): $(BIN)/%.o : $(IWRAM_SRC)/%.c
	$(CC) -c $< $(IWRAM_CFLAGS) -o $@

$(IWRAM_CXX_OBJS): $(BIN)/%.o : $(IWRAM_SRC)/%.cpp
	$(CC) -c $< $(IWRAM_CFLAGS) -o $@

#-------------------------------Build----------------------------------------------------------------------
clean: 
	@rm -fv $(BIN)/*.elf
	@rm -fv $(BIN)/*.o
	@rm -fv $(BIN)/*.gba

clean_saves:
	@rm -fv $(BIN)/*.sav
