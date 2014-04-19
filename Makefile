GCC = nspire-gcc
GPP = nspire-g++
LD = nspire-ld
OPTIMIZE ?= 3
GCCFLAGS = -O$(OPTIMIZE) -g -Wall -W -marm -ffast-math -mcpu=arm926ej-s -fno-math-errno -fno-tree-vectorize -fomit-frame-pointer -flto
LDFLAGS = -lm -flto -Wl,-flto,-O$(OPTIMIZE) -O$(OPTIMIZE) -g
EXE = crafti.tns
OBJS = $(patsubst %.c, %.o, $(shell find . -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find . -name \*.S))

all: $(EXE)

%.o: %.cpp
	$(GPP) -std=c++11 $(GCCFLAGS) -c $< -o $@

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

%.o: %.S
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(LD) $^ -o $@ $(LDFLAGS)

clean:
	rm -f `find . -name \*.o` *.elf
	rm -f $(EXE) $(EXE).gdb
