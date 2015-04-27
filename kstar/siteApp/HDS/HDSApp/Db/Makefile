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

#############
#HDS VPS DB
#############

DB += HDS_VPS_AI.db
DB += HDS_VPS_AI_GAUGE.db
DB += HDS_VPS_AI_GAUGE.template
DB += HDS_VPS_AI.template
DB += HDS_VPS_AO.db
DB += HDS_VPS_AO.template
DB += HDS_VPS_BI.db
DB += HDS_VPS_BI.template
DB += HDS_VPS_BO_SW.db
DB += HDS_VPS_BO.template
DB += HDS_VPS_GAUGE_AL.db
DB += HDS_VPS_GAUGE_AL.template
DB += HDS_VPS_HEART.db
DB += HDS_VPS_MBBI_TMP.db
DB += HDS_VPS_MBBI_TMP.template
DB += HDS_VPS_MBBI_TPG256.db
DB += HDS_VPS_MBBI_TPG256.template
DB += HDS_VPS_REMOTE.db

DB += HDS_HCS_PLC_STAT.db

DB += HDS_VIB_AI_1.template
DB += HDS_VIB_AI_2.template
DB += HDS_VIB_AI_1.db
DB += HDS_VIB_AI_2.db

DB += dbSysMon.db


#############
#HDS HCS DB
#############

DB += HDS_HCS_AI_ALARM_1.db
DB += HDS_HCS_AI_ALARM_1.template
DB += HDS_HCS_AI_ALARM.db
DB += HDS_HCS_AI_ALARM.template
DB += HDS_HCS_AI.db
DB += HDS_HCS_AI_SEL.db
DB += HDS_HCS_AI_SEL.template
DB += HDS_HCS_AI_T.db
DB += HDS_HCS_AI.template
DB += HDS_HCS_AI_T.template
DB += HDS_HCS_ALARM.db
DB += HDS_HCS_ALARM_TEMP.db
DB += HDS_HCS_AO.db
DB += HDS_HCS_AO_FLOW.db
DB += HDS_HCS_AO_FLOW.template
DB += HDS_HCS_AO_HRS2SHDS.template
DB += HDS_HCS_AO_IN.db
DB += HDS_HCS_AO_SEL.db
DB += HDS_HCS_AO_SEL.template
DB += HDS_HCS_AO.template
DB += HDS_HCS_AO_TMS2SHDS.template
DB += HDS_HCS_BI.db
DB += HDS_HCS_BI.template
DB += HDS_HCS_BO.db
DB += HDS_HCS_BO_HRS2SHDS.template
DB += HDS_HCS_BO_IN.db
DB += HDS_HCS_BO.template
DB += HDS_HCS_HEART.db
DB += HDS_HCS_RESET.db
DB += HDS_HCS_Soft_PV.db

#############
#HDS RGA DB
#############

DB += HDS_VPS_INIT.db
DB += HDS_VPS_FL.db
DB += HDS_VPS_READ.db
DB += HDS_VPS_READ.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

