/*
first created by woong 

2009. 08. 24 major modified.

2009. 08. 25 
- int ca_Put_StrVal(chid &chidChannelToPV,  char *strVal);
+ int ca_Put_StrVal(chid &chidChannelToPV,  const char *strVal);

+ int cvt_Double_to_String(double dval, char *buf)

2009. 08. 26 
+ ECA_NORDACCESS, ECA_NOWTACCESS

2009. 08.31
+ add initialized variable.. in connect STPV function.

2010.3.30
ca_Put_Double()....
+ iCAStatus = ECA_BADTYPE;

*/

#include "epicsFunc.h"
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/


int ca_Connect( char *strPVname, chid &chidChannelToPV)
{
	int iCAStatus = ca_create_channel( strPVname, 0, 0, 0, &chidChannelToPV);
	if( iCAStatus != ECA_NORMAL){
		return iCAStatus;
	}
	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL){
		return iCAStatus;
	}
	return iCAStatus;
}


int ca_ConnectCB( char *strPVname, chid &chidChannelToPV, callbackData cbd)
{
	int iCAStatus = ca_create_channel( strPVname, cbd.pFunc, 0, 0, &chidChannelToPV);
	if( iCAStatus != ECA_NORMAL){
		return iCAStatus;
	}
	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL){
		return iCAStatus;
	}
	return iCAStatus;
}

int ca_Disconnect(chid &chidChannelToPV)
{
	int iCAStatus = ca_clear_channel( chidChannelToPV);
	if( iCAStatus != ECA_NORMAL)
		return iCAStatus;

	chidChannelToPV = NULL;

	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL)
		return iCAStatus;

	return iCAStatus;
}


/************************************************************************************
modified 2009. 07. 24 



*************************************************************************************/
int ca_Connect_STPV(ST_PV_NODE &myPV, VOIDFUNCPTR ptrFunc)
{
	int iCAStatus;
	myPV.flag_cid = 0;
	myPV.flag_eid = 0;

	iCAStatus = ca_create_channel( myPV.name, (caCh*)ptrFunc, 0, 0, &myPV.cid);
	if( iCAStatus != ECA_NORMAL){
		return iCAStatus;
	}
	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL){
		return ECA_TIMEOUT;
	}
//	myPV.flag_cid = 1;		// be set in the callback function.
	return iCAStatus;
}

int ca_Disconnect_STPV( ST_PV_NODE &myPV)
{
/* if enable this, must check flag in the connection callback function. */
/*	if( !myPV.flag_cid) return ECA_BADCHID;		*/
	
	int iCAStatus = ca_clear_channel( myPV.cid);
	if( iCAStatus != ECA_NORMAL)
		return iCAStatus;
	myPV.cid = NULL;

	iCAStatus = ca_pend_io( 1.0 );
	if( iCAStatus != ECA_NORMAL)
		return ECA_TIMEOUT;
	
	myPV.flag_cid = 0;

	return iCAStatus;
}

int ca_Get_QStr(chid &chidChannelToPV, QString &qstrVal)
{
	int iCAStatus = ECA_NORMAL;
	unsigned long nelm;

	if( (iCAStatus=ca_read_access( chidChannelToPV)) != ECA_NORMAL ){
		return ECA_NORDACCESS;
	}
	nelm = ca_element_count( chidChannelToPV);

	long lTotalLength = dbr_size_n( dbf_type_to_DBR_TIME( ca_field_type( chidChannelToPV) ),  nelm);
	void * AvGetData = new char [ lTotalLength];
	if( AvGetData == NULL){ 
		iCAStatus = ECA_ALLOCMEM;
		return iCAStatus;
	}
	iCAStatus = ca_array_get( dbf_type_to_DBR_TIME( ca_field_type( chidChannelToPV) ),
			nelm, chidChannelToPV, AvGetData);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvGetData;
		return iCAStatus;
	}
	double dTimeout = 1.0;
	iCAStatus = ca_pend_io( dTimeout);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvGetData;
		return ECA_TIMEOUT;
	}

	union db_access_val * pUData = ( db_access_val *) AvGetData;

