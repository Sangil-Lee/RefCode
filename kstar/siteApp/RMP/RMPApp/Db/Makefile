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

DB += db16aiss8ao4.vdb
DB += dbRfm5565.vdb
DB += dbDio24.vdb

DB += db16AI_rt.db
DB += db16AI_rt.template

DB += dbGPIO_OUT.db
DB += dbGPIO_OUT.template 

DB += dbGPIO_IN.db
DB += dbGPIO_IN.template 

DB += dbWaveform_aio.db
DB += dbWaveform_aio.template

DB += dbPattern.db
DB += dbPattern.template


DB += $(EPICS_SITE)/db/dbSFW.db
DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += dbSysMon.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += RMP_CLTU_CFG.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

