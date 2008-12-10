#!/usr/bin/python

from tmc.scope import *

s = rigol_ds1000c()
s.hor.pos = 0
s.hor.scale = 200e-6	# sample rate is 100MSa/s

s.ch[0].scale = 1
s.ch[0].pos = 2
s.ch[1].scale = 1
s.ch[1].pos = 2

# Manually set trigger. (TMC doesn't have an external trigger yet.)

s.hor.sweep = sweep.Single
s.hor.run()
