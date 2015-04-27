/****************************************************************************
 * kuRFM.h
 * --------------------------------------------------------------------------
 * RFM interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.06.01  yunsw        Initial revision
 ****************************************************************************/

#ifndef __KUTIL_RFM_H
#define __KUTIL_RFM_H

#include "kutil.h"   

#include "rfmAddrMap.h"
#include "rfm2g_api.h"

#ifdef __cplusplus
extern "C" {
#endif

const int kstar_rfm_open (char *pszDevName);
void kstar_rfm_close ();

const int kstar_rfm_read (void *pDataBuf, const long iOffset, const int size);
const int kstar_rfm_write (const void *pDataBuf, const long iOffset, const int size);

const char *kstar_rfm_map (void *pAddr, const long iOffset, const int iBytes);
const char *kstar_rfm_map_by_pages (void *pAddr, const long iOffset, const int iPages);
void kstar_rfm_unmap (void *pAddr, const int iBytes);
void kstar_rfm_unmap_by_pages (void *pAddr, const int iPages);

#ifdef __cplusplus
}
#endif

#endif /* __KUTIL_RFM_H */

