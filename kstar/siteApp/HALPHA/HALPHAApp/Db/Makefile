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
DB += HALPHA.db
DB += HALPHA_AO.db
DB += HALPHA_AO.template
DB += HALPHA_BO.db
DB += HALPHA_BO_CHON.db
DB += HALPHA_BO_CHON.template
DB += HALPHA_BO.template
DB += HALPHA_String.db
DB += HALPHA_String.template
DB += HALPHA_BO_ZCAL.db
DB += HALPHA_BO_ZCAL.template
DB += HALPHATimeStamp.db
DB += HALPHA_BI.db
#DB += DDS1_LTU.db
DB += SYS_CLTU.db
# Add to Sys Monitor by TG Lee  2008.04.05
DB += dbSysMon.db

# Add module If you use 2 module.
#DB += HALPHA_M2.db
#DB += HALPHA_AO_M2.template
#DB += HALPHA_BO_CHON_M2.template
#DB += HALPHA_String_M2.template
#DB += HALPHA_BO_ZCAL_M2.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

