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
DB += devLHCD_AI.db
DB += devLHCD_AI.template
DB += devLHCD_AO.db
DB += devLHCD_AO.template
DB += devLHCD_BI.db
DB += devLHCD_BI.template
DB += devLHCD_BO.db
DB += devLHCD_BO.template
DB += devLHCD_CALC.db
DB += devLHCD_Heart.db
DB += devLHCD_MBBI.db
DB += devLHCD_MBBI_INP.db
DB += devLHCD_MBBI_INP.template
DB += devLHCD_MBBI.template
DB += devLHCD_UTIL.db

# Common
DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db

# Add by woong. for LTU
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += LHCD_CLTU_CFG.template
###################### woong

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

