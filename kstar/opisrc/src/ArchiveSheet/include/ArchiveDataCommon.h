#ifndef ARCHIVE_DATA_COMMON_H
#define ARCHIVE_DATA_COMMON_H

#define XML_STRING 0
#define XML_ENUM 1
#define XML_INT 2
#define XML_DOUBLE 3
void pieces2epicsTime(xmlrpc_int32 secs, xmlrpc_int32 nano, epicsTime &t);
void epicsTime2pieces(const epicsTime &t, xmlrpc_int32 &secs, xmlrpc_int32 &nano);
#endif
