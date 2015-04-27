/****************************************************************************

Module      : sfwMDSplus.c

Copyright(c): 2010 NFRI. All Rights Reserved.

Revision History:
Author: woong   2010-4-16

2010. 7. 7  arrangement.


*****************************************************************************/

#include <math.h>
#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>


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


int mds_Connect(ST_STD_device *pSTDdev)
{
	int op = pSTDdev->ST_Base.opMode;
	pSTDdev->socket = MdsConnect(pSTDdev->treeIPport[op]);
	if ( pSTDdev->socket == -1 )
	{
		fprintf(stderr, "%s: Error connecting to mdsip server(%s).\n", pSTDdev->taskName, pSTDdev->treeIPport[op]);
		printlog("%s: Error connecting to mdsip server(%s).\n", pSTDdev->taskName, pSTDdev->treeIPport[op]);
		return WR_ERROR;
	}
	pSTDdev->StatusMds |= MDS_CONNECTED;
#if PRINT_MDS_CONNECTION
	printf("%s: Tree Connect(\"%s\"):%s (0x%X)... OK\n", pSTDdev->taskName, pSTDdev->treeIPport[op], pSTDdev->treeName[op], pSTDdev->StatusMds);
#endif
	return WR_OK;
}

int mds_Open(ST_STD_device *pSTDdev)
{
	return fnc_open(pSTDdev, pSTDdev->ST_Base.shotNumber);
}
int mds_Open_withFetch(ST_STD_device *pSTDdev)
{
	return fnc_open(pSTDdev, pSTDdev->ST_mds_fetch.shotNumber);
}

int mds_Close(ST_STD_device *pSTDdev)
{
	return fnc_close(pSTDdev, pSTDdev->ST_Base.shotNumber);
}
int mds_Close_withFetch(ST_STD_device *pSTDdev)
{
	return fnc_close(pSTDdev, pSTDdev->ST_mds_fetch.shotNumber);
}

int mds_Disconnect(ST_STD_device *pSTDdev)
{
	if( pSTDdev->StatusMds & MDS_OPEN_OK ){
		epicsPrintf("ERROR! %s: must be Closed! \n", pSTDdev->taskName);
		return WR_ERROR;
	}
/*TG
	int op = pSTDdev->ST_Base.opMode;
	MdsDisconnect(pSTDdev->treeIPport[op]);  */
	MdsDisconnect();
	pSTDdev->StatusMds &= ~MDS_CONNECTED;
#if PRINT_MDS_CONNECTION
	printf("%s: Tree Disconnect (0x%X)...  OK\n", pSTDdev->taskName, pSTDdev->StatusMds );
#endif
	return WR_OK;	
}

int mds_createNewShot(ST_STD_device *pSTDdev)
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int shot;
	int op;
	int tstat, len;
	char buf[128];
	int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an	integer */
	int idesc = descr(&dtype_Long, &tstat, &null);


	shot = (int)pSTDdev->ST_Base.shotNumber;
	op = pSTDdev->ST_Base.opMode;

	if( op == OPMODE_REMOTE ) {
		epicsPrintf("ERROR! %s: Must be Test mode run!!\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	if( !(pSTDdev->StatusMds & MDS_CONNECTED) ){
		epicsPrintf("ERROR! %s: MDS tree not connected: 0x%X\n", pSTDdev->taskName, pSTDdev->StatusMds );
		return WR_ERROR;
	}

		sprintf(buf, "TCL(\"set tree %s/shot=-1\")", pSTDdev->treeName[op]);
		status = MdsValue(buf, &idesc, &null, &len);
		if ( !((status) & 1) )
		{
			epicsPrintf("Error TCL command: %s.\n",MdsGetMsg(status));
		/*	MdsDisconnect(pSTDdev->treeIPport[op]); TG  */
			MdsDisconnect();
			return WR_ERROR;
		}
		sprintf(buf, "TCL(\"create pulse %d\")",(int)shot);
		status = MdsValue(buf, &idesc, &null, &len);
		if ( !((status) & 1) )
		{
			epicsPrintf("Error TCL command: create pulse: %s.\n",MdsGetMsg(status));
			/* TG MdsDisconnect(pSTDdev->treeIPport[op]);  */
			MdsDisconnect();
			return WR_ERROR;
		}
		status = MdsValue("TCL(\"close\")", &idesc, &null, &len);
		
		epicsPrintf("\%s: Create new tree(\"%d\")... OK\n", pSTDdev->taskName,(int)shot);

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
/*
	socket = MdsConnect(pSTDdev->treeEventIP[op]);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server(%s).\n", pSTDdev->treeEventIP[op]);
		return WR_ERROR;
	}

	sprintf(buf, "TCL(\"setevent ADMIN\")");
	status = MdsValue(buf, &idesc, &null, &len);
	if ( !((status) & 1) )
	{
		epicsPrintf("Error TCL command: %s.\n",MdsGetMsg(status));
		MdsDisconnect();
		return WR_ERROR;
	}

	MdsDisconnect();
*/
	epicsPrintf("Spawn event: \"%s\", tree put completed!\n", pSTDdev->treeEventIP[op]);
	return WR_OK;
}

