/*
 * Finding MCIS between two graphs.
 *
 * A graph is represented by
 *   - a three-element list containing all the edges, giving the adjacency list
 *     and edge label, and 
 *   - a list of vertex labels
 */
#define __VMAX__C_
#include <gc/gc.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include "list.h"
#include "bipartite.h"
#include "vmax.h"
void grow(short* v1_list, short* v2_list, short* cur_mapping1, 
		short* cur_mapping2, int cur_components);

const short MAX_MISMATCHES = 0;								// maximum mismatches allowed

/**************************************************************************
 ******************************** global data  ****************************
 **************************************************************************/

/* storage, configuration, parameters and other static data */
static int lbound;														// lower bound
static short max_components;									// maximum disjoint pieces
static Graph* g1;															// graph #1
static Graph* g2;															// graph #2
static short* degree1;												// degrees for nodes in graph 1
static short* degree2;												// degrees for nodes in graph 2

/* holding the best solution so far */
short best_l;																	// best solution size so far
short* best1;	
short* best2;
short mismatches = 0;
static short* bipartite_matching_buf;

/* used by the timer to signal whether we should stop the search because of
   time-out */
short stop_now = 0;


/**************************************************************************
 ********************************* helpers ********************************
 *************************************************************************/

/* in graph g, given edge e and vertex me, find the other vertex; if
 * me is not part of e, return NONE */
int neighbor(Graph* g, int e, int me)
{
		short other;
		if (g->edges[e].left == me)
			other = g->edges[e].right;
		else if (g->edges[e].right == me)
			other = g->edges[e].left;
		else
			other = NONE;
		return other;
}
/* mapping query. Given mapping from m1 to m2 and query, find the
 * match of query. If query is not in m1, return NONE */
int mapping(short* m1, short* m2, short query)
{
	int i = 0;
	while(! EOL(m1[i])) {
		if (m1[i] == query)
			return m2[i];

		i ++;
	}

	return NONE;
}

/**************************************************************************
 ********************************* entrance *******************************
 *************************************************************************/
/* given two graphs, find the maximum common subgraph (MCS)
 * - graph1 and graph2: input graphs
 * - mc: maximum number of disjoint components. set to 1 to find connected MCS
 * - user_bound: user-supplied lower bound on the size. set to 0 if not unknown
 * - long_result: whether you need to read the matching.
 *
 * Values: return the size as an integer; matching is in global variable best1
 * and best2. set long_result to a true value to read it safely by preventing
 * garbage collection.
 * */
int max(Graph* graph1, Graph* graph2, int mc, int user_bound, int long_result)
{
	if (graph1 == NULL || graph2 == NULL)
		return -1;
	mismatches = 0;
	short i;
	/* save parementers to global */
	lbound = user_bound;
	max_components = mc;
	/* save graphs to global, and if necessary, swap them just to make sure
	 * graph1 is not larger than graph2 */
	int swapped = 0;
	if (graph1->n_vertices > graph2->n_vertices) {
		g1 = graph2;
		g2 = graph1;
		swapped = 1;
	} else {
		g1 = graph1;
		g2 = graph2;
	}
	/* prepare storage for bests */
	best1 = makelist(g1->n_vertices, EOL_ELEMENT);
	best2 = makelist(g1->n_vertices, EOL_ELEMENT);
	best_l = 0;

	/* calculating degrees */
	degree1 = makelist(g1->n_vertices, 0);
	degree2 = makelist(g2->n_vertices, 0);
	bipartite_matching_buf = makelist(g1->n_vertices * g2->n_vertices, NONE);

	for (i = 0; i < g1->n_edges; i ++) {
		short left = g1->edges[i].left;
		short right = g1->edges[i].right;
		degree1[left] ++;
		degree1[right] ++;
	}

	for (i = 0; i < g2->n_edges; i ++) {
		short left = g2->edges[i].left;
		short right = g2->edges[i].right;
		degree2[left] ++;
		degree2[right] ++;
	}

	short* v1_list = makelist(g1->n_vertices, NONE);
	for (i = 0; i < g1->n_vertices; i ++) v1_list[i] = i;
	short* v2_list = makelist(g2->n_vertices, NONE);
	for (i = 0; i < g2->n_vertices; i ++) v2_list[i] = i;

	short* cur_mapping1 = makelist(g1->n_vertices, EOL_ELEMENT);
	short* cur_mapping2 = makelist(g1->n_vertices, EOL_ELEMENT);

	stop_now = 0;
	grow(v1_list, v2_list, cur_mapping1, cur_mapping2, 0);
	alarm(0);
	stop_now = 0;
	
	if (! long_result) {
		best1 = best2 = NULL;
	}
	else if (swapped) {
		short* tmp = best1;
		best1 = best2;
		best2 = tmp;
	}
	degree1 = degree2 = bipartite_matching_buf = NULL;
	clear();

	return best_l;
}

/* clear workspace. release memory */
void clear()
{
	GC_gcollect();
}

