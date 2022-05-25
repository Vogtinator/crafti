GCC = nspire-gcc
GPP = nspire-g++
LD = nspire-ld
GENZEHN = genzehn
OPTIMIZE ?= fast
GCCFLAGS = -O$(OPTIMIZE) -I nGL -I . -Wall -W -marm -ffast-math -mcpu=arm926ej-s -fno-math-errno -fomit-frame-pointer -flto -fno-rtti -fgcse-sm -fgcse-las -funsafe-loop-optimizations -fno-fat-lto-objects -frename-registers -fprefetch-loop-arrays -Wold-style-cast -mno-thumb-interwork -ffunction-sections -fdata-sections -fno-exceptions -D NDEBUG $(if $(filter true,${DEBUG}),-DDEBUG -g)
LDFLAGS = -lm -Wl,--gc-sections
ZEHNFLAGS = --name "Crafti" --version 12 --author "Fabian Vogt" --notice "3D Minecraft" --compress

# release with "make all" debug with "make all DEBUG=true"
BUILDPATH=$(if $(filter true,${DEBUG}),debug,release)

EXE = $(BUILDPATH)/crafti
OBJS = $(patsubst ./%.c, ./$(BUILDPATH)/%.o, $(shell find . -name \*.c))
OBJS += $(patsubst ./%.cpp, ./$(BUILDPATH)/%.o, $(shell find . -name \*.cpp))
OBJS += $(patsubst ./%.S, ./$(BUILDPATH)/%.o, $(shell find . -name \*.S))

.PHONY: prep
prep:
	@mkdir -p $(BUILDPATH) $(BUILDPATH)/nGL

.PHONY: all
all: prep $(EXE).tns

test:
	@echo $(if $(filter true,${DEBUG}),-DDEBUG)

.PHONY: help
help:
	@echo run "make all" to build release
	@echo run "make all DEBUG=true" to build debug

./$(BUILDPATH)/%.o: ./%.cpp 
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
	rm -rf `find . -name \*.o  -o  -name \*.elf  -o  -name \*.tns` release/ debug/
