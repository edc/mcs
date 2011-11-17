typedef short elabel_t;
typedef char vlabel_t;

typedef struct _Edge {
	short left;
	short right;
	elabel_t label;
} Edge;

typedef struct _Graph {
	short n_edges;
	short n_vertices;
	vlabel_t* vertices;
	Edge* edges;
} Graph;


#ifdef __CXX__
extern "C" {
#endif
int max(Graph* graph1, Graph* graph2, int mc, int user_bound, int long_result);
Graph* read_graph(const char* filename);
Graph* parse_sdf(const char* sdf);
void destruct(Graph* g);
void clear();
short get_best(int graph_id, int index);
/* for some language bindings that do not understand NULL */
int is_null(Graph* g);
#ifdef __CXX__
}
#endif

#define MCS_TIMEOUT 15
#define MAGIC_BOND 99
#ifndef __VMAX__C_
extern short *best1;
extern short *best2;
extern short best_l;
extern short stop_now;
#endif
