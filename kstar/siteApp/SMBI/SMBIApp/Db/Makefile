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
DB += SMBI_MGI_PULSE.db
DB += SMBI_MGI_AI.db
DB += SMBI_MGI_AI.template
DB += SMBI_FILD_AI.db
DB += SMBI_FILD_AI.template
DB += SMBI_MGI_AO.db
DB += SMBI_MGI_AO.template
DB += SMBI_MGI_BI.db
DB += SMBI_MGI_BI.template
DB += SMBI_MGI_BO.db
DB += SMBI_MGI_BO.template
DB += SMBI_MGI_BO_SW.db
DB += SMBI_MGI_BO_SW.template

#Added by linupark, second[[
DB += SMBI_MGI_HEART.db
DB += SMBI_MGI_TIME.db
#]]

#]]


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

