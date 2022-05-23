GCC = nspire-gcc
GPP = nspire-g++
LD = nspire-ld
GENZEHN = genzehn
OPTIMIZE ?= fast
GCCFLAGS = -O$(OPTIMIZE) -I nGL -I . -Wall -W -marm -ffast-math -mcpu=arm926ej-s -fno-math-errno -fomit-frame-pointer -flto -fno-rtti -fgcse-sm -fgcse-las -funsafe-loop-optimizations -fno-fat-lto-objects -frename-registers -fprefetch-loop-arrays -Wold-style-cast -mno-thumb-interwork -ffunction-sections -fdata-sections -fno-exceptions -D NDEBUG
LDFLAGS = -lm -Wl,--gc-sections
ZEHNFLAGS = --name "Crafti" --version 12 --author "Fabian Vogt" --notice "3D Minecraft" --compress
EXE = crafti
OBJS = $(patsubst %.c, %.o, $(shell find . -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find . -name \*.S))

all: $(EXE).tns

%.o: %.cpp
	@echo Compiling $<...
	@$(GPP) -std=c++11 $(GCCFLAGS) -c $< -o $@

$(EXE).elf: $(OBJS)
	+$(LD) $^ -o $@ $(GCCFLAGS) $(LDFLAGS)

$(EXE).tns: $(EXE).elf
	+$(GENZEHN) --input $^ --output $@.zehn $(ZEHNFLAGS)
	+make-prg $@.zehn $@
	+rm $@.zehn

.PHONY: clean
clean:
	rm -f `find . -name \*.o`
	rm -f $(EXE).tns $(EXE).elf
