#include <stdio.h>
#include <errno.h>
#include "vmax.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

void process_alarm(int sig)
{
	fprintf(stderr, "time out. terminate now.\n");
	stop_now = 1;
	signal(SIGALRM, process_alarm);
}

static FILE *out = NULL;
static FILE *err = NULL;
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
	if (argc < 2) {
		fprintf(stderr, "Usage: %s command-file [output]\n", argv[0]);
		return 1;
	}
	char output[256] = "";
	char error[256] = "";
	if (argc == 3) {
		strcat(output, argv[2]);
		strcat(output, ".out");
		strcat(error, argv[2]);
		strcat(error, ".err");
	}

	FILE* f = fopen(argv[1], "r");
	if (f == NULL) {
		fprintf(stderr, "cannot open file %s for read\n", argv[1]);
		return 1;
	}

	int file_out = 0;
	if (strlen(output)) {
		file_out = 1;
		out = fopen(output, "a");
		err = fopen(error, "a");
		if (out == NULL || err == NULL) {
		fprintf(stderr, "cannot open file %s for write\n", argv[2]);
		return 1;
		}
	}

	char f1[256*2];
	char f2[256*2];
	int mc = 1;
	int c = 0;
	while (! feof(f)) {
		int i = fscanf(f, "%s %s\n", f1, f2);
		if (i != 2) 
			break;
	
		Graph *g = read_graph(f1);
		Graph *g2 = read_graph(f2);
		if (g == NULL && g2 == NULL) {
			fprintf(stderr, "%s:%s:skipped for %s and %s\n", f1, f2, f1, f2);
			if (file_out)
				fprintf(err, "%s:%s:skipped for %s and %s\n", f1, f2, f1, f2);
			continue;
		} else if (g == NULL) {
			fprintf(stderr, "%s:%s:skipped for %s\n", f1, f2, f1);
			if (file_out)
				fprintf(err, "%s:%s:skipped for %s\n", f1, f2, f1);
			if (g2) destruct(g2);
			continue;
		} else if (g2 == NULL) {
			fprintf(stderr, "%s:%s:skipped for %s\n", f1, f2, f2);
			if (file_out)
				fprintf(err, "%s:%s:skipped for %s\n", f1, f2, f2);
			if (g) destruct(g);
			continue;
		}

		alarm(MCS_TIMEOUT);
		struct timeval start, end;
		gettimeofday(&start, NULL);
		int best_l = max(g, g2, mc, 0, 0);
		gettimeofday(&end, NULL);
		alarm(0);

		long long tdiff;
		tdiff = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

		destruct(g);
		destruct(g2);

		fprintf(stdout, "%s:%s:%d:%d\n", f1, f2, best_l, tdiff);
		if (file_out)
			fprintf(out, "%s:%s:%d:%d\n", f1, f2, best_l, tdiff);

		if (c++ % 150 == 0 && out && err) {
			fflush(out);
			fflush(err);
		}
		
	}
	
	if (out) fclose(out);
	if (err) fclose(err);
	
	fclose(f);

	return 0;
}
