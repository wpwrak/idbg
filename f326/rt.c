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
