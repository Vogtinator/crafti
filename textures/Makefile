OBJS := font_bmp.h font_dat.h inventory.h inv_selection.h loading.h loadingtext.h menu.h selection.h terrain.h blockselection.h

all: $(OBJS)

%.h: %.bmp
	ConvertImg --format=ngl $^ $@

%.h: %.png
	ConvertImg --format=ngl $^ $@

inventory.png: gui.png
	convert -scale 420x420 -crop 168x37+0+0 $^ $@

inv_selection.png: gui.png
	convert -scale 420x420 -crop 38x37+0+37 $^ $@

font_dat.h: font.dat
	xxd -i $^ > $@

clean:
	rm -f $(OBJS)
