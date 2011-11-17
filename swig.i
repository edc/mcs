%module Cmcs
%{
#include "vmax.h"
%}

int max(struct _Graph* graph1, struct _Graph* graph2, int mc, int user_bound, int long_result);
struct _Graph* read_graph(const char* filename);
struct _Graph* parse_sdf(const char* sdf);
short get_best(int graph_id, int index);
int is_null(struct _Graph* g);
void set_timeout(int time);
