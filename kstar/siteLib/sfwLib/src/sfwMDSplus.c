/****************************************************************************

Module      : sfwMDSplus.c

Copyright(c): 2010 NFRI. All Rights Reserved.

Revision History:
Author: woong   2010-4-16

2010. 7. 7  arrangement.


*****************************************************************************/

#include "sfwMDSplus.h"
#include "sfwCommon.h"


#if 0
/* An example of Data readout using the MDSplus Data Access library (mdslib) */
int dtype_Float = DTYPE_FLOAT; /* We are going to read a signal made of float values */
int dtype_Double = DTYPE_DOUBLE;
int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an	integer */
int dtype_ULong = DTYPE_ULONG; 
int dypte_Short = DTYPE_SHORT ;
int dypte_UShort = DTYPE_USHORT ;
#endif


#define PRINT_SFW_MDS_CONNECTION     0


int mds_Connect(ST_STD_device *pSTDdev)
{
	int op = pSTDdev->ST_Base.opMode;
	pSTDdev->ST_mds.socket = MdsConnect(pSTDdev->ST_mds.treeIPport[op]);
	if ( pSTDdev->ST_mds.socket == -1 )
	{
		notify_error( ERR_SCN_PNT, "%s:Error mdsip \"%s\"\n", pSTDdev->taskName, pSTDdev->ST_mds.treeIPport[op]);
		printlog("%s: Error connecting to mdsip server \"%s\".\n", pSTDdev->taskName, pSTDdev->ST_mds.treeIPport[op]);
		return WR_ERROR;
	}
	pSTDdev->ST_mds.StatusMds |= MDS_CONNECTED;
#if PRINT_SFW_MDS_CONNECTION
	printf("%s: Tree Connect(\"%s\"):%s (0x%X)... OK\n", pSTDdev->taskName, pSTDdev->ST_mds.treeIPport[op], pSTDdev->ST_mds.treeName[op], pSTDdev->ST_mds.StatusMds);
#endif
	return WR_OK;
}

int mds_Connect_master(ST_MASTER *pMaster)
{
/*	int op = pMaster->ST_Base.opMode; */
/* alway create local tree */
	int op = OPMODE_LOCAL;
	pMaster->ST_mds.socket = MdsConnect(pMaster->ST_mds.treeIPport[op]);
	if ( pMaster->ST_mds.socket == -1 )
	{
		notify_error( ERR_SCN_PNT, "%s:Error mdsip \"%s\"\n", pMaster->taskName, pMaster->ST_mds.treeIPport[op]);
		printlog("%s: Error connecting to mdsip server \"%s\".\n", pMaster->taskName, pMaster->ST_mds.treeIPport[op]);
		return WR_ERROR;
	}
	pMaster->ST_mds.StatusMds |= MDS_CONNECTED;
#if PRINT_SFW_MDS_CONNECTION
	printf("%s: Tree Connect(\"%s\"):%s (0x%X)... OK\n", pMaster->taskName, pMaster->ST_mds.treeIPport[op], pMaster->ST_mds.treeName[op], pMaster->ST_mds.StatusMds);
#endif
	return WR_OK;
}


int mds_Open(ST_STD_device *pSTDdev)
{
	return fnc_open(pSTDdev, pSTDdev->ST_Base.shotNumber);
}
int mds_Open_withFetch(ST_STD_device *pSTDdev)
{
	return fnc_open(pSTDdev, pSTDdev->ST_Base_fetch.shotNumber);
}

int mds_Close(ST_STD_device *pSTDdev)
{
	return fnc_close(pSTDdev, pSTDdev->ST_Base.shotNumber);
}
int mds_Close_withFetch(ST_STD_device *pSTDdev)
{
	return fnc_close(pSTDdev, pSTDdev->ST_Base_fetch.shotNumber);
}

int mds_Disconnect(ST_STD_device *pSTDdev)
{
/*	int op = pSTDdev->ST_Base.opMode; */
	if( pSTDdev->ST_mds.StatusMds & MDS_OPEN_OK ){
		notify_error( ERR_SCN_PNT, "%s: must be Closed!\n", pSTDdev->taskName);
		return WR_ERROR;
	}

/*	MdsDisconnect(pSTDdev->ST_mds.treeIPport[op]); */
	MdsDisconnect();
	pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;
#if PRINT_SFW_MDS_CONNECTION
	printf("%s: Tree Disconnect (0x%X)...  OK\n", pSTDdev->taskName, pSTDdev->ST_mds.StatusMds );
#endif
	return WR_OK;	
}

