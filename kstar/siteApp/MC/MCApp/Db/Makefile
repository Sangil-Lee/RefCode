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
#DB += dbExample1.db
#DB += dbExample2.db
#DB += dbSubExample.db
#DB += MC_DAQ_SOFTAO.db 
#DB += MC_DAQ_AO2.db

DB += MC_DAQ_AO.db
DB += MC_DAQ_AI.db
DB += MC_DAQ_BO.db
DB += MC_DAQ_MBBO.db
DB += MC_DAQ_MBBI.db
DB += MC_DAQ_MBBO2.db
#DB += MC_DAQ_SOFT_AO.db 
DB += MC_DAQ_SOFT_MASTER_BO.db
DB += MC_DAQ_SOFT_MASTER_AO.db 
DB += MC_DAQ_SOFT_MASTER_MBBO.db 
DB += MC_DAQ_MDSNODE_STRING.db
DB += MC_DAQ_SOFT_MASTER_STRING.db
#DB += MC_DAQ_SOFT_MASTER_8S.db 

#DB += MC_DAQ_AO.template
#DB += MC_DAQ_AI.template
#DB += MC_DAQ_BO.template
#DB += MC_DAQ_MBBO.template
#DB += MC_DAQ_MBBI.template
#DB += MC_DAQ_MBBO2.template
#DB += MC_DAQ_SOFT_AO.template 
#DB += MC_DAQ_SOFT_MASTER.template 
#DB += MC_DAQ_MDSNODE_STRING.template
#DB += MC_DAQ_AO_8S.template
#DB += MC_DAQ_AI_8S.template
#DB += MC_DAQ_BO_8S.template
#DB += MC_DAQ_MBBO_8S.template
#DB += MC_DAQ_MBBI_8S.template
#DB += MC_DAQ_MBBO2_8S.template

DB += MC_DAQ_AO_10S.template
DB += MC_DAQ_AI_10S.template
DB += MC_DAQ_BO_10S.template
DB += MC_DAQ_MBBO_10S.template
DB += MC_DAQ_MBBI_10S.template
DB += MC_DAQ_MBBO2_10S.template
#DB += MC_DAQ_SOFT_AO_8S.template
DB += MC_DAQ_SOFT_MASTER_BO.template 
DB += MC_DAQ_SOFT_MASTER_AO.template 
#DB += MC_DAQ_SOFT_MASTER_8S.template /* Remove TG - Mastar function edited */ 
#DB += MC_DAQ_MDSNODE_STRING_8S.template
DB += MC_DAQ_MDSNODE_STRING_10S.template

DB += MC_DAQ_BIO_CAL.db
DB += MC_DAQ_timeStamp.db
DB += dbSysMon.db
#DB += MC_DAQ_WAVE_A.db
#DB += MC_DAQ_WAVE_B.db
#DB += MC_DAQ_WAVE.db
#DB += MC_DAQ_WAVE_A.template
#DB += MC_DAQ_WAVE.template

# add by woong 2011. 5. 27
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += MC_CLTU_CFG.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