//	ui.lineEditGetPvAlarmStatus->setText( alarmStatusString[ pUData->tstrval.status] );
//	ui.lineEditGetPvAlarmSeverity->setText( alarmStatusString[ pUData->tstrval.severity] );
//	ui.labelGetPvTimestamp->setText( QString( "%1.%2")
//			.arg( pUData->tstrval.stamp.secPastEpoch)
//			.arg( pUData->tstrval.stamp.nsec) );
//	char AcTimestampString[ 40];
//	epicsTimeToStrftime ( AcTimestampString, sizeof( AcTimestampString),
//		"%a %b/%d/%Y %H:%M:%S.%6f", & pUData->tstrval.stamp);
//	ui.lineEditGetPvTimestamp->setText( AcTimestampString);

	for( unsigned long ulSample = 0; ulSample < nelm; ulSample++){
		switch( ca_field_type( chidChannelToPV) ){
			case DBF_STRING:
//				QMessageBox::information(0, "Kstar Viewer", QString( "%1").arg( *( &( pUData->tstrval.value) + ulSample) ) );
				qstrVal = QString( "%1").arg( *( &( pUData->tstrval.value) + ulSample) );
//				fprintf(stdout, ">>> get STRING value: %s\n", *( &( pUData->tstrval.value) + ulSample) );
//				textEdit_status->insertPlainText( buf );
				break;
			case DBF_SHORT: //case DBF_INT:
//				QMessageBox::information(0, "Kstar Viewer", QString( "%1").arg( *( &( pUData->tshrtval.value) + ulSample) ) );
				qstrVal = QString( "%1").arg( *( &( pUData->tshrtval.value) + ulSample) ) ;
//				sprintf(buf, ">>> get SHORT value: %d\n", *( &( pUData->tshrtval.value) + ulSample) );
//				textEdit_status->insertPlainText( buf );
				break;
			case DBF_FLOAT:
//				QMessageBox::information(0, "Kstar Viewer", QString( "%1").arg( *( &( pUData->tfltval.value) + ulSample) ) );
				qstrVal = QString( "%1").arg( *( &( pUData->tfltval.value) + ulSample) );
//				sprintf(buf, ">>> get FLOAT value: %f\n", *( &( pUData->tfltval.value) + ulSample) );
//				textEdit_status->insertPlainText( buf );
				break;
			case DBF_ENUM:
//				QMessageBox::information(0, "Kstar Viewer", QString( "%1").arg( *( &( pUData->tenmval.value) + ulSample) ) );
				qstrVal = QString( "%1").arg( *( &( pUData->tenmval.value) + ulSample) );
//				sprintf(buf, ">>> get ENUM value: %d\n", *( &( pUData->tenmval.value) + ulSample) );
//				textEdit_status->insertPlainText( buf );
				break;
			case DBF_CHAR:
//				QMessageBox::information(0, "Kstar Viewer", QString( "%1").arg( *( &( pUData->tchrval.value) + ulSample) ) );
				qstrVal = QString( "%1").arg( *( &( pUData->tchrval.value) + ulSample) );
//				sprintf(buf, ">>> get CHAR value: %c\n", *( &( pUData->tchrval.value) + ulSample) );
//				textEdit_status->insertPlainText( buf );
				break;
			case DBF_LONG:
//				QMessageBox::information(0, "Kstar Viewer", QString( "%1").arg( *( &( pUData->tlngval.value) + ulSample) ) );
				qstrVal = QString( "%1").arg( *( &( pUData->tlngval.value) + ulSample) );
//				sprintf(buf, ">>> get LONG value: %d\n", *( &( pUData->tlngval.value) + ulSample) );
//				textEdit_status->insertPlainText( buf );
				break;
			case DBF_DOUBLE:
//				QMessageBox::information(0, "Kstar Viewer", QString( "%1").arg( *( &( pUData->tdblval.value) + ulSample) ) );
				qstrVal = QString( "%1").arg( *( &( pUData->tdblval.value) + ulSample) );
//				fprintf(stdout, ">>> get DOUBLE value: 0x%x\n", (double)(*( &( pUData->tdblval.value) + ulSample)) );
//				textEdit_status->insertPlainText( buf );
				break;
//			case DBF_NO_ACCESS:
//				break;
			default:
				iCAStatus = ECA_BADTYPE;
		}
	}
