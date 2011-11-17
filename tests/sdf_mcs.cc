#define __CXX__
extern "C" {
#include "vmax.h"
}
#include <iostream>
#include <fstream>
using namespace std;


int main(int argc, char* argv[])
{
	if (argc < 3) {
		fprintf(stderr, "calculate MCS using two SDF files.\n");
		fprintf(stderr, "usage: %s file1.sdf file2.sdf\n", argv[0]);
		exit(1);
	}


	ifstream ifs(argv[1], ifstream::in);
	char buf[81];
	string sdf;

	while (not ifs.eof()) {
		ifs.getline(buf, 80);
		sdf += buf;
		sdf += "\n";
	}
	
	Graph* g1 = parse_sdf((const char*) sdf.c_str());

	ifstream ifs2(argv[2], ifstream::in);
	sdf = "";

	while (not ifs2.eof()) {
		ifs2.getline(buf, 80);
		sdf += buf;
		sdf += "\n";
	}
	Graph* g2 = parse_sdf((const char*) sdf.c_str());
	
	if (g1 == NULL || g2 == NULL) {
		fprintf(stderr, "cannot find graph file\n");
		return 1;
	}

	int best_l = max(g1, g2, 1, 0, 1);

	printf("%d\n", best_l);
	clear();

	return 0;
}
