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
DB += EC1_D1_DAQ.db
DB += EC1_D1_DAQ_READ_CH.db
DB += EC1_D1_DAQ_READ_CH.template
DB += EC1_D1_DAQ_POL_CH.db
DB += EC1_D1_DAQ_POL_CH.template
DB += EC1_D1_DAQ_TC_CH.db
DB += EC1_D1_DAQ_TC_CH.template
DB += EC1_D1_DAQ_STATE_Cfg.db

DB += $(EPICS_SITE)/db/dbSFW.db
DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += EC1B_CLTU_CFG.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

