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
DB += dbExample1.db
DB += dbExample2.db
DB += dbSubExample.db
DB += user.substitutions
DB += userHost.substitutions

DB += FUEL_DAQ_timeStamp.db
DB += NIDAQ.db
DB += NIDAQ_READ_CH_DAQ.db
DB += NIDAQ_WRITE_CH_DAQ.db
DB += NIDAQ_READ_CH_DAQ.template
DB += NIDAQ_READ_CH_DAQ2.template
DB += NIDAQ_WRITE_CH_DAQ.template
DB += NIDAQ_WRITE_CH_DAQ2.template
DB += NIDAQ_READ_CAL_PRESSURE.db
DB += $(EPICS_SITE)/db/dbSFW.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += FUEL_CLTU_CFG.template
DB += FUEL2_CLTU_CFG.template


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

