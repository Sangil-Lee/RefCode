#include <math.h>
#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "sfwCommon.h"
#include "sfwDriver.h"
#include "sfwMDSplus.h"
#include "sfwTree.h" /* segmented archiving */


#include "myMDSplus.h"

/*
#include <dbCommon.h>	 for epicsPrintf 
#include <epicsThread.h>  for epicsThreadSleep() 
*/


#define statusOk(status) ((status) & 1)

#if 0
int ersc_put_raw_value(ST_STD_device *pSTDdev)
{
	
	int _null = 0; /* Used to mark the end of the argument list */
	int status, i; /* Will contain the status of the data access operation */
//	int dypte_UShort = DTYPE_USHORT ;
	int dypte_Short = DTYPE_SHORT ;
	int dtype_ULong = DTYPE_ULONG; 
	int dtype_Double = DTYPE_DOUBLE;
	
	int rawData_Desc; /* must int type */
	int realRate_Desc_A, realRate_Desc_B;
	int value_at_start_Desc;
	int id_end_Desc;
//	int error_bit = 0;
	char buf[128];
	signed short *prevPtr_A;
	signed short *prevPtr_B;

//	short s16tmp, tlsb;

//	FILE *fp;

	
	int nCntPerRec=0, nCntPerRec_1;	
	int nPseudoSampleCnt = 0;


	double dStartTime;
	double dRealTimeGap_A;
	double dRealTimeGap_B;



	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;



	if( mds_Connect(pSTDdev) == WR_ERROR ) return WR_ERROR;

	if( mds_Open_withFetch(pSTDdev) == WR_ERROR ) {
		mds_Disconnect(pSTDdev);
		return WR_ERROR;
	}
	


	nCntPerRec = pERSC->recordSize / sizeof(short) ; 
	nPseudoSampleCnt = pERSC->recordSize  / sizeof(int) ; /* OS handle data with 32bit */
	nCntPerRec_1 = nCntPerRec - 1;
	dRealTimeGap_A = 1.0 / INIT_SAMPLE_CLOCK_A;
	dRealTimeGap_B = 1.0 / INIT_SAMPLE_CLOCK_B;


	id_end_Desc = descr(&dtype_ULong, &nCntPerRec_1, &_null);

#if 0
	fp = fopen("/nfriapp/rxA.dat", "wb");
	fwrite(pERSC->pRxA_DATA, 2, 8*1024, fp);
	fclose(fp);
	fp = fopen("/nfriapp/rxB.dat", "wb");
	fwrite(pERSC->pRxB_DATA, 2, 8*1024, fp);
	fclose(fp);
#endif

	/* 32 bit swapping */
#if 0
	for(i=0; i<nPseudoSampleCnt; i++) {
//		*(pERSC->pRxA_DATA+i) = SWAP32( *(pERSC->pRxA_DATA+i) );
//		*(pERSC->pRxB_DATA+i) = SWAP32( *(pERSC->pRxB_DATA+i) );

		s16tmp1 = tlsb1 = 0x0;
		tmsb = (*(pERSC->pRxA_DATA+i) & 0xffff0000 ) >> 16 ;
		tlsb = *(pERSC->pRxA_DATA+i) & 0xffff ;
		for( i=0; i < 16; i++) {
			if( i < 8 )
				tmsb1  |=  ( tmsb & ( 0x1 << i)) << (15 - i*2) ;
			else
				tmsb1  |=  ( tmsb & ( 0x1 << i)) >> (i*2 - 15) ;
		}
		for( i=0; i < 16; i++) {
			if( i < 8 )
				tlsb1  |=  ( tlsb & ( 0x1 << i)) << (15 - i*2) ;
			else
				tlsb1  |=  ( tlsb & ( 0x1 << i)) >> (i*2 - 15) ;
		}
		
		*(pERSC->pRxA_DATA+i) = (tmsb1 << 16) | tlsb1;

	}
	
	tmsb = tlsb = 0x0;
	for(i=0; i<nPseudoSampleCnt; i++) {
		tmsb  = (( *(pERSC->pRxA_DATA+i) & 0x20000000 )<< 2) | ( *(pERSC->pRxA_DATA+i) & 0x1fff0000 );
		tlsb  = (( *(pERSC->pRxA_DATA+i) & 0x00002000 )<< 2) | ( *(pERSC->pRxA_DATA+i) & 0x00001fff );
		*(pERSC->pRxA_DATA+i) = tmsb | tlsb;
	}
	
#endif

#if 0
	for(i=0; i<nCntPerRec; i++) {
		s16tmp = tlsb = 0x0;
		for( i=0; i < 16; i++) {
			if( i < 8 )
				s16tmp  |=  ( *(pERSC->pRxA_DATA+i) & ( 0x1 << i)) << (15 - i*2) ;
			else
				s16tmp  |=  ( *(pERSC->pRxA_DATA+i) & ( 0x1 << i)) >> ( i*2 - 15) ;
		}
		*(pERSC->pRxA_DATA+i) = s16tmp;
	}

/*
	s16tmp = tlsb = 0x0;
	for(i=0; i<nCntPerRec; i++) {
		s16tmp  = (( *(pERSC->pRxA_DATA+i) & 0x2000 )<< 2) | ( *(pERSC->pRxA_DATA+i) & 0x1fff );
		*(pERSC->pRxA_DATA+i) = s16tmp;
	}
*/
#endif
#if 0
	for(i=0; i<nCntPerRec; i++) {
		s16tmp = *(pERSC->pRxA_DATA+i);
		
		*(pERSC->pRxA_DATA+i) = ( SFT8MSK4( s16tmp ) << 12) | (SFT12MSK4( s16tmp ) << 8) | (LEV4LSB( s16tmp ) << 4) | SFT4MSK4( s16tmp ) ;
	}
#endif
	
/*
	*(pERSC->pRxA_DATA+1) = 0x00200030;
	*(pERSC->pRxA_DATA+2) = 0x00400050;
	*(pERSC->pRxA_DATA+3) = 0x00600070;
*/

#if 0
	for(i=0; i<nCntPerRec * pERSC->recordNum; i++) {
		*(pERSC->pRxA_DATA+i) = SWAP16( *(pERSC->pRxA_DATA+i) );
		*(pERSC->pRxB_DATA+i) = SWAP16( *(pERSC->pRxB_DATA+i) );
	}
#endif
/*
	fp = fopen("/nfriapp/rxA-swap.dat", "wb");
	fwrite(pERSC->pRxA_DATA, 2, 100, fp);
	fclose(fp);
	fp = fopen("/nfriapp/rxB-swap.dat", "wb");
	fwrite(pERSC->pRxB_DATA, 2, 100, fp);
	fclose(fp);
*/
	prevPtr_A = pERSC->pRxA_DATA;
	prevPtr_B = pERSC->pRxB_DATA;
	for( i=0; i < pERSC->recordNum; i++) 
	{
		if ( pSTDdev->ST_Base.opMode == OPMODE_LOCAL)
			dStartTime = pSTDdev->ST_Base_fetch.dT0[i];
		else
			dStartTime = pSTDdev->ST_Base_fetch.dT0[i] - pSTDdev->ST_Base_fetch.fBlipTime;

		value_at_start_Desc = descr(&dtype_Double, &dStartTime, &_null);



		if( pERSC->ch_mask & BCR_ADC_B_RUN) 
		{
			rawData_Desc = descr(&dypte_Short, pERSC->pRxB_DATA , &nCntPerRec, &_null);
			realRate_Desc_B = descr(&dtype_Double, &dRealTimeGap_B, &_null);
			
			sprintf(buf, "%s%d", pERSC->strTagName[CH_RX_B], i);
			printf("\"%lu\": (ch B, rec:%d) \"%s\", T0: %f, put .... ", pSTDdev->ST_Base_fetch.shotNumber, i, buf, dStartTime);
			status = MdsPut(buf,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($/32764.,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))",
				&rawData_Desc, &id_end_Desc, &value_at_start_Desc, &realRate_Desc_B, &_null);
			printf("Done\n");

			if ( !((status) & 1) )
			{
				printf("%s: Error tag\"%s\", ch B: %s\n",pSTDdev->taskName, buf, MdsGetMsg(status));
			} 
		}
		if( pERSC->ch_mask & BCR_ADC_A_RUN) 
		{
			rawData_Desc = descr(&dypte_Short, pERSC->pRxA_DATA , &nCntPerRec, &_null);
			realRate_Desc_A = descr(&dtype_Double, &dRealTimeGap_A, &_null);
			
			sprintf(buf, "%s%d", pERSC->strTagName[CH_RX_A], i);
			printf("\"%lu\": (ch A, rec:%d) \"%s\", T0: %f, put .... ", pSTDdev->ST_Base_fetch.shotNumber, i, buf, dStartTime);
			status = MdsPut(buf,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($/32764.,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))",
				&rawData_Desc, &id_end_Desc, &value_at_start_Desc, &realRate_Desc_A, &_null);
			printf("Done\n");

			if ( !((status) & 1) )
			{
				printf("%s: Error tag\"%s\", ch A: %s\n",pSTDdev->taskName, buf, MdsGetMsg(status));
			} 
		}
		
		pERSC->pRxA_DATA = pERSC->pRxA_DATA + nCntPerRec;
		pERSC->pRxB_DATA = pERSC->pRxB_DATA + nCntPerRec;
	}
	
	pERSC->pRxA_DATA = prevPtr_A;
	pERSC->pRxB_DATA = prevPtr_B;


	mds_Close_withFetch(pSTDdev);
	mds_Disconnect(pSTDdev);


	return WR_OK;
}
#endif

