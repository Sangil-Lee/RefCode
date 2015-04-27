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
DB += ACQ196_cfg.db

DB += dbAcq196_ch.db
DB += dbB5_ch.template
DB += dbB6_ch.template


DB += $(EPICS_SITE)/db/sysTimeStamp.db
#DB += $(EPICS_SITE)/db/dbSysMon.db
DB += dbSysMon.db
DB += $(EPICS_SITE)/db/dbSFW.db

DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += DDS1B_CLTU_CFG.template


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

