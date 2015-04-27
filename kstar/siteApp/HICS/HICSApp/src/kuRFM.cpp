/****************************************************************************
 * kuRFM.c
 * --------------------------------------------------------------------------
 * RFM API
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.06.01  yunsw        Initial revision
 ****************************************************************************/

#include "kuRFM.h"
#include "kutilObj.h"

static RFM2GHANDLE		gHandle		= NULL;

void kstar_rfm_close ()
{
	/* Close the Reflective Memory device */
	RFM2gClose (&gHandle);

    kuDebug (kuMON, "[kstar_rfm_close] closed!\n" );
}

const int kstar_rfm_open (char *pszDevName)
{
	RFM2G_NODE		NodeId;
	RFM2G_STATUS	result;                 /* Return codes from RFM2Get API calls  */
	RFM2G_UINT32	dmaThreshold;

	/* Open the Reflective Memory device */
	result = RFM2gOpen (pszDevName, &gHandle);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gOpen] %s\n", RFM2gErrorMsg(result));
		return(kuNOK);
	}

	/* Tell the user the NodeId of this device */
	result = RFM2gNodeID (gHandle, &NodeId);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gNodeID] %s\n", RFM2gErrorMsg(result));
		kstar_rfm_close ();
		return(kuNOK);
	}

	kuDebug (kuINFO, "[RFM2gNodeID] NodeID = 0x%02x\n", NodeId);

	/* Get the DMA Threshold */
	result = RFM2gGetDMAThreshold (gHandle, &dmaThreshold);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gGetDMAThreshold] %s\n", RFM2gErrorMsg(result));
		kstar_rfm_close ();
		return(kuNOK);
	}

	kuDebug (kuINFO, "[RFM2gGetDMAThreshold] DMA Threshold is %u\n", dmaThreshold);

	kuDebug (kuMON, "[kstar_rfm_open] dev(%s) opened ...\n", pszDevName);

	return kuOK;
}

const int kstar_rfm_read (void *pDataBuf, const long iOffset, const int size)
{
	if ( (NULL == pDataBuf) || size < 0) {
		return (kuNOK);
	}

	RFM2G_STATUS	result;					/* Return codes from RFM2Get API calls */

    /* Use read to collect the data */
	result = RFM2gRead (gHandle, iOffset, pDataBuf, size);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gRead] %s\n", RFM2gErrorMsg(result));
		kstar_rfm_close ();
		return(kuNOK);
	}

	kuDebug (kuTRACE, "[kstar_rfm_read] offset(0x%08x) size(%d)\n", iOffset, size);

	if (kuTRUE == kuCanPrint (kuDATA)) {
		kuStd::dump (size, (const char *)pDataBuf);
	}

	return (kuOK);
}

const int kstar_rfm_write (const void *pDataBuf, const long iOffset, const int size)
{
	if ( (NULL == pDataBuf) || size < 0) {
		return (kuNOK);
	}

	RFM2G_STATUS	result;					/* Return codes from RFM2Get API calls */

	kuDebug (kuTRACE, "[kstar_rfm_write] offset(0x%08x) size(%d)\n", iOffset, size);

	if (kuTRUE == kuCanPrint (kuDATA)) {
		kuStd::dump (size, (const char *)pDataBuf);
	}

	result = RFM2gWrite (gHandle, iOffset, (void *)pDataBuf, size);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gWrite] %s\n", RFM2gErrorMsg(result));
		kstar_rfm_close ();
		return(kuNOK);
	}

	return (kuOK);
}

const char *kstar_rfm_map (void *pAddr, const long iOffset, const int iBytes)
{
	RFM2G_STATUS	result;                 /* Return codes from RFM2Get API calls  */
	
	result = RFM2gUserMemoryBytes (gHandle, (volatile void **)(&pAddr), iOffset, iBytes);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gUserMemoryBytes] %s\n", RFM2gErrorMsg(result));
		kstar_rfm_close ();
		return (NULL);
	}

	kuDebug (kuDEBUG, "[kstar_rfm_map] offset(0x%08x) size(%d) : map ok\n", iOffset, iBytes);

	return ((const char *)pAddr);
}

const char *kstar_rfm_map_by_pages (void *pAddr, const long iOffset, const int iPages)
{
	RFM2G_STATUS	result;                 /* Return codes from RFM2Get API calls  */
	
	result = RFM2gUserMemory (gHandle, (volatile void **)(&pAddr), iOffset, iPages);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gUserMemory] %s\n", RFM2gErrorMsg(result));
		kstar_rfm_close ();
	}

	return ((const char *)pAddr);
}

void kstar_rfm_unmap (void *pAddr, const int iBytes)
{
	RFM2G_STATUS	result;                 /* Return codes from RFM2Get API calls  */
	
	result = RFM2gUnMapUserMemoryBytes (gHandle, (volatile void **)(&pAddr), iBytes);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gUnMapUserMemoryBytes] %s\n", RFM2gErrorMsg(result));
	}

	kstar_rfm_close ();
}

void kstar_rfm_unmap_by_pages (void *pAddr, const int iPages)
{
	RFM2G_STATUS	result;                 /* Return codes from RFM2Get API calls  */
	
	result = RFM2gUnMapUserMemory (gHandle, (volatile void **)(&pAddr), iPages);

	if( result != RFM2G_SUCCESS ) {
		kuDebug (kuERR, "[RFM2gUnMapUserMemory] %s\n", RFM2gErrorMsg(result));
	}

	kstar_rfm_close ();
}

#if 0
void kstar_rfm_example ()
{
	int		nShotNumber;

	// only one time
	if (kuOK != kstar_rfm_open ((char *)STR_RFM_DEV_NAME)) {
		kuDebug (kuERR, "[kstar_rfm_handler] open(%s) failed\n", STR_RFM_DEV_NAME);
		return;
	}

#if 0
	if (NULL == kstar_rfm_map (&pEC1_ANT->pRFMMapBuf, pEC1_ANT->Offset, pEC1_ANT->Bytes)) {
		kuDebug (kuERR, "[kstar_rfm_handler] kstar_rfm_map() failed\n");
		return;
	}
#endif

	if (kuOK != kstar_rfm_read (&nShotNumber, RFM_PCS_SHOT_NO, sizeof(nShotNumber))) {
		kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_SHOT_NO\n");
		continue;
	}

	// only one time
	kstar_rfm_close ();
}
#endif

