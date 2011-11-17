#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "vmax.h"

void process_alarm(int sig)
{
	fprintf(stderr, "time out. terminate now.\n");
	stop_now = 1;
}

int main(int argc, char* argv[])
{
	signal(SIGALRM, process_alarm);

	if (argc <3) {
		fprintf(stderr, "usage: %s graph_1 graph_2 [connectivity]\n", argv[0]);
		exit(1);
	}
	//Graph *g = read_graph("g2");
	//Graph *g2 = read_graph("g3");
	
	Graph *g = read_graph(argv[1]);
	Graph *g2 = read_graph(argv[2]);

	if (g == NULL || g2 == NULL) {
		fprintf(stderr, "cannot find graph\n");
		exit(1);
	}

	int mc = 1;
	if (argc == 4) {
		mc = strtol(argv[3], NULL, 10);
		if (mc == 0 && errno == EINVAL) {
			fprintf(stderr, "invalid number: %s\n", argv[3]);
			exit(1);
		}
	}

	alarm(MCS_TIMEOUT);
	int best_l = max(g, g2, mc, 0, 0);
	alarm(0);

	destruct(g);
	destruct(g2);

	printf("%s:%s:%d\n", argv[1], argv[2], best_l);

	clear();

	return 0;

}