int mds_Disconnect_master(ST_MASTER *pMaster)
{
/*	int op = pMaster->ST_Base.opMode; */
	if( pMaster->ST_mds.StatusMds & MDS_OPEN_OK ){
		notify_error( ERR_SCN_PNT,"%s: must be Closed!\n", pMaster->taskName);
		return WR_ERROR;
	}

/*	MdsDisconnect(pMaster->ST_mds.treeIPport[op]); */
	MdsDisconnect();
	pMaster->ST_mds.StatusMds &= ~MDS_CONNECTED;
#if PRINT_SFW_MDS_CONNECTION
	printf("%s: Tree Disconnect (0x%X)...  OK\n", pMaster->taskName, pMaster->ST_mds.StatusMds );
#endif
	return WR_OK;	
}

int mds_createNewShot(ST_STD_device *pSTDdev)
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int shot, op;
	int tstat, len;
	char buf[128];
	int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an	integer */
	int idesc = descr(&dtype_Long, &tstat, &null);


	shot = pSTDdev->ST_Base.shotNumber;
	op = pSTDdev->ST_Base.opMode;

	if( op == OPMODE_REMOTE ) {
		kfwPrint(0, "%s: Must be Test mode run!!\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	if( !(pSTDdev->ST_mds.StatusMds & MDS_CONNECTED) ){
		kfwPrint(0, "%s: MDS tree not connected: 0x%X\n", pSTDdev->taskName, pSTDdev->ST_mds.StatusMds );
		return WR_ERROR;
	}

		sprintf(buf, "TCL(\"set tree %s/shot=-1\")", pSTDdev->ST_mds.treeName[op]);
		status = MdsValue(buf, &idesc, &null, &len);
		if ( !((status) & 1) )
		{
			kfwPrint(0, "Error TCL command: %s.\n",buf);
			MdsDisconnect();
			pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;
			return WR_ERROR;
		}
		sprintf(buf, "TCL(\"create pulse %d\")", shot);
		status = MdsValue(buf, &idesc, &null, &len);
		if ( !((status) & 1) )
		{
			kfwPrint(0, "Error TCL command: %s.\n", buf);
			MdsDisconnect();
			pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;
			return WR_ERROR;
		}

		/* udpate shotid.sys file with current shot number */
		sprintf(buf, "TCL(\"set current %s %d\")", pSTDdev->ST_mds.treeName[op], shot);
		status = MdsValue(buf, &idesc, &null, &len);
		if ( !((status) & 1) )
		{
			kfwPrint(0, "Error TCL command: %s.\n", buf);
			MdsDisconnect();
			return WR_ERROR;
		}

		status = MdsValue("TCL(\"close\")", &idesc, &null, &len);
		
		kfwPrint(0, "%s: Create new tree(\"%d\")... OK\n", pSTDdev->taskName, shot);

	return WR_OK;	
}

/* this function create only local tree */
int mds_createNewShot_master(ST_MASTER *pMaster)
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int shot, op;
	int tstat, len;
	char buf[128];
	int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an	integer */
	int idesc = descr(&dtype_Long, &tstat, &null);


	shot = pMaster->ST_Base.shotNumber;