//	if( iCAStatus != ECA_NORMAL) {
//		delete [] ( char *) AvGetData;
//		return iCAStatus;
//	} 

	delete [] ( char *) AvGetData;

	return iCAStatus;
}

int ca_Put_QStr(chid &chidChannelToPV, QString oQString)
{
	int iCAStatus = ECA_NORMAL;
	unsigned long nelm;

	if( (iCAStatus = ca_write_access( chidChannelToPV) ) != ECA_NORMAL ){
		return ECA_NOWTACCESS;
	}

	nelm = ca_element_count( chidChannelToPV);
	long lTotalLength = dbr_size_n( dbf_type_to_DBR( ca_field_type( chidChannelToPV) ), nelm );
	void * AvPutData = new char [ lTotalLength];
	if( AvPutData == NULL){ 
		iCAStatus = ECA_ALLOCMEM;
		return iCAStatus;
	}
	union db_access_val * pUData = ( db_access_val *) AvPutData;

	for( unsigned long ulSample = 0; ulSample < nelm; ulSample++)
	{		
		switch( ca_field_type( chidChannelToPV) ){
			case DBF_STRING:
				strncpy( *( &( pUData->strval) + ulSample), oQString.toAscii().constData(), MAX_STRING_SIZE);
				break;
			case DBF_SHORT: //case DBF_INT:
				*( &( pUData->shrtval) + ulSample) = oQString.toShort();
				break;
			case DBF_FLOAT:
				*( &( pUData->fltval) + ulSample) = oQString.toFloat();
				break;
			case DBF_ENUM:
				*( &( pUData->enmval) + ulSample) = oQString.toShort();
				break;
			case DBF_CHAR:
				*( &( pUData->charval) + ulSample) = ( char) oQString.toShort();
				break;
			case DBF_LONG:
				*( &( pUData->longval) + ulSample) = oQString.toLong();
				break;
			case DBF_DOUBLE:
				*( &( pUData->doubleval) + ulSample) = oQString.toDouble();
				break;
//			case DBF_NO_ACCESS:
//				break;
			default:
				iCAStatus = ECA_BADTYPE;
		}
	}

	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvPutData;
		return iCAStatus;
	}

	iCAStatus = ca_array_put( dbf_type_to_DBR( ca_field_type( chidChannelToPV) ), 
			nelm, chidChannelToPV, AvPutData);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvPutData;
		return iCAStatus;
	}
	double dTimeout = 1.0;
	iCAStatus = ca_pend_io( dTimeout);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvPutData;
		return ECA_TIMEOUT;
	}

	delete [] ( char *) AvPutData;

	return iCAStatus;
}


int  ca_Get_StrVal(chid &chidChannelToPV,  char *strVal)
{
	unsigned long nelm;
	double dVal;
	int iCAStatus = ECA_NORMAL;

	if( ca_read_access( chidChannelToPV) == 0 ){
		return ECA_NORDACCESS;
	}
	nelm = ca_element_count( chidChannelToPV);

	long lTotalLength = dbr_size_n( dbf_type_to_DBR_TIME( ca_field_type( chidChannelToPV) ), nelm );
	void * AvGetData = new char [ lTotalLength];
	if( AvGetData == NULL){ 
		iCAStatus = ECA_ALLOCMEM;
		return iCAStatus;
	}
	iCAStatus = ca_array_get( dbf_type_to_DBR_TIME( ca_field_type( chidChannelToPV) ),
			nelm, chidChannelToPV, AvGetData);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvGetData;
		return iCAStatus;
	}
	iCAStatus = ca_pend_io( 1.0);
	if( iCAStatus != ECA_NORMAL){	
		delete [] ( char *) AvGetData;
		return ECA_TIMEOUT;
	}

	union db_access_val * pUData = ( db_access_val *) AvGetData;
