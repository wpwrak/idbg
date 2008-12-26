/*
 * f326/rt.c - Enable/disable real-time scheduling priority
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * The full ritual would also include mlock'ing, but we skip that.
 */


#include <stdlib.h>
#include <stdio.h>
#include <sched.h>


static void realtimize(void)
{
    struct sched_param prm;

    prm.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (prm.sched_priority < 0) {
	perror("sched_get_priority_max SCHED_FIFO");
	exit(1);
    }
    if (sched_setscheduler(0, SCHED_FIFO, &prm) < 0) {
	perror("sched_setscheduler SCHED_FIFO");
	exit(1);
    }
}


static void unrealtime(void)
{
    struct sched_param prm = { .sched_priority = 0 };

    if (sched_setscheduler(0, SCHED_OTHER, &prm) < 0) {
	perror("sched_setscheduler SCHED_OTHER");
	exit(1);
    }
}


void rt(int on)
{
    if (on)
	realtimize();
    else
	unrealtime();
}
