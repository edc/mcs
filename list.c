#define __LIST_C
#include "list.h"

short* foreach(short* list, short** bk)
{
	if (*bk == NULL)
		*bk = list;

	short* peek = NULL;

	while (!EOL(**bk)) {
		if (!NA(**bk)) {
			*bk = *bk + 1;
			return *bk - 1;
		} else {
			/* move following */
			if (peek == NULL) peek = *bk + 1;
			else peek ++;
			**bk = *peek;
			*peek = DEL_ELEMENT;
		}
	}

	*bk = NULL;
	return NULL;
}

void print(short* list)
{
	FOREACH(list)
			printf("%d ", *p);
	printf("\n");
}


