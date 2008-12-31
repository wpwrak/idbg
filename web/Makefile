ACCOUNT=werner@almesberger.net
DIR=/home/httpd/almesberger/misc/idbg/

IMAGES=mech.ps wiring.ps mech.png wiring.png idbg.png idbg-Component.png

FILES=index.html idbg-coexistence.jpg idbg-connected.jpg idbg-ipl.jpg \
      idbg-soldered.jpg idbg-top.jpg idbg-wired.jpg idbg-wires.jpg \
      pcb-cut.jpg glued.jpg hole-before.jpg hole-trying.jpg hole-inside.jpg \
      hole-zoom.jpg hole-done.jpg works.jpg \
      ../hw/idbg.ps ../hw/idbg-Component.ps $(IMAGES)

.PHONY:		all images upload clean

all:
		@echo "make what ?" 2>&1
		@exit 1

images:		$(IMAGES)

upload:		$(IMAGES)
		rsync -av -e ssh $(FILES) $(ACCOUNT):$(DIR)

mech.ps:	../hw/mech.fig
		fig2dev -L ps -m 0.9 $< $@ || { rm -f $@; exit 1; }

wiring.ps:	../hw/wiring.fig
		fig2dev -L ps -m 0.9 $< $@ || { rm -f $@; exit 1; }

mech.png:	../hw/mech.fig
		fig2dev -L png -m 0.3 -S 4 $< $@ || { rm -f $@; exit 1; }

wiring.png:	../hw/wiring.fig
		fig2dev -L png -m 0.25 -S 4 $< $@ || { rm -f $@; exit 1; }

idbg.png:	../hw/idbg.ps
		pstopnm -xmax 150 -landscape -stdout $< | \
		  pnmcrop | pnmflip -r180 | pnmtopng >$@ || \
		  { rm -f $@; exit 1; }

idbg-Component.png:	../hw/idbg-Component.ps
		pstopnm -xmax 1500 -ymax 1200 -landscape -stdout $< | \
		  pnmcrop | pnmflip -r180 | pnmtopng >$@ || \
		  { rm -f $@; exit 1; }

clean:
		rm -f $(IMAGES)
