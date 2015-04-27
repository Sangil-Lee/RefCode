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

DB += AUTO_LTU.db
DB += dg645.db
DB += labview.db
DB += ps_in.db
DB += ps_out.db
DB += tl_in.db
DB += tl_out.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += EC1A_CLTU_CFG.template
DB += EC1_MAIN.db
DB += EC1_PermReady.db


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

