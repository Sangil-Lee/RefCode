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

#Added by linupark[[
DB += FUEL_SMBI_MGI_PULSE.db
DB += FUEL_SMBI_MGI_AI.db
DB += FUEL_SMBI_MGI_AI.template
DB += FUEL_FILD_AI.db
DB += FUEL_FILD_AI.template
DB += FUEL_SMBI_MGI_AO.db
DB += FUEL_SMBI_MGI_AO.template
DB += FUEL_SMBI_MGI_BI.db
DB += FUEL_SMBI_MGI_BI.template
DB += FUEL_SMBI_MGI_BO.db
DB += FUEL_SMBI_MGI_BO.template
DB += FUEL_SMBI_MGI_BO_SW.db
DB += FUEL_SMBI_MGI_BO_SW.template

#Added by linupark, second[[
DB += FUEL_SMBI_MGI_HEART.db
DB += FUEL_SMBI_MGI_TIME.db
#]]

#]]


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

