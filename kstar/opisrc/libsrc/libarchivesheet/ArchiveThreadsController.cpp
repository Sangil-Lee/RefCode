#include <algorithm>
#include <cmath>
#include <functional>
#include "ArchiveThreadsController.h"

using namespace std;
template <typename t_type> 
struct start:public unary_function < t_type, void >
{
	void operator()(t_type t)
	{
		t->start();
	};
};

ArchiveThreadsController::ArchiveThreadsController(ArchiveDataClient *client, stdVector<stdString> &names,
		epicsTime &start, epicsTime &end, int count, int how, double interval, void *arg)
{
	ArchiveThread *pthr = 0;
	pthr = new ArchiveThread(client, names, start, end, count, how, interval, arg);
}

ArchiveThreadsController::~ArchiveThreadsController()
{
}
void ArchiveThreadsController::run()
{
	//printchannel();	//for Debug
	for_each(vecQThread.begin(), vecQThread.end(), start<ArchiveThread*> () );
}
