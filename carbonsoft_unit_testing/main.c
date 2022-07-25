#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "func.h"

int usage(const char *pname)
{
	printf("usage: %s <number> <number>\n", pname);
	return EINVAL;
}

int main(int argc, char *argv[])
{
	char *endptr;
	int x, y;
	if (argc != 3)
		return usage(argv[0]);
	x = strtol(argv[1], &endptr, 10);
	if (!endptr || *endptr != 0)
		return usage(argv[0]);
	y = strtol(argv[2], &endptr, 10);
	if (!endptr || *endptr != 0)
		return usage(argv[0]);
	printf("%i\n", func(x, y));
	return 0;
}
