// Tools
#include <MsgLogger.h>

// add leesi@nfrc.re.kr
// XML-RPC
#include <xmlrpc.h>
// EPICS Base
#include <epicsTime.h>
// Local
//#include "ArchiveDataServer.h"
// end leesi

// epicsTime -> time_t-type of seconds & nanoseconds
void epicsTime2pieces(const epicsTime &t,
                      xmlrpc_int32 &secs, xmlrpc_int32 &nano)
{   // TODO: This is lame, calling epicsTime's conversions twice
    epicsTimeStamp stamp = t;
    time_t time;
    epicsTimeToTime_t(&time, &stamp);
    secs = time;
    nano = stamp.nsec;
}

// Inverse to epicsTime2pieces
void pieces2epicsTime(xmlrpc_int32 secs, xmlrpc_int32 nano, epicsTime &t)
{   // As lame as other nearby code
    epicsTimeStamp stamp;
    if (secs < 0)
    {
        LOG_MSG("Received negative seconds %ld, forcing to 0\n",
                (long) secs);
        secs = 0;
    }
    if (nano < 0)
    {
        LOG_MSG("Received negative nano-seconds %ld, forcing to 0\n",
                (long) nano);
        nano = 0;
    }
    if (nano >= 1000000000)
    {
        LOG_MSG("Received excessive nano-seconds %ld, forcing to 0\n",
                (long) nano);
        nano = 0;
    }
    time_t time = secs;
    epicsTimeFromTime_t(&stamp, time);
    stamp.nsec = nano;
    t = stamp;
}

