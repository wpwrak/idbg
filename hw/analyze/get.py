#!/usr/bin/python

from tmc.scope import rigol_ds1000c

#-800, +1600
s = rigol_ds1000c()
#w = s.wave((s.ch[0], s.ch[1]), start = -180e-6, end = 80e-6, step = 20e-9)

# -50us, +50us
#w = s.wave((s.ch[0], s.ch[1]), start = -200e-6, end = 200e-6, step = 50e-9)

# -10ms, +10ms
#w = s.wave((s.ch[0], s.ch[1]), start = -10e-3, end = 10e-3, step = 50e-9)

# -250us, +250us
w = s.wave((s.ch[1], s.ch[0]), start = -250e-6, end = 250e-6, step = 10e-9)

w[0].label = "D+";
w[1].label = "D-";

w.save("_wv")
