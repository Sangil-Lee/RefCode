#ifndef DRVASYNLOCALNIPCI_H
#define DRVASYNLOCALNIPCI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Define additional pasynUser->auxStatus error masks */
int drvAsynNiPci6254Configure(char *portName, int extMode, int trigMode);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif 
