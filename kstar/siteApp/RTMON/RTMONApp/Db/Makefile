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

DB += dbRTMON.db
DB += dbRTCORE.db
DB += dbRfmOffsetScan.db
DB += dbManualOffset.db
DB += dbRfmOffsetScan.template
DB += dbManualOffset.template

DB += $(EPICS_SITE)/db/dbSFW.db

DB += dbRFM_TAG.db
DB += dbRFM_TAG.template
DB += dbKstarInfo.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

