#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "vmax.h"
#include <gc/gc.h>
#include "list.h"

Graph* read_graph(const char* filename)
{
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		return NULL;
	}
	int n_edges, n_vertices;
	if (fscanf(f, "%d %d\n", &n_vertices, &n_edges) != 2) {
		fclose(f); return NULL;
	}
	if (n_vertices <= 0 || n_edges <= 0) {
		fclose(f); return NULL;}
	vlabel_t* vertices = GC_MALLOC_ATOMIC(sizeof(vlabel_t) * n_vertices);
	Edge* edges = (Edge*) GC_MALLOC_ATOMIC(sizeof(Edge) * n_edges);

	int i;
	if (fscanf(f, "%c", vertices) != 1) {
		fclose(f); return NULL;
	}
	for (i = 1; i < n_vertices; i ++) 
		if (fscanf(f, " %c", vertices + i) != 1) {
			fclose(f); return NULL;
		}
	for (i = 0; i < n_edges; i ++) {
		int a, b, e;
		if (fscanf(f, " %d %d %d", &a, &b, &e) != 3) {
			fclose(f); return NULL;
		}
		if (a > n_vertices || b > n_vertices) {
			fclose(f); return NULL;
		}
		edges[i].left = a;
		edges[i].right = b;
		edges[i].label = e;
		//fprintf(stderr, "\tedge(%d,%d,%d)\n", a, b, e);
	}

	Graph* g = (Graph*) GC_MALLOC(sizeof(Graph));
	g->n_edges = n_edges; g->n_vertices = n_vertices;
	g->edges = edges; g->vertices = vertices;
	//fprintf(stderr, "graph(%d,%d) is built\n", n_vertices, n_edges);

	fclose(f);

	return g;
}

static char elements[111][3] = { "Ru", "Re", "Rf", "Rg", "Ra", "Rb", "Rn", "Rh", "Be", "Ba", "Bh", "Bi", "Bk", "Br", "H", "P", "Os", "Ge", "Gd", "Ga", "Pr", "Pt", "Pu", "C", "Pb", "Pa", "Pd", "Cd", "Po", "Pm", "Hs", "Ho", "Hf", "Hg", "He", "Md", "Mg", "K", "Mn", "O", "Mt", "S", "W", "Zn", "Eu", "Zr", "Er", "Ni", "No", "Na", "Nb", "Nd", "Ne", "Np", "Fr", "Fe", "Fm", "B", "F", "Sr", "N", "Kr", "Si", "Sn", "Sm", "V", "Sc", "Sb", "Sg", "Se", "Co", "Cm", "Cl", "Ca", "Cf", "Ce", "Xe", "Tm", "Cs", "Cr", "Cu", "La", "Li", "Tl", "Lu", "Lr", "Th", "Ti", "Te", "Tb", "Tc", "Ta", "Yb", "Db", "Dy", "Ds", "At", "I", "U", "Y", "Ac", "Ag", "Ir", "Am", "Al", "As", "Ar", "Au", "Es", "In", "Mo" };
/* map every element to a char */
static char code[111] = {
	'r', 'J', (char)0x86, (char)0x8d, 'u', 'R', 'j', '.', 'b', '5', (char)0x89, '`', (char)0x7f, '*', 'H', 'P', 'o', 'g', ';', 'G', '7', '[', '|', 'C', '_', 'x', 'p', '0', 'd', '9', (char)0x8a, '=', '@', ']', 'h', (char)0x83, 'M', 'K', 'm', 'O', (char)0x8b, 'S', 'W', 'Z', (char)0x8e, 'z', (char)0x8f, '&', (char)0x84, '!', ',', '8', 'n', '{', 'q', 'f', (char)0x82, 'B', 'F', '+', 'N', 'k', 's', '1', ':', 'V', '#', '2', (char)0x88, ')', '%', '~', 'c', '"', (char)0x80, '6', 'X', '>', '4', '$', '\'', 'l', 'L', '^', '?', (char)0x85, 'w', 'T', '3', '<', 't', 'E', 'y', (char)0x87, 'D', (char)0x8c, 'e', 'I', 'U', 'Y', 'v', '/', 'Q', '}', 'A', '(', 'a', '\\', (char)0x81, 'i', '-'
};
char* strip(char* str)
{
	int str_len = strlen(str);
	int i = 0;
	while (i < str_len) {
		if (str[i] == ' ') {str[i] = str[i + 1]; str_len --;}
		else i ++;
	}
	str[str_len] = '\0';

	return str;
}
char map(char* str)
{
	for (int i = 0; i < 111; i ++) {
		if (strcmp(strip(str), elements[i]) == 0)
			return code[i];
	}

	return 0;
}

