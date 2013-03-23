#include "util.h"
	
int last_index(char* s, char c)
{
	int i, result = 0;
	for(i = 0; s[i]; i++)
		if(s[i] == c)
			result = i;
	return result;
}