int fnc_open(ST_STD_device *pSTDdev, int shot)
{
	int status; /* Will contain the status of the data access operation */
	int op = pSTDdev->ST_Base.opMode;

	if( pSTDdev->StatusMds & MDS_CONNECTED ){
		status = MdsOpen(pSTDdev->treeName[op], &shot);
		if ( !((status) & 1) )
		{
			fprintf(stderr,"%s: Error opening \"%s\" for shot %d: %s.\n", pSTDdev->taskName, pSTDdev->treeName[op],(int)shot, MdsGetMsg(status));
			printlog("%s: Error opening tree for shot %d: %s.\n", pSTDdev->taskName, shot, MdsGetMsg(status));
			/* TG MdsDisconnect(pSTDdev->treeIPport[op]);  */
			MdsDisconnect();
			return WR_ERROR;
		}
		pSTDdev->StatusMds |= MDS_OPEN_OK;
#if PRINT_MDS_CONNECTION		
		printf("%s: Tree Open(\"%s\", shot: %d)... OK\n", pSTDdev->taskName, pSTDdev->treeName[op], shot);
#endif
		
	} else {
		epicsPrintf("ERROR! %s: MDS tree not connected! (0x%X)\n", pSTDdev->taskName, pSTDdev->StatusMds);
		printlog("ERROR! %s: MDS tree not connected! (0x%X)\n", pSTDdev->taskName, pSTDdev->StatusMds);
		return WR_ERROR;
	}

	return WR_OK;
}

int fnc_close(ST_STD_device *pSTDdev, int shot)
{
	int status; /* Will contain the status of the data access operation */
	int op = pSTDdev->ST_Base.opMode;

	if( !(pSTDdev->StatusMds & MDS_OPEN_OK) ){
		epicsPrintf("ERROR! %s: tree \"%s\" shot \"%d\" not opened: 0x%X\n", pSTDdev->taskName, pSTDdev->treeName[op],(int)shot, pSTDdev->StatusMds);
		return WR_ERROR;
	}
	
	status = MdsClose(pSTDdev->treeName[op], &shot);
	if ( !((status) & 1) )
	{
		epicsPrintf("\n>>> Error closing tree for shot %d: %s.\n",(int)shot, MdsGetMsg(status));
		/* TG MdsDisconnect(pSTDdev->treeIPport[op]);  */
		MdsDisconnect();
		return WR_ERROR;
	}
	pSTDdev->StatusMds &= ~MDS_OPEN_OK;
#if PRINT_MDS_CONNECTION
	printf("%s: Tree Close shot \"%d\" (0x%X)...  OK\n", pSTDdev->taskName, shot,  pSTDdev->StatusMds);
#endif
	return WR_OK;
}


