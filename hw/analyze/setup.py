#!/usr/bin/python

from tmc.scope import rigol_ds1000c

s = rigol_ds1000c()
s.hor.pos = 0
s.hor.scale = 1e-3	# sample rate is 20MSa/s

# channel setup
# trigger setup
# single-shot
