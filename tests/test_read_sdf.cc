#define __CXX__
extern "C" {
#include "vmax.h"
}
#include <iostream>
#include <fstream>
using namespace std;


int main()
{
	ifstream ifs("/home/ycao/query.sdf", ifstream::in);
	char buf[81];
	string sdf;

	while (not ifs.eof()) {
		ifs.getline(buf, 80);
		sdf += buf;
		sdf += "\n";
	}
	
	parse_sdf((const char*) sdf.c_str());

	return 0;
}