/* specifically used to parse an unsigned integer between 0 to 999 */
int parse_int(const char* p)
{
	char num_buf[4] = "";
	char *endp;
	strncpy(num_buf, p, 3);
	num_buf[3] = '\0';
	int num = strtol(num_buf, &endp, 10);

	if (endp == num_buf)
		return -1;
	return num;
}

/* read a line and return the endpoint*/
const char* read_line(const char* from, char* to, int limit)
{
	int i;
	for (i = 0; i < limit && *(from + i) != '\0' && *(from + i) != '\n'; i ++) 
		*(to + i) = *(from + i);
	if (i == limit) return NULL;
	to[i] = '\0';
	if (*(from +i) == '\0') return NULL;
	const char* endp; endp = from + i + 1;
	if (*endp == '\r') endp ++;
	return endp;
}

Graph* parse_sdf(const char* sdf)
{
	char line_buf[81] = "";		/* support up to 80 characters */
	const char *p = sdf;
	/* first skip 3 lines */
	if ((p = read_line(p, line_buf, 81)) == NULL) return NULL;
	if ((p = read_line(p, line_buf, 81)) == NULL) return NULL;
	if ((p = read_line(p, line_buf, 81)) == NULL) return NULL;
	/* read number of atoms and bonds */
	p = read_line(p, line_buf, 81);
	int n_vertices = parse_int(line_buf);	if (n_vertices == -1) {return NULL;}
	int n_edges = parse_int(line_buf + 3); if (n_edges == -1) {return NULL;}

	vlabel_t* vertices = GC_MALLOC_ATOMIC(sizeof(vlabel_t) * n_vertices);
	Edge* edges = (Edge*) GC_MALLOC_ATOMIC(sizeof(Edge) * n_edges);

	/* iterate over the following n_vertices lines to read atoms */
	char element[4] = "";
	for (int i = 0; i < n_vertices; i ++) {
		if (p == NULL) return NULL;
		p = read_line(p, line_buf, 81);
		strncpy(element, line_buf + 31, 3);
		element[3] = '\0';
		char c = map(element);
    if (c == '\0') {return NULL;}
		vertices[i] = c;
	}

	/* iterate over the following n_edges lines to read the connection table
		 we supports bond type to up to 9. if value is large than 9, it must be
		 larger than 100, and the value will be deducted by 100, and then treated
		 as a OR or several types to allow flexiable matching. For example, type
		 99 is not valid. Type 103 in binary is 0 0000 0011, and therefore will
		 match type 1 and type 2
   */
	for (int i = 0; i < n_edges; i ++) {
		int a, b, e;
		if (p == NULL) return NULL;
		p = read_line(p, line_buf, 81);
		a = parse_int(line_buf);
		b = parse_int(line_buf + 3);
		e = parse_int(line_buf + 6);
		if (a == -1 || b == -1 || e == -1) {return NULL;}
		edges[i].left = a - 1;
		edges[i].right = b - 1;
		if (e == 0) return NULL;
		if (e >= 10) {
			if (e < 100) return NULL;
			edges[i].label = e - 100;
		} else edges[i].label = 1 << (e - 1);
		//fprintf(stderr, "\tedge(%d,%d,%d)\n", a, b, e);
	}

	Graph* g = (Graph*) GC_MALLOC(sizeof(Graph));
	g->n_edges = n_edges; g->n_vertices = n_vertices;
	g->edges = edges; g->vertices = vertices;
	//fprintf(stderr, "graph(%d,%d) is built\n", n_vertices, n_edges);
	//

	return g;
}

void destruct(Graph* g)
{
  (void) g; // suppress unused variable warning
	//free(g->vertices); free(g->edges); free(g);
}

short get_best(int graph_id /* 1 or 2 */, int idx)
{
	short* p = NULL;
	if (graph_id == 1) p = best1;
	else if (graph_id == 2) p = best2;
	if (p == NULL) return NONE;
	if (idx >= len(p)) return NONE;

	return *(p + idx); 
}

/* for R extension */
int is_null(Graph* g) {
	if (g == NULL) return 1;
	else return 0;
}
