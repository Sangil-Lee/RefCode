#ifndef DRVASYNLOCALNIPCI6220_H
#define DRVASYNLOCALNIPCI6220_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Define additional pasynUser->auxStatus error masks */
int drvAsynNiPci6220Configure(char *portName, int extMode, int trigMode);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif 
