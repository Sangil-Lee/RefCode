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

DB += ts_value_B0.substitutions
DB += ts_value_B1.substitutions
DB += ts_value_B2.substitutions
DB += ts_value_B3.substitutions
DB += ts_tag_B0.substitutions
DB += ts_tag_B1.substitutions
DB += ts_tag_B2.substitutions
DB += ts_tag_B3.substitutions
DB += ts_result.substitutions

DB += TS_CH_tag.db
DB += TS_CH_value.db
DB += TS_RESULT.db
DB += TS_cfg.db
DB += TS_get_TS1.db
DB += TS_shot.db

DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/dbSFW.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

