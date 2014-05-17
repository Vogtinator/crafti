OBJS := font.h font_dat.h inventory.h loading.h loadingtext.h menu.h selection.h terrain.h

all: $(OBJS)

%.h: %.bmp
	ConvertImg $^ > $@

%.h: %.png
	ConvertImg $^ > $@

font_dat.h: font.dat
	xxd -i $^ > $@

clean:
	rm -f $(OBJS)