#include <QtGui>

#include "pvST_mng.h"

#include "mainWindow.h"



Cpvst_mng::Cpvst_mng( void *parent)
{
	proot = (MainWindow*)parent;

}

/*
CqueueThread::CqueueThread( QObject *parent)
     : QThread(parent)
{
	proot = (MainWindow*)parent;
}
*/

Cpvst_mng::~Cpvst_mng()
{

}

void Cpvst_mng::temp_slot()
{
	printf("this is temp_slot() \n");
}
void Cpvst_mng::print_return_msg(ST_QUEUE_STATUS qnode)
{
	MainWindow *pMain = (MainWindow *)proot;

	switch( qnode.pEventArg->type )
	{
	case TYPE_SINGLE_TRIG:
		if( pMain->pvCLTU[qnode.pEventArg->system].POL[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].POL[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
		break;
	case TYPE_SINGLE_T0:
		if( pMain->pvCLTU[qnode.pEventArg->system].T0[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].T0[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
		break;
	case TYPE_SINGLE_T1:
		if( pMain->pvCLTU[qnode.pEventArg->system].T1[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].T1[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);

		break;
	case  TYPE_SINGLE_CLOCK:
		if( pMain->pvCLTU[qnode.pEventArg->system].CLOCK[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].CLOCK[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
		break;
	case  TYPE_SINGLE_SETUP:
		if( pMain->pvCLTU[qnode.pEventArg->system].SETUP_Pt[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].SETUP_Pt[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;

	case TYPE_MULTI_CLOCK:
		if( pMain->pvCLTU[qnode.pEventArg->system].mCLOCK[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].mCLOCK[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);

		break;
	case TYPE_MULTI_TRIG:
		if( pMain->pvCLTU[qnode.pEventArg->system].mTRIG.status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].mTRIG.status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
		break;
	case TYPE_MULTI_T0:
		if( pMain->pvCLTU[qnode.pEventArg->system].mT0[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].mT0[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
		break;
	case TYPE_MULTI_T1:
		if( pMain->pvCLTU[qnode.pEventArg->system].mT1[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].mT1[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
		break;
	case TYPE_MULTI_SETUP:
		if( pMain->pvCLTU[qnode.pEventArg->system].SETUP_Pt[4].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].SETUP_Pt[4].status_flag &= ~PV_STATUS_TOUCH;
		} 
		else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;
/**************************************************************/
/*   In case of New R2 */
	case  TYPE_R2_ActiveLow:
		if( pMain->pvCLTU[qnode.pEventArg->system].R2_ActiveLow[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%f)\n", qnode.pEventArg->pvname, qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].R2_ActiveLow[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;
	case  TYPE_R2_Enable:
		if( pMain->pvCLTU[qnode.pEventArg->system].R2_Enable[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].R2_Enable[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;
	case  TYPE_R2_Mode:
		if( pMain->pvCLTU[qnode.pEventArg->system].R2_Mode[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].R2_Mode[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;
	case  TYPE_R2_CLOCK:
		if( pMain->pvCLTU[qnode.pEventArg->system].R2_CLOCK[qnode.pEventArg->port][qnode.pEventArg->arg].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].R2_CLOCK[qnode.pEventArg->port][qnode.pEventArg->arg].status_flag &= ~PV_STATUS_TOUCH;
		} else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;
	case  TYPE_R2_T0:
		if( pMain->pvCLTU[qnode.pEventArg->system].R2_T0[qnode.pEventArg->port][qnode.pEventArg->arg].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].R2_T0[qnode.pEventArg->port][qnode.pEventArg->arg].status_flag &= ~PV_STATUS_TOUCH;
		} else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;
	case  TYPE_R2_T1:
		if( pMain->pvCLTU[qnode.pEventArg->system].R2_T1[qnode.pEventArg->port][qnode.pEventArg->arg].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].R2_T1[qnode.pEventArg->port][qnode.pEventArg->arg].status_flag &= ~PV_STATUS_TOUCH;
		} else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;
	case  TYPE_R2_SETUP:
		if( pMain->pvCLTU[qnode.pEventArg->system].SETUP_Pt[qnode.pEventArg->port].status_flag & PV_STATUS_TOUCH ) {
			pMain->printReturn(LEVEL_NOTICE, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
			pMain->pvCLTU[qnode.pEventArg->system].SETUP_Pt[qnode.pEventArg->port].status_flag &= ~PV_STATUS_TOUCH;
		} else 
			pMain->printReturn(LEVEL_WARNING, " \"%s\":(%d)\n", qnode.pEventArg->pvname, (int)qnode.dval);
		break;
		
	default: break;
	}

}

int Cpvst_mng::check_is_first_event(ST_ARG_EVENT_CB *pEventArg)
{
	MainWindow *pMain = (MainWindow *)proot;
	int nval = -1;
	
	switch( pEventArg->type )
		{
		case TYPE_SINGLE_TRIG:
			if( pMain->pvCLTU[pEventArg->system].POL[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].POL[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			}  else  nval = 0;
			break;
		case TYPE_SINGLE_T0:
			if( pMain->pvCLTU[pEventArg->system].T0[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].T0[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else  nval = 0;
			break;
		case TYPE_SINGLE_T1:
			if( pMain->pvCLTU[pEventArg->system].T1[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].T1[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;	
			break;
		case  TYPE_SINGLE_CLOCK:
			if( pMain->pvCLTU[pEventArg->system].CLOCK[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].CLOCK[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_SINGLE_SETUP:
			if( pMain->pvCLTU[pEventArg->system].SETUP_Pt[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].SETUP_Pt[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
	
		case TYPE_MULTI_CLOCK:
			if( pMain->pvCLTU[pEventArg->system].mCLOCK[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].mCLOCK[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} 
			else nval = 0;	
			break;
		case TYPE_MULTI_TRIG:
			if( pMain->pvCLTU[pEventArg->system].mTRIG.status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].mTRIG.status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case TYPE_MULTI_T0:
			if( pMain->pvCLTU[pEventArg->system].mT0[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].mT0[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case TYPE_MULTI_T1:
			if( pMain->pvCLTU[pEventArg->system].mT1[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].mT1[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_MULTI_SETUP:
			if( pMain->pvCLTU[pEventArg->system].SETUP_Pt[4].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].SETUP_Pt[4].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
/**************************************************************/
/*   In case of New R2 */
		case  TYPE_R2_ActiveLow:
			if( pMain->pvCLTU[pEventArg->system].R2_ActiveLow[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].R2_ActiveLow[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_R2_Enable:
			if( pMain->pvCLTU[pEventArg->system].R2_Enable[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].R2_Enable[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_R2_Mode:
			if( pMain->pvCLTU[pEventArg->system].R2_Mode[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].R2_Mode[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_R2_CLOCK:
			if( pMain->pvCLTU[pEventArg->system].R2_CLOCK[pEventArg->port][pEventArg->arg].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].R2_CLOCK[pEventArg->port][pEventArg->arg].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_R2_T0:
			if( pMain->pvCLTU[pEventArg->system].R2_T0[pEventArg->port][pEventArg->arg].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].R2_T0[pEventArg->port][pEventArg->arg].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_R2_T1:
			if( pMain->pvCLTU[pEventArg->system].R2_T1[pEventArg->port][pEventArg->arg].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].R2_T1[pEventArg->port][pEventArg->arg].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_R2_SETUP:
			if( pMain->pvCLTU[pEventArg->system].SETUP_Pt[pEventArg->port].status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].SETUP_Pt[pEventArg->port].status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case  TYPE_R2_GET_COMP_NS:
			if( pMain->pvCLTU[pEventArg->system].GET_COMP_NS.status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].GET_COMP_NS.status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;
		case TYPE_R2_SET_FREE_RUN:
			if( pMain->pvCLTU[pEventArg->system].SET_FREE_RUN.status_flag & PV_STATUS_FIRST_EVENT ) {
				nval = 1;
				pMain->pvCLTU[pEventArg->system].SET_FREE_RUN.status_flag &= ~PV_STATUS_FIRST_EVENT;
			} else nval = 0;
			break;

			
			
		default:
			pMain->printReturn(LEVEL_ERROR, "no correlated system in check_is_first_event()");
			nval = -1;
			break;
		}
	
	return nval;
}

void Cpvst_mng::set_pv_status_delete_clear(ST_ARG_EVENT_CB * pEventArg)
{
	MainWindow *pMain = (MainWindow *)proot;

	switch( pEventArg->type )
	{
		case TYPE_SINGLE_TRIG:
			pMain->pvCLTU[pEventArg->system].POL[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case TYPE_SINGLE_T0:
			pMain->pvCLTU[pEventArg->system].T0[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case TYPE_SINGLE_T1:
			pMain->pvCLTU[pEventArg->system].T1[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_SINGLE_CLOCK:
			pMain->pvCLTU[pEventArg->system].CLOCK[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_SINGLE_SETUP:
			pMain->pvCLTU[pEventArg->system].SETUP_Pt[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
	
		case TYPE_MULTI_CLOCK:
			pMain->pvCLTU[pEventArg->system].mCLOCK[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case TYPE_MULTI_TRIG:
			pMain->pvCLTU[pEventArg->system].mTRIG.status_flag &= ~PV_STATUS_DELETE; break;
		case TYPE_MULTI_T0:
			pMain->pvCLTU[pEventArg->system].mT0[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case TYPE_MULTI_T1:
			pMain->pvCLTU[pEventArg->system].mT1[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_MULTI_SETUP:
			pMain->pvCLTU[pEventArg->system].SETUP_Pt[4].status_flag &= ~PV_STATUS_DELETE; break;
/**************************************************************/
/*   In case of New R2 */	
		case  TYPE_R2_ActiveLow:
			pMain->pvCLTU[pEventArg->system].R2_ActiveLow[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_R2_Enable:
			pMain->pvCLTU[pEventArg->system].R2_Enable[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_R2_Mode:
			pMain->pvCLTU[pEventArg->system].R2_Mode[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_R2_CLOCK:
			pMain->pvCLTU[pEventArg->system].R2_CLOCK[pEventArg->port][pEventArg->arg].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_R2_T0:
			pMain->pvCLTU[pEventArg->system].R2_T0[pEventArg->port][pEventArg->arg].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_R2_T1:
			pMain->pvCLTU[pEventArg->system].R2_T1[pEventArg->port][pEventArg->arg].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_R2_SETUP:
			pMain->pvCLTU[pEventArg->system].SETUP_Pt[pEventArg->port].status_flag &= ~PV_STATUS_DELETE; break;
		case  TYPE_R2_GET_COMP_NS:
			pMain->pvCLTU[pEventArg->system].GET_COMP_NS.status_flag &= ~PV_STATUS_DELETE; break;
		
		default:
			pMain->printReturn(LEVEL_ERROR, "no correlated system in set_pv_status_delete_clear()");
			break;
	}
}
	
	

int Cpvst_mng::get_pv_status_flag(ST_ARG_EVENT_CB * pEventArg)
{
	MainWindow *pMain = (MainWindow *)proot;
	int nval=0;

	switch( pEventArg->type )
	{
		case TYPE_SINGLE_TRIG:
			nval = pMain->pvCLTU[pEventArg->system].POL[pEventArg->port].status_flag;
			break;
		case TYPE_SINGLE_T0:
			nval = pMain->pvCLTU[pEventArg->system].T0[pEventArg->port].status_flag;
			break;
		case TYPE_SINGLE_T1:
			nval = pMain->pvCLTU[pEventArg->system].T1[pEventArg->port].status_flag;
			break;
		case  TYPE_SINGLE_CLOCK:
			nval = pMain->pvCLTU[pEventArg->system].CLOCK[pEventArg->port].status_flag;
			break;
		case  TYPE_SINGLE_SETUP:
			nval = pMain->pvCLTU[pEventArg->system].SETUP_Pt[pEventArg->port].status_flag;
			break;
	
		case TYPE_MULTI_CLOCK:
			nval = pMain->pvCLTU[pEventArg->system].mCLOCK[pEventArg->port].status_flag;
			break;
		case TYPE_MULTI_TRIG:
			nval = pMain->pvCLTU[pEventArg->system].mTRIG.status_flag;
			break;
		case TYPE_MULTI_T0:
			nval = pMain->pvCLTU[pEventArg->system].mT0[pEventArg->port].status_flag;
			break;
		case TYPE_MULTI_T1:
			nval = pMain->pvCLTU[pEventArg->system].mT1[pEventArg->port].status_flag;
			break;
		case  TYPE_MULTI_SETUP:
			nval = pMain->pvCLTU[pEventArg->system].SETUP_Pt[4].status_flag;
			break;
/**************************************************************/
/*	 In case of New R2 */	
		case  TYPE_R2_ActiveLow:
			nval = pMain->pvCLTU[pEventArg->system].R2_ActiveLow[pEventArg->port].status_flag;
			break;
		case  TYPE_R2_Enable:
			nval = pMain->pvCLTU[pEventArg->system].R2_Enable[pEventArg->port].status_flag;
			break;
		case  TYPE_R2_Mode:
			nval = pMain->pvCLTU[pEventArg->system].R2_Mode[pEventArg->port].status_flag;
			break;
		case  TYPE_R2_CLOCK:
			nval = pMain->pvCLTU[pEventArg->system].R2_CLOCK[pEventArg->port][pEventArg->arg].status_flag;
			break;
		case  TYPE_R2_T0:
			nval = pMain->pvCLTU[pEventArg->system].R2_T0[pEventArg->port][pEventArg->arg].status_flag;
			break;
		case  TYPE_R2_T1:
			nval = pMain->pvCLTU[pEventArg->system].R2_T1[pEventArg->port][pEventArg->arg].status_flag;
			break;
		case  TYPE_R2_SETUP:
			nval = pMain->pvCLTU[pEventArg->system].SETUP_Pt[pEventArg->port].status_flag;
			break;
		case  TYPE_R2_GET_COMP_NS:
			nval = pMain->pvCLTU[pEventArg->system].GET_COMP_NS.status_flag;
			break;

		default:
			pMain->printReturn(LEVEL_ERROR, "no correlated system in get_pv_status_flag()");
			break;
	}
	
//	printf("nval = %d\n", nval);
	
	return nval;
}