/*
//	ui.lineEditGetPvAlarmStatus->setText( alarmStatusString[ pUData->tstrval.status] );
//	ui.lineEditGetPvAlarmSeverity->setText( alarmStatusString[ pUData->tstrval.severity] );
//	ui.labelGetPvTimestamp->setText( QString( "%1.%2")
//			.arg( pUData->tstrval.stamp.secPastEpoch)
//			.arg( pUData->tstrval.stamp.nsec) );
//	char AcTimestampString[ 40];
//	epicsTimeToStrftime ( AcTimestampString, sizeof( AcTimestampString),
//		"%a %b/%d/%Y %H:%M:%S.%6f", & pUData->tstrval.stamp);
//	ui.lineEditGetPvTimestamp->setText( AcTimestampString);
*/
	for( unsigned long ulSample = 0; ulSample < nelm; ulSample++){
		switch( ca_field_type( chidChannelToPV) ){
			case DBF_STRING:
				sprintf(strVal, "%s", *( &( pUData->tstrval.value) + ulSample) );
				break;
			case DBF_SHORT: //case DBF_INT:
				sprintf(strVal, "%d", *( &( pUData->tshrtval.value) + ulSample) );
				break;
			case DBF_FLOAT:
				sprintf(strVal, "%f", *( &( pUData->tfltval.value) + ulSample) );
				break;
			case DBF_ENUM:
				sprintf(strVal, "%d", *( &( pUData->tenmval.value) + ulSample) );
				break;
			case DBF_CHAR:
				sprintf(strVal, "%c", *( &( pUData->tchrval.value) + ulSample) );
				break;
			case DBF_LONG:
				sprintf(strVal, "%d", *( &( pUData->tlngval.value) + ulSample) );
				break;
			case DBF_DOUBLE:
				dVal = *( &( pUData->tdblval.value) + ulSample) ;
				cvt_Double_to_String( dVal, strVal);
				break;
//			case DBF_NO_ACCESS:
//				break;
			default:
				iCAStatus = ECA_BADTYPE;
		}
	}

	delete [] ( char *) AvGetData;

	return iCAStatus;
}


int ca_Put_StrVal(chid &chidChannelToPV,  const char *strVal)
{
	int iCAStatus = ECA_NORMAL;
	unsigned long nelm;
	char *endp;

	if( (iCAStatus = ca_write_access( chidChannelToPV) ) != ECA_NORMAL ){
		return ECA_NOWTACCESS;
	}

	nelm = ca_element_count( chidChannelToPV);

	long lTotalLength = dbr_size_n( dbf_type_to_DBR( ca_field_type( chidChannelToPV) ), nelm );

	void * AvPutData = new char [ lTotalLength];
	if( AvPutData == NULL){ 
		iCAStatus = ECA_ALLOCMEM;
		return iCAStatus;
	}
	union db_access_val * pUData = ( db_access_val *) AvPutData;
	for( unsigned long ulSample = 0; ulSample < nelm; ulSample++){
		switch( ca_field_type( chidChannelToPV) ){
			case DBF_STRING:
				strncpy( *( &( pUData->strval) + ulSample), strVal, MAX_STRING_SIZE);
				break;
			case DBF_SHORT: //case DBF_INT:
				*( &( pUData->shrtval) + ulSample) = (short) atoi( strVal );
				break;
			case DBF_FLOAT:
				*( &( pUData->fltval) + ulSample) = (float) atof( strVal ) ; //+ ulSample; //     ulSample  .
				break;
			case DBF_ENUM:
				*( &( pUData->enmval) + ulSample) = (int) atoi( strVal );
				break;
			case DBF_CHAR:
				*( &( pUData->charval) + ulSample) =  *strVal;
				break;
			case DBF_LONG:
				*( &( pUData->longval) + ulSample) = atol( strVal );
				break;
			case DBF_DOUBLE:
				*( &( pUData->doubleval) + ulSample) = strtod( strVal, &endp);
				break;
			default:
				iCAStatus = ECA_BADTYPE;
		}
	}


	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvPutData;
		return iCAStatus;
	}

	iCAStatus = ca_array_put( dbf_type_to_DBR( ca_field_type( chidChannelToPV) ), 
			nelm, chidChannelToPV, AvPutData);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvPutData;
		return iCAStatus;
	}
	double dTimeout = 1.0;
	iCAStatus = ca_pend_io( dTimeout);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvPutData;
		return ECA_TIMEOUT;
	}

	delete [] ( char *) AvPutData;

	return iCAStatus;
}


