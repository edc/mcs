#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include "vmax.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "comm.h"
#include <unistd.h>
#include <getopt.h>

char f1[JOBS_AT_A_TIME][256];
char f2[JOBS_AT_A_TIME][256];
long _start_ = 0;

static FILE *out = NULL;
static FILE *err = NULL;

void process_alarm(int sig)
{
	fprintf(stderr, "time out. terminate now.\n");
	stop_now = 1;
	signal(SIGALRM, process_alarm); /* maybe unnecessary, but won't hurt */
}

void leave(int sig)
{
	fprintf(stderr, "received SIGINT. Save and exit.\n");
	if (out) {
		fflush(out);
		fclose(out);
	}
	if (err) {
		fflush(err);
		fclose(err);
	}
	exit(1);
}

int main(int argc, char* argv[])
{
	signal(SIGALRM, process_alarm);
	signal(SIGINT, leave);

	int verbose = 0;
	int clean_start = 0;
	int mc = 1;
	char host[256] = "localhost";

	/* time stats */
	struct timeval begin, end;
	double time_total = 0;
	
	char c;
	int k;
	while ((c = getopt (argc, argv, "vcm:h:")) != -1) {
		switch(c) {
			case 'v' :
				verbose = 1;
				break;
			case 'c' :
				clean_start = 1;
				break;
			case 'h' :
				k = sscanf(optarg, "%s", host);
				if (k != 1) {
					fprintf(stderr, "Usage: %s [-v] [-c] [-m maxconn] [-h host] output\n", argv[0]);
					return 1;
				}
				break;
			case 'm' :
				k = sscanf(optarg, "%d", &mc);
				if (k != 1) {
					fprintf(stderr, "Usage: %s [-v] [-c] [-m maxconn] [-h host] output\n", argv[0]);
					return 1;
				}
				break;
			default :
				fprintf(stderr, "Usage: %s [-v] [-c] [-m maxconn] [-h host] output\n", argv[0]);
				return 1;
		}
	}

	if (optind != argc - 1) {
		fprintf(stderr, "Usage: %s [-v] [-c] [-m maxconn] [-h host] output\n", argv[0]);
		return 1;
	}

	char* output = argv[optind];

	char mode[3] = "";
	if (clean_start) strcat(mode, "w");
	else strcat(mode, "a");

	out = fopen(output, mode);
	if (out == NULL) {
		fprintf(stderr, "cannot open file %s for write\n", output);
		return 1;
	}

	char err_filename[256] = "";
	strcat(err_filename, output);
	strcat(err_filename, ".err");
	err = fopen(err_filename, mode);
	if (err == NULL) {
		fprintf(stderr, "cannot open file %s for write\n", err_filename);
		return 1;
	}

	int lines;
	int progress = 0;
	do {
		lines = refill(host, (char**)f1, (char**)f2);
		//printf("read %d lines\n", lines);
		int i;
		for (i = 0; i < lines; i ++) {

			Graph *g = read_graph(f1[i]);
			Graph *g2 = read_graph(f2[i]);
			if (g == NULL || g2 == NULL) {
				fprintf(err, "%s:%s:skipped\n", f1[i], f2[i]);
				if (g) destruct(g);
				if (g2) destruct(g2);
				continue;
			}

			if (verbose) fprintf(stdout, "%s:%s\n", f1[i], f2[i]);
			gettimeofday(&begin, NULL);
			alarm(MCS_TIMEOUT);
			int best_l = max(g, g2, mc, 0, 0);
			alarm(0);
			gettimeofday(&end, NULL);
			time_total += (end.tv_sec - begin.tv_sec + 
				(end.tv_usec - begin.tv_usec) * 0.000001);

			destruct(g);
			destruct(g2);

			fprintf(out, "%s:%s:%d\n", f1[i], f2[i], best_l);
		}
		fflush(out);
		fflush(err);
		progress += i;
		printf("finished %d lines\n", progress);
	} while (lines == JOBS_AT_A_TIME);

	fprintf(err, "Total time: %f\n", time_total);
	fclose(out); fclose(err);
	return 0;
}
