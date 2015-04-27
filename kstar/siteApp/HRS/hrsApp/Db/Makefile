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
DB += example.db
DB += status.db
DB += s7plc_bi.db
DB += s7plc_bo.db
DB += s7plc_ai.db
DB += s7plc_ao.db
DB += hrs_io.template
DB += save_restoreStatus.db
DB += hrs_mbbi.db
DB += hrs_mbbo.db
DB += dbSysMon.db
DB += hrsTimeStamp.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