int ca_Put_Double(chid &chidChannelToPV, double dval)
{
	int iCAStatus = ECA_NORMAL;
	unsigned long nelm;

	if( (iCAStatus = ca_write_access( chidChannelToPV) ) != ECA_NORMAL ){
		return ECA_NOWTACCESS;
	}

	nelm = ca_element_count( chidChannelToPV);

	if( ca_field_type(chidChannelToPV) != DBF_DOUBLE ) {
		iCAStatus = ECA_BADTYPE;
		return iCAStatus;
	}
	long lTotalLength = dbr_size_n( dbf_type_to_DBR( ca_field_type( chidChannelToPV) ), nelm );

	void * AvPutData = new char [ lTotalLength];
	if( AvPutData == NULL){ 
		iCAStatus = ECA_ALLOCMEM;
		return iCAStatus;
	}
	union db_access_val * pUData = ( db_access_val *) AvPutData;
	
	*( &( pUData->doubleval) ) = dval;
	
	iCAStatus = ca_array_put( dbf_type_to_DBR( ca_field_type( chidChannelToPV) ), 
			nelm, chidChannelToPV, AvPutData);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvPutData;
		return iCAStatus;
	}
//	printf("1\n");

	iCAStatus = ca_pend_io( 1.0);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvPutData;
		return ECA_TIMEOUT;
	}
//	printf("2\n");
	
	delete [] ( char *) AvPutData;
	return iCAStatus;

}

int  ca_Get_Double(chid &chidChannelToPV, double &dval)
{
	int iCAStatus = ECA_NORMAL;
	unsigned long nelm;

	if( (iCAStatus=ca_read_access( chidChannelToPV)) != ECA_NORMAL ){
		return ECA_NORDACCESS;
	}
	nelm = ca_element_count( chidChannelToPV);

	long lTotalLength = dbr_size_n( dbf_type_to_DBR_TIME( ca_field_type( chidChannelToPV) ), nelm );
	void * AvGetData = new char [ lTotalLength];
	if( AvGetData == NULL){ 
		iCAStatus = ECA_ALLOCMEM;
		return iCAStatus;
	}
	iCAStatus = ca_array_get( dbf_type_to_DBR_TIME( ca_field_type( chidChannelToPV) ),
			nelm, chidChannelToPV, AvGetData);
	if( iCAStatus != ECA_NORMAL){
		delete [] ( char *) AvGetData;
		return iCAStatus;
	}
	iCAStatus = ca_pend_io( 1.0);
	if( iCAStatus != ECA_NORMAL){	
		delete [] ( char *) AvGetData;
		return ECA_TIMEOUT;
	}

	union db_access_val * pUData = ( db_access_val *) AvGetData;

	for( unsigned long ulSample = 0; ulSample < nelm; ulSample++){
		switch( ca_field_type( chidChannelToPV) ){
			
			case DBF_DOUBLE:
				dval = *( &( pUData->tdblval.value) + ulSample);
				break;
			default:
				iCAStatus = ECA_BADTYPE;
		}
	}

	delete [] ( char *) AvGetData;
	return iCAStatus;
}



int cvt_Double_to_String(double dval, char *buf)
{
	int len;

	sprintf(buf, "%.12f", dval);	
	len = strlen(buf);
	for (int i=len; i>0; i--) 
	{
		if( buf[i-1] == '0' ) 
		{
//			buf[i-1] = '\n';
			buf[i-1] = 0L;
			if( buf[i-2] == '.' )
			{
				buf[i-2] = 0L;
				break;
			}
		} else {
			buf[i] = 0L;
			break;
		}
	}

//	printf("buf: %s\n", buf);
	
	return ECA_NORMAL;
}







