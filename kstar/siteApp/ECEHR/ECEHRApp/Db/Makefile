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
DB += ECEHR.db
DB += ECEHR_AO.db
DB += ECEHR_AO.template
DB += ECEHR_AO_M2.template
DB += ECEHR_BO.db
DB += ECEHR_BO_CHON.db
DB += ECEHR_BO_CHON.template
DB += ECEHR_BO_CHON_M2.template
DB += ECEHR_BO.template
DB += ECEHR_String.db
DB += ECEHR_String.template
DB += ECEHR_String_M2.template
DB += ECEHR_BO_ZCAL.db
DB += ECEHR_BO_ZCAL.template
DB += ECEHR_BO_ZCAL_M2.template
DB += ECEHRTimeStamp.db
DB += ECEHR_BI.db
#DB += DDS1_LTU.db
#DB += SYS_CLTU.db
# Add to Sys Monitor by TG Lee  2008.04.05
DB += dbSysMon.db

DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += ECEHR_CLTU_CFG.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

