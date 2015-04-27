// This software is copyrighted by the HELMHOLTZ-ZENTRUM BERLIN FUER MATERIALIEN UND ENERGIE G.M.B.H., BERLIN, GERMANY (HZB).
// The following terms apply to all files associated with the software. HZB hereby grants permission to use, copy, and modify
// this software and its documentation for non-commercial educational or research purposes, provided that existing copyright
// notices are retained in all copies. The receiver of the software provides HZB with all enhancements, including complete
// translations, made by the receiver.
// IN NO EVENT SHALL HZB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING
// OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF, EVEN IF HZB HAS BEEN ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE. HZB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS,
// AND HZB HAS NO OBLIGATION TO PROVIDE MAlNTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

//==================================================================================================
// Name        : caLab.h
// Author      : Carsten Winkler, Tobias Höft
// Version     : 1.2.0.0
// Copyright   : HZB
// Description : library for get, put EPICS variables (process variables) in LabVIEW™
//==================================================================================================

#if MAX_STRING_SIZE
#undef MAX_STRING_SIZE
#endif

#define MAX_ERROR_SIZE		255
#define MAX_STRING_SIZE		40
#define PV_CONNECTED		1
#define PV_VALUE_RECEIVED	2
#define PV_INITIALIZED      4
#define PV_IN_QUEUE         8
#define NO_ALARM            0

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

enum epicsAlarmSeverity {
    epicsSevNone = NO_ALARM,
    epicsSevMinor,
    epicsSevMajor,
    epicsSevInvalid,
    ALARM_NSEV
};

enum epicsAlarmCondition {
    epicsAlarmNone = NO_ALARM,
    epicsAlarmRead,
    epicsAlarmWrite,
    epicsAlarmHiHi,
    epicsAlarmHigh,
    epicsAlarmLoLo,
    epicsAlarmLow,
    epicsAlarmState,
    epicsAlarmCos,
    epicsAlarmComm,
    epicsAlarmTimeout,
    epicsAlarmHwLimit,
    epicsAlarmCalc,
    epicsAlarmScan,
    epicsAlarmLink,
    epicsAlarmSoft,
    epicsAlarmBadSub,
    epicsAlarmUDF,
    epicsAlarmDisable,
    epicsAlarmSimm,
    epicsAlarmReadAccess,
    epicsAlarmWriteAccess,
    ALARM_NSTATUS
};

// create Epics context and start needed tasks
// parameter void* is needed for LabVIEW™ compatibility
// call first!
extern "C" EXPORT void create(void*);


// cleanup function
// call last!
extern "C" EXPORT void disconnect(void*);


// initialization of PV
//    szName:                name of EPICS PV
//    szError:               result of function calls
//                           this is no indicator for valid PV values!!!
//    szFieldNames:          chain of optional field requests
//                           field names have format: "%-40s%-40s%-40s" (3 field names)
//    lFieldNameCount:       number of field names
//    lIndex:                internal PV index needed by "get" and "put"-function (return value)
//    lElems:                array size of native EPICS value (return value)
//    dTimeout:              timeout for Channel Access calls; default is 3.0
//    lBufferSize:           max. size of "(re)connect PV queue"; default is 100
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long init(char szName[], char szError[], char szFieldNames[], long lFieldNameCount, long *lIndex, long *lElems, double dTimeout=3.0, long lBufferSize=100);


// get value of Epics PV as string
//    szName:                name of EPICS PV
//    szValueArray:          values formatet as "%-40s%-40s%-40s" (array of 3 values)
//    szTimestamp:           time stamp of EPICS PV as string
//    nStatus:               status of EPICS PV as number
//    nSeverity:             severity of EPICS PV as number
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long getValueAsString(char szName[MAX_STRING_SIZE], char szResult[MAX_STRING_SIZE], char szTimestamp[MAX_STRING_SIZE], short *nStatus, short *nSeverity);


// get value of Epics PV
//    szError:               result of function calls
//    lIndex:                internal PV index read from "init"-function
//    szValueArray:          values formatet as "%-40s%-40s%-40s" (array of 3 values)
//    dValueArray:           values as double array
//    szStatus:              status of EPICS PV as string
//    nStatus:               status of EPICS PV as number
//    szSeverity:            severity of EPICS PV as string
//    nSeverity:             severity of EPICS PV as number
//    szTimestamp:           time stamp of EPICS PV as string
//    lTimestamp:            time stamp of EPICS PV as number
//    szFieldResults:        optional field values of PV formatet as "%-40s%-40s%-40s" (3 fields)
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long get(char szError[], long lIndex, char szValueArray[], double dValueArray[], char szStatus[], short *nStatus, char szSeverity[], short *nSeverity, char szTimeStamp[], long *lTimeStamp, char szFieldResults[]);


// put value of Epics PV as string
//    szName:                name of EPICS PV
//    szValue:               value to be written formatet as "%-40s"
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long putValueAsString(char szName[MAX_STRING_SIZE], char szValue[MAX_STRING_SIZE]);


// put value of Epics PV
//    szError:               result of function calls
//    lIndex:                internal PV index read from "init"-function
//    lElems:                number of values in value array
//    szValueArray:          values to be written formatet as "%-40s%-40s%-40s" (array of 3 values)
//    dValueArray:           values to be written as double array
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long put(char szError[], long lIndex, long lElems, char szValueArray[], double *dValueArray);


// put value of Epics PV without checking of result
//    szName:                name of EPICS PV
//    lElems:                number of values in value array
//    szValueArray:          values to be written formatet as "%-40s%-40s%-40s" (array of 3 values)
//    dValueArray:           values to be written as double array
//    dTimeout:              timeout for Channel Access calls; default is 3.0
//    lBufferSize:           max. size of "(re)connect PV queue"; default is 100
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long fireAndForget(char szName[], long iElems, char szValueArray[], double *dValueArray, double dTimeout=3.0, long lBufferSize=100);


// converts error code to error string
//    lError:                error code of function
//    szError:               error string (return value)
extern "C" EXPORT void getErrorString(long lError, char szError[MAX_ERROR_SIZE]);
