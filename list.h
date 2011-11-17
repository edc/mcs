#ifndef __LIST_H
#define __LIST_H
#include <stdlib.h>
#include <gc/gc.h>
#include <stdio.h>
#include <limits.h>

static const short EOL_ELEMENT = SHRT_MIN;						// mark the end of list
static const short DEL_ELEMENT = SHRT_MIN + 1;				// mark deleted element
static const short CROSSED_ELEMENT = SHRT_MIN + 2;		// mark crossed-out element
static const short NONE = SHRT_MIN + 2;							// representing None 

/* is current position the end of list ? */
#define EOL(x) ((x) == EOL_ELEMENT)
/* does current position contain a None object */
#define NA(x) ((x) == DEL_ELEMENT)

/* an interator-style function, for use with while(.) to iterate over all
 * elements in a list and execute the while-loop body */
#define FOREACH(x)	for (short *pp = NULL, *p = foreach((x), &pp); \
													p != NULL; \
													p = foreach((x), &pp))

short* foreach(short*, short**);
void print(short*);

static inline short* makelist(int len, short init)
{
	short* c = GC_MALLOC_ATOMIC(sizeof(short) * (len + 1));
	if (init != NONE) {
		for (int i = 0; i < len; i ++)
			*(c + i) = init;
	}
	*(c + len) = EOL_ELEMENT;
	return c;
}


static inline int len(short* list)
{
	int l = 0;
	FOREACH(list) {
		if (*p != CROSSED_ELEMENT)
			l ++;
	}
	return l;
}

/* append will always do a FOREACH first, to clean the gaps introduced by del
 * and cross */
static inline void append(short* list, short element)
{
	short* last = list;
	FOREACH(list) {
		last = p;
	}
	if (EOL(*last)) {
		*last = element;
		*(last + 1) = EOL_ELEMENT;
	} else {
		*(last + 1) = element;
		*(last + 2) = EOL_ELEMENT;
	}
}

static inline short* find(short* list, short query)
{
	short *result = NULL;
	FOREACH(list) {
		if (result == NULL && *p == query) {
			result = p;
			break;
		}
	}
	return result;
}

static inline short* del(short* list, short query)
{
	short* pointer = find(list, query);
	if (pointer) *pointer = DEL_ELEMENT;
	return pointer;
}

static inline void cross(short* element)
{
	if (element) *element = CROSSED_ELEMENT;
}

static inline short* copy(short* list)
{
	int l = len(list);
	short* c = makelist(l, EOL_ELEMENT); 
	short *c2 = c;
	FOREACH(list)
			*(c2 ++) = *p;
	return c;
}

#endif
