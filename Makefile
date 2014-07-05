GCC = nspire-gcc
GPP = nspire-g++
LD = nspire-ld
GENZEHN = genzehn
OPTIMIZE ?= fast
GCCFLAGS = -O$(OPTIMIZE) -g -Wall -W -marm -ffast-math -mcpu=arm926ej-s -fno-math-errno -fomit-frame-pointer -flto -fno-rtti -fgcse-sm -fgcse-las -funsafe-loop-optimizations -fno-fat-lto-objects -frename-registers -fprefetch-loop-arrays
LDFLAGS = -lm
EXE = crafti
OBJS = $(patsubst %.c, %.o, $(shell find . -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find . -name \*.S))

all: $(EXE).tns $(EXE).prg.tns

%.o: %.cpp
	$(GPP) -std=c++11 $(GCCFLAGS) -c $< -o $@

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

%.o: %.S
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(EXE).elf: $(OBJS)
	+$(LD) $^ -o $@ $(GCCFLAGS) $(LDFLAGS)

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $^ --output $@ --name "Crafti" --version 10 --author "Fabian Vogt" --notice "3D Minecraft"

$(EXE).prg.tns: $(EXE).tns
	cat /opt/nspire/Ndless/ndless/src/resources-loader/zehn_loader.tns $^ > $@

.PHONY: clean
clean:
	rm -f `find . -name \*.o`
	rm -f $(EXE).tns $(EXE).prg.tns $(EXE).elf
