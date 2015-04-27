/***********************************************************************
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory, and the Regents of the University of
* California, as Operator of Los Alamos National Laboratory, and
* Berliner Elektronenspeicherring-Gesellschaft m.b.H. (BESSY).
* asynDriver is distributed subject to a Software License Agreement
* found in file LICENSE that is included with this distribution.
***********************************************************************/

#ifndef DRVASYNLOCALACQ_H
#define DRVASYNLOCALACQ_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Define additional pasynUser->auxStatus error masks */
int
drvAsynModbusConfigure(char *portName,
                     char *ipaddr,
                     int devId,
                     int noAutoConnect);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* DRVASYNLOCALSERIALPORT_H */
