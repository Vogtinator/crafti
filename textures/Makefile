OBJS := font_bmp.h font_dat.h inventory.h inv_selection.h loading.h loadingtext.h menu.h selection.h terrain.h blockselection.h part_fire.h scrollbar.h
ConvertImg = ../nGL/tools/ConvertImg/ConvertImg/ConvertImg

all: $(OBJS)

%.h: %.bmp
	$(ConvertImg) --format=ngl $^ $@

%.h: %.png
	$(ConvertImg) --format=ngl $^ $@

inventory.png: gui.png
	convert -scale 512x512 -crop 200x42+0+1 $^ $@

inv_selection.png: gui.png
	convert -scale 512x512 -crop 46x46+1+44 $^ $@

part_fire.png: particles.png
	convert -crop 16x16+0+48 $^ $@

font_dat.h: font.dat
	xxd -i $^ > $@

clean:
	rm -f $(OBJS)
