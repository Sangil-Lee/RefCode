TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this

DB += ccs_vxStats.vdb
DB += ccs_vxStats.template

DB += ccs_ClusterLine.db
DB += ccs_ClusterLine.template

#DB += vmic2534_bi.db
#DB += vmic2534_bo.db
#DB += vmic2534.template

#DB += ccs_heartbeatCheck.db
DB += ccs_mainScreenSoftPVs_BI.db
DB += ccs_mainScreenSoftPVs_MBBI.db
DB += ccs_mainScreenSoftPVs.template

DB += ccs_watchdogSoftPVs_BI.db
DB += ccs_watchdogSoftPVs.template

DB += ccs_MPSMeasured.db
DB += ccs_MPSMeasured.template

DB += ccs_interlockOutputModule.vdb
DB += ccs_interlockInputModule.vdb
DB += ccs_interlockOutputModule_heating.db
DB += ccs_pcsControlStatus.vdb
DB += ccs_ShotNumber.db
DB += ccs_pcsRTMode.db
DB += sysTimeStamp.db
#DB += ccs_pv_relay.db
#DB += ccs_pv_relay.template
DB += ccs_Interlock_T.db


#
#   Test DB, It will be removed after finishing test
#
#DB += ccs_EtherIP_longin.db
DB += ccs_EtherIP_ai.db
DB += ccs_EtherIP_ao.db
DB += ccs_EtherIP_ao_deliver.db
DB += ccs_EIPtest.template
DB += ccs_EIP_fast_interlock.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

