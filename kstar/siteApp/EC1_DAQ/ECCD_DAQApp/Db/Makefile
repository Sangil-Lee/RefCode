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
DB += devGain.db
DB += devGain.template
DB += devIntegral.db
DB += devIntegral.template
DB += devMdsplus.db
DB += devNiPci6220.db
DB += devNiPci6254.db
DB += devNiPci.db
DB += devOffset.db
DB += devOffset.template
DB += devPoloScan.db
DB += devPower.db
DB += devPower.template
DB += devLabviewTC.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/sysTimeStamp.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

