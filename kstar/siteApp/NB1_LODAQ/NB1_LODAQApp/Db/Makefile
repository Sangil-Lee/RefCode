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
DB += NB1_LODAQ.db
DB += NB1_LODAQ_Cfg.db
DB += NB1_LODAQ_DeltaT.substitutions
DB += NB1_LODAQ_DeltaT.db
DB += NB1_LODAQ_TC.substitutions
DB += NB1_LODAQ_TC.db
# for testing with self NB1_DTACQ_LocalShot / NB1_BEAM_INJECT_KSTAR / NB1_BEAM_INJECT_DUMMY
#DB += NB1_LODAQ_Test.db

DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSFW.db

# 2012. 5. 23 woong 
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += NBI1C_CLTU_CFG.template


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

