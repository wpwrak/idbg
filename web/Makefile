#
# Written 2008, 2009 by Werner Almesberger
# Copyright 2008, 2009 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


ACCOUNT=werner@almesberger.net
DIR=/home/httpd/almesberger/misc/idbg/

IMAGES=mech.ps wiring.ps \
       mech.png wiring.png idbg.png idbg-Component.png idbg-SilkS_Cmp.png

FILES=index.html idbg-coexistence.jpg idbg-connected.jpg idbg-ipl.jpg \
      idbg-soldered.jpg idbg-top.jpg idbg-wired.jpg idbg-wires.jpg \
      pcb-cut.jpg glued.jpg hole-before.jpg hole-trying.jpg hole-inside.jpg \
      hole-zoom.jpg hole-done.jpg works.jpg \
      ww-connected.jpg ww-first.jpg ww-gnd.jpg ww-wired.jpg \
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

idbg-SilkS_Cmp.png:	../hw/idbg-SilkS_Cmp.ps
		pstopnm -xmax 6000 -ymax 4800 -landscape -stdout $< | \
		  pnmcrop | pnmflip -r180 | pnmtopng >$@ || \
		  { rm -f $@; exit 1; }

clean:
		rm -f $(IMAGES)
