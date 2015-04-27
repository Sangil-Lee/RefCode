/* enum for timer structure
 * add some more indices if you want
 */
#ifndef _TIMER_H
#define _TIMER_H


/* timer structure
 * Each histogram function must fill in this structure
 */
typedef struct
{
				double sumT;
				double curT;
				//double freq;
} timer;


/* timer functions */
void timer_init(timer * timer_);
void timer_start(timer * timer_);
void timer_stop(timer * timer_);
double timer_print(timer * timer_, char * string);
#endif
