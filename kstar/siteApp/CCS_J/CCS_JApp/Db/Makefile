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

DB += ccs_PF1TestSoftPVs_BI.db
#DB += ccs_PF1TestSoftPVs_CALCOUT.db
DB += ccs_PF1TestSoftPVs_BI.template
DB += ccs_PF1TestSoftPVs_AI.db
DB += ccs_PF1TestSoftPVs_AI2.db
DB += ccs_PF1TestSoftPVs_STRING.db
DB += ccs_PF1TestSoftPVs_STRING.template
#DB += ccs_PF1TestSoftPVs_BO.db
#DB += ccs_PF1TestSoftPVs_BO.template
DB += ccs_PF1TestSoftPVs_LI.db
DB += ccs_PF1TestSoftPVs_LI.template
DB += dbSysMon.db

# 2010. 07. 08  move here from CCS_T
DB += ccs_pv_manipulate.db
# 2013. 6. 3  wall protection.
DB += ccs_Interlock_logic.db
DB += ccs_Interlock_logic.template
# 2013. 6. 4 this is from CCS_T temporary
DB += ccs_Interlock_T.db


DB += ccs_EtherIP_ao_deliver.db
DB += ccs_EIP_fast_interlock.template


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