/*	op = pMaster->ST_Base.opMode; */
	/* alway create local tree */
	op = OPMODE_LOCAL;

	if( op == OPMODE_REMOTE ) {
		kfwPrint(0, "%s: Must be Test mode run!!\n", pMaster->taskName);
		return WR_ERROR;
	}

	if( !(pMaster->ST_mds.StatusMds & MDS_CONNECTED) ){
		kfwPrint(0, "%s: MDS tree not connected: 0x%X\n", pMaster->taskName, pMaster->ST_mds.StatusMds );
		return WR_ERROR;
	}

		sprintf(buf, "TCL(\"set tree %s/shot=-1\")", pMaster->ST_mds.treeName[op]);
		status = MdsValue(buf, &idesc, &null, &len);
		if ( !((status) & 1) )
		{
			kfwPrint(0, "Error TCL command: %s.\n", buf);
			MdsDisconnect();
			pMaster->ST_mds.StatusMds &= ~MDS_CONNECTED;
			return WR_ERROR;
		}
		sprintf(buf, "TCL(\"create pulse %d\")", shot);
		status = MdsValue(buf, &idesc, &null, &len);
		if ( !((status) & 1) )
		{
			kfwPrint(0, "Error TCL command: %s.\n", buf);
			MdsDisconnect();
			pMaster->ST_mds.StatusMds &= ~MDS_CONNECTED;
			return WR_ERROR;
		}

		/* udpate shotid.sys file with current shot number */
		sprintf(buf, "TCL(\"set current %s %d\")", pMaster->ST_mds.treeName[op], shot);
		status = MdsValue(buf, &idesc, &null, &len);
		if ( !((status) & 1) )
		{
			kfwPrint(0, "Error TCL command: %s.\n", buf);
			MdsDisconnect();
			return WR_ERROR;
		}

		status = MdsValue("TCL(\"close\")", &idesc, &null, &len);
		
		kfwPrint(0, "%s: Create new tree(\"%d\")... OK\n", pMaster->taskName, shot);

	return WR_OK;	
}

int mds_notify_EndOfTreePut(ST_STD_device *pSTDdev)
{
	int null = 0; 
	int status;
	int tstat, len;
	char buf[128];
	int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an	integer */
	int idesc = descr(&dtype_Long, &tstat, &null);
	int socket, op;

	op = pSTDdev->ST_Base.opMode;
	socket = MdsConnect(pSTDdev->ST_mds.treeEventIP[op]);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server(%s).\n", pSTDdev->ST_mds.treeEventIP[op]);
		return WR_ERROR;
	}

	sprintf(buf, "TCL(\"setevent %s\")", pSTDdev->ST_mds.treeEventArg);
	status = MdsValue(buf, &idesc, &null, &len);
	if ( !((status) & 1) )
	{
		kfwPrint(0, "Error TCL command: %s.\n", buf);
		MdsDisconnect();
/*		pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED; don't need this */
		return WR_ERROR;
	}

	MdsDisconnect();

	kfwPrint(0, "Spawn event: \"%s\", tree put completed!\n", pSTDdev->ST_mds.treeEventIP[op]);
	return WR_OK;
}

int fnc_open(ST_STD_device *pSTDdev, int shot)
{
	int status; /* Will contain the status of the data access operation */
	int op = pSTDdev->ST_Base.opMode;

	if( pSTDdev->ST_mds.StatusMds & MDS_CONNECTED ){
		status = MdsOpen(pSTDdev->ST_mds.treeName[op], &shot);
		if ( !((status) & 1) )
		{
			fprintf(stderr,"%s: Error opening \"%s\" for shot %d.\n", pSTDdev->taskName, pSTDdev->ST_mds.treeName[op], shot );
			printlog("%s: Error opening tree for shot %d.\n", pSTDdev->taskName, shot );
			MdsDisconnect();
			pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;
			return WR_ERROR;
		}
		pSTDdev->ST_mds.StatusMds |= MDS_OPEN_OK;
#if PRINT_SFW_MDS_CONNECTION		
		printf("%s: Tree Open(\"%s\", shot: %d)... OK\n", pSTDdev->taskName, pSTDdev->ST_mds.treeName[op], shot);
#endif
		
	} else {
		kfwPrint(0, "%s: MDS tree not connected! (0x%X)\n", pSTDdev->taskName, pSTDdev->ST_mds.StatusMds);
		printlog("[SFW] %s: MDS tree not connected! (0x%X)\n", pSTDdev->taskName, pSTDdev->ST_mds.StatusMds);
		return WR_ERROR;
	}

	return WR_OK;
}

