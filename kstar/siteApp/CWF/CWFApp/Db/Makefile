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
DB += CWF_AI_TEMP.db
DB += CWF_AI_TEMP.template
DB += CWF_DI.db
DB += CWF_DI.template
DB += PMS_AI.db
DB += PMS_AI.template
DB += PMS_BI.db
DB += PMS_BI.template
DB += PFC_AI_TEMP.db
DB += PFC_AI_TEMP.template
DB += dbPFCCalc.db
DB += dbSysMon.db
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

