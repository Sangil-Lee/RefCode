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
DB += ACQ196_READ_WAVE.db
DB += ECEHRCal.db


DB += /usr/local/epics/siteLib/db/SYS_CLTU.db
DB += /usr/local/epics/siteLib/db/SYS_CLTU_CFG.db
DB += ECEHR_CLTU_CFG.template

DB += /usr/local/epics/siteLib/db/sysTimeStamp.db
DB += /usr/local/epics/siteLib/db/dbSysMon.db
DB += /usr/local/epics/siteLib/db/dbSFW.db


#DB += ECEHRCal.db
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

