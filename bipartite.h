/*
 * Edmonds-Karp algorithm for finding a maximum flow and minimum
 * cut in a network. Source code adapted from 
 * http://portablepuzzlecollection-wm.googlecode.com/
 * 
 * The original License agreement
 *
 * This software is copyright (c) 2004-2007 Simon Tatham.
 * 
 * Portions copyright Richard Boulton, James Harvey, Mike Pinna, Jonas
 * Kölker, Dariusz Olszewski and Christopher Fairbairn.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef MAXFLOW_MAXFLOW_H
#define MAXFLOW_MAXFLOW_H

/*
 * The actual algorithm.
 *
 * Inputs:
 *
 *  - `scratch' is previously allocated scratch space of a size
 *    previously determined by calling `maxflow_scratch_size'.
 *
 *  - `nv' is the number of vertices. Vertices are assumed to be
 *    numbered from 0 to nv-1.
 *
 *  - `source' and `sink' are the distinguished source and sink
 *    vertices.
 *
 *  - `ne' is the number of edges in the graph.
 *
 *  - `edges' is an array of 2*ne integers, giving a (source, dest)
 *    pair for each network edge. Edge pairs are expected to be
 *    sorted in lexicographic order.
 *
 *  - `backedges' is an array of `ne' integers, each a distinct
 *    index into `edges'. The edges in `edges', if permuted as
 *    specified by this array, should end up sorted in the _other_
 *    lexicographic order, i.e. dest taking priority over source.
 *
 *  - `capacity' is an array of `ne' integers, giving a maximum
 *    flow capacity for each edge. A negative value is taken to
 *    indicate unlimited capacity on that edge, but note that there
 *    may not be any unlimited-capacity _path_ from source to sink
 *    or an assertion will be failed.
 *
 * Output:
 *
 *  - `flow' must be non-NULL. It is an array of `ne' integers,
 *    each giving the final flow along each edge.
 *
 *  - `cut' may be NULL. If non-NULL, it is an array of `nv'
 *    integers, which will be set to zero or one on output, in such
 *    a way that:
 *     + the set of zero vertices includes the source
 *     + the set of one vertices includes the sink
 *     + the maximum flow capacity between the zero and one vertex
 * 	 sets is achieved (i.e. all edges from a zero vertex to a
 * 	 one vertex are at full capacity, while all edges from a
 * 	 one vertex to a zero vertex have no flow at all).
 *
 *  - the returned value from the function is the total flow
 *    achieved.
 */
int maxflow_with_scratch(void *scratch, int nv, int source, int sink,
			 int ne, const int *edges, const int *backedges,
			 const int *capacity, int *flow, int *cut);

/*
 * The above function expects its `scratch' and `backedges'
 * parameters to have already been set up. This allows you to set
 * them up once and use them in multiple invocates of the
 * algorithm. Now I provide functions to actually do the setting
 * up.
 */
int maxflow_scratch_size(int nv);
void maxflow_setup_backedges(int ne, const int *edges, int *backedges);

/*
 * Simplified version of the above function. All parameters are the
 * same, except that `scratch' and `backedges' are constructed
 * internally. This is the simplest way to call the algorithm as a
 * one-off; however, if you need to call it multiple times on the
 * same network, it is probably better to call the above version
 * directly so that you only construct `scratch' and `backedges'
 * once.
 *
 * Additional return value is now -1, meaning that scratch space
 * could not be allocated.
 */
int maxflow(int nv, int source, int sink,
	    int ne, const int *edges, const int *capacity,
	    int *flow, int *cut);
int max_bipartite(int, int, int, short*);

#endif /* MAXFLOW_MAXFLOW_H */
