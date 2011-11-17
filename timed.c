#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "vmax.h"

void process_alarm(int sig)
{
	fprintf(stderr, "time out. terminate now.\n");
	stop_now = 1;
}

void set_timeout(int time)
{
	signal(SIGALRM, process_alarm);
	alarm(time);
}
