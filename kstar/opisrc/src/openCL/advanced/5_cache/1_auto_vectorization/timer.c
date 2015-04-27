#include "timer.h"
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>


void timer_init(timer * timer_)
{
				int i;
				timer_->sumT = 0.0;
}

void timer_start(timer * t)
{
				struct timeval tv;
				gettimeofday(&tv, NULL);
				t->curT = tv.tv_sec + tv.tv_usec / 1.e6;
}

void timer_stop(timer * t)
{
				struct timeval tv;
				gettimeofday(&tv, NULL);
				t->sumT +=  tv.tv_sec + tv.tv_usec / 1.e6 - t->curT;
}

double timer_print(timer * t,  char * string)
{
				printf("%s : %fs \n",string, t->sumT);
				t->sumT = 0;
}

