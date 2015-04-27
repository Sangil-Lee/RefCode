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

DB += dbSysMon.db
DB += PMS_HEART.db
DB += PMS_TIME.db
DB += PMS_TEMP.db
DB += PMS_TEMP.template
DB += PMS_TEMP_UTIL.db
DB += PMS_MAX.db
DB += PMS_MAX.template
DB += PMS_TEMP_NBI.db
DB += PMS_TEMP_NBI.template


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