int fnc_close(ST_STD_device *pSTDdev, int shot)
{
	int status; /* Will contain the status of the data access operation */
	int op = pSTDdev->ST_Base.opMode;

	if( !(pSTDdev->ST_mds.StatusMds & MDS_OPEN_OK) ){
		kfwPrint(0, "%s: tree \"%s\" shot \"%d\" not opened: 0x%X\n", pSTDdev->taskName, pSTDdev->ST_mds.treeName[op], shot, pSTDdev->ST_mds.StatusMds);
		return WR_ERROR;
	}
	
	status = MdsClose(pSTDdev->ST_mds.treeName[op], &shot);
	if ( !((status) & 1) )
	{
		kfwPrint(0, "\n>>> Error closing tree for shot %d\n",shot );
		MdsDisconnect();
		pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;
		return WR_ERROR;
	}
	pSTDdev->ST_mds.StatusMds &= ~MDS_OPEN_OK;
#if PRINT_SFW_MDS_CONNECTION
	printf("%s: Tree Close shot \"%d\" (0x%X)...  OK\n", pSTDdev->taskName, shot,  pSTDdev->ST_mds.StatusMds);
#endif
	return WR_OK;
}

int env_MDSinfo_to_STDdev(ST_STD_device *pSTDdev, char *filter, char *value)
{
	if( !strcmp(filter, "LOCAL_IP") ) 
		strcpy( pSTDdev->ST_mds.treeIPport[OPMODE_LOCAL], value );
	else if ( !strcmp(filter, "LOCAL_NAME") )
		strcpy( pSTDdev->ST_mds.treeName[OPMODE_LOCAL], value );
	else if ( !strcmp(filter, "LOCAL_EVENT") ) 
		strcpy( pSTDdev->ST_mds.treeEventIP[OPMODE_LOCAL], value );

	else if( !strcmp(filter, "REMOTE_IP") ) 
		strcpy( pSTDdev->ST_mds.treeIPport[OPMODE_REMOTE], value );
	else if ( !strcmp(filter, "REMOTE_NAME") ) 
		strcpy( pSTDdev->ST_mds.treeName[OPMODE_REMOTE], value );
	else if ( !strcmp(filter, "REMOTE_EVENT") ) 
		strcpy( pSTDdev->ST_mds.treeEventIP[OPMODE_REMOTE], value );
	
	else if( !strcmp(filter, "CAL_IP") ) 
		strcpy( pSTDdev->ST_mds.treeIPport[OPMODE_CALIBRATION], value );
	else if ( !strcmp(filter, "CAL_NAME") ) 
		strcpy( pSTDdev->ST_mds.treeName[OPMODE_CALIBRATION], value );
	else if ( !strcmp(filter, "CAL_EVENT") ) 
		strcpy( pSTDdev->ST_mds.treeEventIP[OPMODE_CALIBRATION], value );
	else {
		notify_error(ERR_SCN_PNT, "%s: no filter \"%s\"", pSTDdev->taskName, filter );
		return WR_ERROR;
	}

	return WR_OK;
}

int mds_copy_master_to_STD(ST_STD_device *pSTDdev)
{
	ST_MASTER *pMaster = get_master();
	
	memcpy( &pSTDdev->ST_mds , &pMaster->ST_mds, sizeof(ST_MDS_PLUS) );
#if 0
	for( i = 0; i< CNT_OPMODE; i++) {
		strcpy(pSTDdev->ST_mds.treeName[i] , pMaster->ST_mds.treeName[i] );
		strcpy(pSTDdev->ST_mds.treeIPport[i] , pMaster->ST_mds.treeIPport[i] );
		strcpy(pSTDdev->ST_mds.treeEventIP[i] , pMaster->ST_mds.treeEventIP[i] );
	}
	strcpy(pSTDdev->ST_mds.treeEventArg , pMaster->ST_mds.treeEventArg );
#endif
/*
	printf("local tree: %s, ip: %s\n", pSTDdev->ST_mds.treeName[OPMODE_LOCAL] , pSTDdev->ST_mds.treeIPport[OPMODE_LOCAL] );
	printf("remote tree: %s, ip: %s\n", pSTDdev->ST_mds.treeName[OPMODE_REMOTE] , pSTDdev->ST_mds.treeIPport[OPMODE_REMOTE] );
	printf("calib tree: %s, ip: %s\n", pSTDdev->ST_mds.treeName[OPMODE_CALIBRATION] , pSTDdev->ST_mds.treeIPport[OPMODE_CALIBRATION] );
*/
	return WR_OK;
}



