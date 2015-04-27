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
#DB += user.substitutions
#DB += userHost.substitutions


DB += ACQ196_DAQ.db
DB += ACQ196_READ_CH_DAQ.db
DB += ACQ196_READ_CH_DAQ.template
DB += ACQ196_WRITE_CH_DAQ.db
DB += ACQ196_WRITE_CH_DAQ.template
#DB += ACQ196_cfg.db
#DB += ACQ196_String.db
#DB += ACQ196_String1.substitutions
#DB += ACQ196_String2.substitutions
#DB += ACQ196_String3.substitutions
DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/dbSFW.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