/* return the best v1 in v1_list */
int top(short* v1_list, short* cur_mapping1, int cur_components)
{
	short best_v1_general = v1_list[0];
	short best_v1_connected = NONE;

	FOREACH(v1_list) {
		short i = *p;
		if (degree1[i] > degree1[best_v1_general])
			best_v1_general = i;

		/* connected case. check ALL neighbors of i */
		short j;
		for (j = 0; j < g1->n_edges; j ++) {
			/* find a neighbor of i */
			short other = neighbor(g1, j, i);
			if (other == NONE)
				continue;

			/* is the neighbor in the partial match? */
			if (find(cur_mapping1, other) == NULL) continue;

			/* yes. Then we compare the degree */
			if (best_v1_connected == NONE ||
					degree1[i] > degree1[best_v1_connected]) {
				best_v1_connected = i;
				break;
			}
		}
		i ++;
	}

	if (best_v1_connected == NONE &&
			(max_components == NONE || cur_components < max_components))
		return best_v1_general;

	return best_v1_connected;
}

/* boundary handling */
void boundary(short* cur_mapping1, short* cur_mapping2)
{
#ifdef __DEBUG__
	printf("boundary %d", len(cur_mapping1));printf("\n\t");print(cur_mapping1);printf("\t");print(cur_mapping2);
#endif
	short l = len(cur_mapping1);
	if (l > best_l) {
		int i;
		for (i = 0; i < l; i ++) {
			best1[i] = cur_mapping1[i];
			best2[i] = cur_mapping2[i];
		}
		best1[l] = EOL_ELEMENT;
		best2[l] = EOL_ELEMENT;
		best_l = l;
	}
}

/* test compatibility between v1 and v2 given current mapping */
int compatible(short v1, short v2, short* cur_mapping1, short* cur_mapping2)
{
	/* label compatible */
	if (g1->vertices[v1] != g2->vertices[v2])
		return 0;

	/* put neighbors of v1 into a list*/
	int i;
	short* m_v1n = makelist(g1->n_edges, NONE);
	elabel_t* m_v1n_el =
			(elabel_t*) GC_MALLOC_ATOMIC(sizeof(elabel_t) * (g1->n_edges));
	int m_v1n_index = 0;
	for (i = 0; i < g1->n_edges; i ++) {
		short other = neighbor(g1, i, v1);
		if (other != NONE) {
			short m = mapping(cur_mapping1, cur_mapping2, other);
			if (m != NONE) {
				m_v1n[m_v1n_index] = m;
				m_v1n_el[m_v1n_index] = g1->edges[i].label;
				m_v1n_index ++;
			}
		}
	}
	m_v1n[m_v1n_index] = EOL_ELEMENT;
	m_v1n_el[m_v1n_index] = EOL_ELEMENT;

	/* test neighbors of v2 */
	short *p;
	for (i = 0; i < g2->n_edges; i ++) {
		short other = neighbor(g2, i, v2);
		if (other != NONE && mapping(cur_mapping2, cur_mapping1, other) != NONE) {
			if ((p = find(m_v1n, other)) == NULL)
				goto out_0;
			if (!(g2->edges[i].label & m_v1n_el[p - m_v1n]))
				goto out_0;
			*p = CROSSED_ELEMENT;
			m_v1n_el[p - m_v1n] = CROSSED_ELEMENT;
		}
	}

	/* test length of m_v1n */
	if (len(m_v1n) > 0)
		goto out_0;

	/* all tests succeed. return true */
	return 1;

	/* exit for false */
	out_0:
	return 0;
}

