OBJS := font_bmp.h font_dat.h inventory.h loading.h loadingtext.h menu.h selection.h terrain.h blockselection.h

all: $(OBJS)

%.h: %.bmp
	ConvertImg --format=ngl $^ $@

%.h: %.png
	ConvertImg --format=ngl $^ $@

font_dat.h: font.dat
	xxd -i $^ > $@

clean:
	rm -f $(OBJS)