int mds_assign_from_Env(ST_STD_device *pSTDdev)
{
	if( getenv("MDS_LOCAL_IP_PORT") ){
		strcpy( pSTDdev->treeIPport[OPMODE_LOCAL], getenv("MDS_LOCAL_IP_PORT") );
	} else strcpy( pSTDdev->treeIPport[OPMODE_LOCAL], "Oops!" );
	if( getenv("MDS_LOCAL_TREE_NAME") ){
		strcpy( pSTDdev->treeName[OPMODE_LOCAL], getenv("MDS_LOCAL_TREE_NAME") );
	} else strcpy( pSTDdev->treeName[OPMODE_LOCAL], "Oops!" );
	if( getenv("MDS_LOCAL_EVENT_IP") ){
		strcpy( pSTDdev->treeEventIP[OPMODE_LOCAL], getenv("MDS_LOCAL_EVENT_IP") );
	} else strcpy( pSTDdev->treeEventIP[OPMODE_LOCAL], "Oops!" );

	if( getenv("MDS_REMOTE_IP_PORT") ){
		strcpy( pSTDdev->treeIPport[OPMODE_REMOTE], getenv("MDS_REMOTE_IP_PORT") );
	} else strcpy( pSTDdev->treeIPport[OPMODE_REMOTE], "Oops!" );
	if( getenv("MDS_REMOTE_TREE_NAME") ){
		strcpy( pSTDdev->treeName[OPMODE_REMOTE], getenv("MDS_REMOTE_TREE_NAME") );
	} else strcpy( pSTDdev->treeName[OPMODE_REMOTE], "Oops!" );
	if( getenv("MDS_REMOTE_EVENT_IP") ){
		strcpy( pSTDdev->treeEventIP[OPMODE_REMOTE], getenv("MDS_REMOTE_EVENT_IP") );
	} else strcpy( pSTDdev->treeEventIP[OPMODE_REMOTE], "Oops!" );

	if( getenv("MDS_CAL_IP_PORT") ){
		strcpy( pSTDdev->treeIPport[OPMODE_CALIBRATION], getenv("MDS_CAL_IP_PORT") );
	} else strcpy( pSTDdev->treeIPport[OPMODE_CALIBRATION], "Oops!" );
	if( getenv("MDS_CAL_TREE_NAME") ){
		strcpy( pSTDdev->treeName[OPMODE_CALIBRATION], getenv("MDS_CAL_TREE_NAME") );
	} else strcpy( pSTDdev->treeName[OPMODE_CALIBRATION], "Oops!" );
	if( getenv("MDS_CAL_EVENT_IP") ){
		strcpy( pSTDdev->treeEventIP[OPMODE_CALIBRATION], getenv("MDS_CAL_EVENT_IP") );
	} else strcpy( pSTDdev->treeEventIP[OPMODE_CALIBRATION], "Oops!" );

	strcpy( pSTDdev->treeIPport[OPMODE_INIT], "Init. IP");
	strcpy( pSTDdev->treeName[OPMODE_INIT], "Init. name");
	strcpy( pSTDdev->treeEventIP[OPMODE_INIT], "Init. event");


#if 0
{
	int i;
	for( i=0; i<CNT_OPMODE; i++) {
		epicsPrintf("%s: %s, ", pSTDdev->taskName, pSTDdev->treeIPport[i] );
		epicsPrintf(" %s, ", pSTDdev->treeName[i] );		
		epicsPrintf(" %s\n", pSTDdev->treeEventIP[i] );
	}
}
#endif

	return WR_OK;
}

int callFunc_set_MDSplus(ST_STD_device *pSTDdev, char *filter, char *value)
{
	if( !strcmp(filter, "LOCAL_IP") ) 
		strcpy( pSTDdev->treeIPport[OPMODE_LOCAL], value );
	else if ( !strcmp(filter, "LOCAL_NAME") )
		strcpy( pSTDdev->treeName[OPMODE_LOCAL], value );
	else if ( !strcmp(filter, "LOCAL_EVENT") ) 
		strcpy( pSTDdev->treeEventIP[OPMODE_LOCAL], value );

	else if( !strcmp(filter, "REMOTE_IP") ) 
		strcpy( pSTDdev->treeIPport[OPMODE_REMOTE], value );
	else if ( !strcmp(filter, "REMOTE_NAME") ) 
		strcpy( pSTDdev->treeName[OPMODE_REMOTE], value );
	else if ( !strcmp(filter, "REMOTE_EVENT") ) 
		strcpy( pSTDdev->treeEventIP[OPMODE_REMOTE], value );
	
	else if( !strcmp(filter, "CAL_IP") ) 
		strcpy( pSTDdev->treeIPport[OPMODE_CALIBRATION], value );
	else if ( !strcmp(filter, "CAL_NAME") ) 
		strcpy( pSTDdev->treeName[OPMODE_CALIBRATION], value );
	else if ( !strcmp(filter, "CAL_EVENT") ) 
		strcpy( pSTDdev->treeEventIP[OPMODE_CALIBRATION], value );
	else {
		notify_admin_error_info(1, "%s: no filter \"%s\"", pSTDdev->taskName, filter );
		return WR_ERROR;
	}

	return WR_OK;
}



