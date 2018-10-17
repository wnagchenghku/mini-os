#include "timer.h"
#include <stdio.h>

unsigned long long g_timerfreq;

int main(int argc, char const *argv[])
{
	HRT_INIT(g_timerfreq);
	printf("g_timerfreq is %llu\n", g_timerfreq);
	return 0;
}