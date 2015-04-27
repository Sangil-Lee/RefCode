TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

DB += devBendingAI.db
DB += devBendingAI.template
DB += devBendingAO.db
DB += devBendingAO.template
DB += devBendingBI.template
DB += devBendingBO.db
DB += devBendingBO.template
DB += devBendingCalc.db
DB += devBendingMBBI.db
DB += devBendingMBBI.template
DB += devEtosAI.db
DB += devEtosAI.template
DB += devEtosAO.db
DB += devEtosAO.template
DB += devMdsplus.db
DB += devSoftTrig.db
DB += devTCAi.db
DB += devTCAi_Kelvin.db
DB += devTCAi_Kelvin.template
DB += devTCAi.template
DB += devTcMinMax.db
DB += NB1_BL_AI.template
DB += NB1_BL_AO.template
DB += NB1_BL_BG.db
DB += NB1_BL_BI.template
DB += NB1_BL_BO.template
DB += NB1_BL_CRP.db
DB += NB1_BL_CRP.template
DB += NB1_BL_DELTA.db
DB += NB1_BL_DELTA.template
DB += NB1_BL_CALC.db
DB += NB1_BM_AI.db
DB += NB1_EPICS_to_PLC.db
DB += NB1_EPICS_to_PLC.template
DB += NB1_HeartBeat.db
DB += NB1_HeartBeat.template
DB += NB1_INT_BO.db
DB += NB1_PLC_AI.db
DB += NB1_PLC_AO.db
DB += NB1_PLC_BI.db
DB += NB1_PLC_BO.db
DB += NB1_PLC_WATER_AI.db
DB += NB1_TC_INTC.db
DB += NB1_TC_to_PLC.db
DB += NB1_TC_to_PLC.template
DB += NB1_TEMP_INT.db
DB += NB1_TIME_STAMP.db
DB += NB1_WATER_AI.template
DB += NB1_WATER_AO.template
DB += NB1_WATER_BI.template
DB += NB1_WATER_BO.template
DB += NBI1B_CLTU_CFG.template

DB += NB1_DUMMY.db
DB += NB1_TC_INTC_AO.db
DB += NB1_TC_INTC_AO.template
DB += NB1_TC_INTC_CALC.db
DB += NB1_WATER_CALC.db
DB += NB1_WATER_TEMP.db
DB += devEtosCALC.db

# RGA
DB += NB1_LOCAL_RGA_FL.db
DB += NB1_LOCAL_RGA_INIT.db
DB += NB1_LOCAL_RGA_READ.db
DB += NB1_LOCAL_RGA_READ.template

DB += devICP.db
DB += devICP.template
DB += ICP_HePAC.db

DB += TestBo.db

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this

DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += NBI1B_CLTU_CFG.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