int ersc_put_raw_value(ST_STD_device *pSTDdev)
{
	int i; /* Will contain the status of the data access operation */
	int opmode;
	
	char buf[128];
	signed short *prevPtr_A;
	signed short *prevPtr_B;

	
	int nCntPerRec=0;
	double dStartTime;

	sfwTreeErr_e	eReturn;
	epicsFloat64	dFBITS;


	ST_ERSC* pERSC = (ST_ERSC *)pSTDdev->pUser;

	opmode = pSTDdev->ST_Base.opMode;


	nCntPerRec = pERSC->recordSize / sizeof(short) ; 

#if 0
	fp = fopen("/nfriapp/rxA.dat", "wb");
	fwrite(pERSC->pRxA_DATA, 2, 8*1024, fp);
	fclose(fp);
	fp = fopen("/nfriapp/rxB.dat", "wb");
	fwrite(pERSC->pRxB_DATA, 2, 8*1024, fp);
	fclose(fp);
#endif


	sfwTreePrintInfo (pERSC->pSfwTree);

	// 4-1. initialize archiving : creates connection and file
	eReturn = sfwTreeBeginArchive (pERSC->pSfwTree);
	if (sfwTreeOk != eReturn) {
		printf ("cannot begin archiving\n");
		return;
	}

	//dScaleValue	= 20 / 65536.0;
	dFBITS = 1.0/32764.0;
	
	// 4-2-0. archiving : scalar value : scale
	eReturn = sfwTreePutFloat64 ( pERSC->pSfwTreeScaleNode[CH_RX_A], dFBITS);
	if (sfwTreeOk != eReturn) printf ("cannot put record, scale A\n");
	eReturn = sfwTreePutFloat64 ( pERSC->pSfwTreeScaleNode[CH_RX_B], dFBITS);
	if (sfwTreeOk != eReturn) printf ("cannot put record, scale B\n");

	prevPtr_A = pERSC->pRxA_DATA;
	prevPtr_B = pERSC->pRxB_DATA;
	for( i=0; i < pERSC->recordNum; i++) 
	{
		if ( pSTDdev->ST_Base.opMode == OPMODE_LOCAL)
			dStartTime = pSTDdev->ST_Base_fetch.dT0[i];
		else
			dStartTime = pSTDdev->ST_Base_fetch.dT0[i] - pSTDdev->ST_Base_fetch.fBlipTime;


		if( pERSC->ch_mask & BCR_ADC_B_RUN) 
		{
			sprintf(buf, "%s:RAW", pERSC->strTagName[CH_RX_B] );
			printf("\"%lu\": (ch B, rec:%d) \"%s\", T0: %f, put .... ", pSTDdev->ST_Base_fetch.shotNumber, i, buf, dStartTime);

			// 4-2. archiving : node, index, start time, samples, buffer
			eReturn = sfwTreePutData (pERSC->pSfwTreeNode[CH_RX_B], i, dStartTime, nCntPerRec, pERSC->pRxB_DATA);
			
			printf("Done\n");
			if (sfwTreeOk != eReturn) {
				printf ("cannot put B record, %d\n", i);
			}

			
		}
		if( pERSC->ch_mask & BCR_ADC_A_RUN) 
		{
			sprintf(buf, "%s:RAW", pERSC->strTagName[CH_RX_A] );
			printf("\"%lu\": (ch A, rec:%d) \"%s\", T0: %f, put .... ", pSTDdev->ST_Base_fetch.shotNumber, i, buf, dStartTime);

			eReturn = sfwTreePutData (pERSC->pSfwTreeNode[CH_RX_A], i, dStartTime, nCntPerRec, pERSC->pRxA_DATA);
			printf("Done\n");

			if (sfwTreeOk != eReturn) {
				printf ("cannot put A record, %d\n", i);
			}

		}
		
		pERSC->pRxA_DATA = pERSC->pRxA_DATA + nCntPerRec;
		pERSC->pRxB_DATA = pERSC->pRxB_DATA + nCntPerRec;
	}
	
	pERSC->pRxA_DATA = prevPtr_A;
	pERSC->pRxB_DATA = prevPtr_B;

	sfwTreeUpdateArchive (pERSC->pSfwTree);

	sfwTreeSyncArchive (pERSC->pSfwTree, 100, 1);
		
	// 4-3. cleanups archiving : flush data, close tree and disconnect
	eReturn = sfwTreeEndArchive (pERSC->pSfwTree, sfwTreeActionFlush);
	if (sfwTreeOk != eReturn) {
		printf ("cannot cleanup archiving\n");
	}

	return WR_OK;
}