static int level = 0;
/* recursive function to grow partial match */
void grow(short* v1_list, short* v2_list, short* cur_mapping1, 
		short* cur_mapping2, int cur_components)
{
	short* v1s = copy(v1_list);
	short* v2s = copy(v2_list);

	short * v1_degrees = makelist(g1->n_vertices, NONE);
	int v1_degrees_index = 0;
	short * v2_degrees = makelist(g2->n_vertices, NONE);
	int v2_degrees_index = 0;
	int i, j, other;
	for (i = 0; i < g1->n_vertices; i ++)
		if (find(cur_mapping1, i) == NULL) {
			int degree = 0;
			for (j = 0; j < g1->n_edges; j ++)
				if ((other = neighbor(g1, j, i)) != NONE && 
						find(cur_mapping1, other) != NULL)
					degree ++;
			v1_degrees[v1_degrees_index] = degree;
			v1_degrees_index ++;
		} else {
			v1_degrees[v1_degrees_index] = -1;
			v1_degrees_index ++;
		}
	v1_degrees[v1_degrees_index] = EOL_ELEMENT;

	for (i = 0; i < g2->n_vertices; i ++)
		if (find(cur_mapping2, i) == NULL) {
			int degree = 0;
			for (j = 0; j < g2->n_edges; j ++)
				if ((other = neighbor(g2, j, i)) != NONE &&
						find(cur_mapping2, other) != NULL)
					degree ++;
			v2_degrees[v2_degrees_index] = degree;
			v2_degrees_index ++;
		} else {
			v2_degrees[v2_degrees_index] = -1;
			v2_degrees_index ++;
		}
	v2_degrees[v2_degrees_index] = EOL_ELEMENT;

	/* bound */
	int bound = len(cur_mapping1);
	short* v1_degrees_c = copy(v1_degrees);
#ifdef __DEBUG__
		printf("v2_degrees:"); print(v2_degrees);
		printf("v1_degrees:"); print(v1_degrees);
#endif
	
/* we are using a very special case of bipartite matching, so general
 * bipartite matching algorithm is not necessary. But we still keep it
 * here as an option */
#ifdef __USE_BIPARTITE_MATCHING
	int bmbi = 0;
	for (int i = 0; i < g1->n_vertices; i ++) {
		if (v1_degrees[i] == -1) continue;
		for (int j = 0; j < g2->n_vertices; j ++) {
			if (v2_degrees[j] == v1_degrees[i] && g2->vertices[j] == g1->vertices[i]) {
				bipartite_matching_buf[bmbi ++] = i;
				bipartite_matching_buf[bmbi ++] = j;
			}
		}
	}
	int bmb = max_bipartite(g1->n_vertices, g2->n_vertices, bmbi/2,
			bipartite_matching_buf);
	//printf("bmb: %d", bmb);
	bound += bmb;
#else
#ifdef __USE_STRICTER_BOUND
	int bmbi = 0;
	for (int j = 0; j < g2->n_vertices; j ++) {
		if (v2_degrees[j] == -1) continue;
		for (int i = 0; i < g1->n_vertices; i ++) {
			if (v2_degrees[j] == v1_degrees_c[i] && compatible(i,j,cur_mapping1,cur_mapping2)) {
				v1_degrees_c[i] = CROSSED_ELEMENT;
				bmbi ++;
			}
		}
	}
	//printf("bmb: %d", bmb);
	bound += bmbi;
#else
	int bip_match = 0;
	for (j = 0; j < g2->n_vertices; j ++) {
		if (v2_degrees[j] == -1) continue;
		for (i = 0; i < g1->n_vertices; i ++) {
			if (v2_degrees[j] == v1_degrees_c[i] && g1->vertices[i] == g2->vertices[j]) {
				v1_degrees_c[i] = CROSSED_ELEMENT;
				bip_match ++;
			}
		}
	}
	bound += bip_match;
#endif
#endif

	if (bound < lbound || bound < best_l) {
		return;
	}

#ifdef __DEBUG__
	printf("bound is %d\n" , bound);
#endif

	/* main loop */
	while (1) {
#ifdef __DEBUG__
		printf("**v1s:"); print(v1s);
#endif
		if (len(v1s) == 0) {
			boundary(cur_mapping1, cur_mapping2);
			return;
		}

		short v1 = top(v1s, cur_mapping1, cur_components);
		if (v1 == NONE) {
			boundary(cur_mapping1, cur_mapping2);
			return;
		}
#ifdef __DEBUG__
		printf("\tpick v1=%d\n", v1);
#endif

		bound = len(cur_mapping1);
		short* v2_degrees_c = copy(v2_degrees);
		FOREACH(v1s) {
			if (del(v2_degrees_c, v1_degrees[*p]) != NULL)
				bound ++;
		}
#ifdef __DEBUG__
		printf("** 486 bound = %d\n", bound);
#endif
		if (bound < lbound || bound < best_l) {
			return;
		}


		del(v1s, v1);
		short v2;
		FOREACH(v2s) {
			v2 = *p;
#ifdef __DEBUG__
			printf("\t\ttry v2=%d (v1=%d) ", v2, v1); print(v2s);
#endif
			int c = compatible(v1, v2, cur_mapping1, cur_mapping2);
			if (c || (!c && mismatches < MAX_MISMATCHES)) {
				if (!c) mismatches ++;
				append(cur_mapping1, v1);
				append(cur_mapping2, v2);
#ifdef __DEBUG__
				printf("\t\tattempting %d:%d\n", v1, v2);
				printf("\t\tmapping:\n"); print(cur_mapping1); print(cur_mapping2);
#endif
				int new_components = cur_components + 1;
				/* do we need to increase components? test whether v1 is connected
				 * to the current partial match*/
				for (j = 0; j < g1->n_edges; j ++)
					if ((other = neighbor(g1, j, v1)) != NONE) {
						if (find(cur_mapping1, other) != NULL) {
							new_components = cur_components;
							break;
						}
					}
				del(v2_list, v2);
				level ++;
#ifdef __DEBUG__
				printf("***** level = %d\n", level);
#endif
				grow(v1s, v2_list, cur_mapping1, cur_mapping2, new_components);
				if (stop_now) return;
				level --;
#ifdef __DEBUG__
				printf("***** level = %d\n", level);
#endif
				/* recover */
				append(v2_list, v2);
				del(cur_mapping1, v1);
				del(cur_mapping2, v2);
#ifdef __DEBUG__
				printf("\t\t\t"); print(cur_mapping1);
				printf("\t\t\t"); print(cur_mapping2);
#endif
			if (!c) mismatches --;
			}
		}
	}

}


