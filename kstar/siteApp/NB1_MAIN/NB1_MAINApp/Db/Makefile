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
DB += NB1_BM_FUNC.db
DB += NB1_ETOS_AI.db
DB += NB1_ETOS_AI.template
DB += NB1_KST_INFO.db
DB += NB1_LTU_ALL_SHOT.db
DB += NB1_MAIN_HeartBeat.db
DB += NB1_OPR_MODE_BO.db
DB += NB1_OPR_MODE_BO.template
DB += NB1_PARAM_BO.db
DB += NB1_PS_AI_LN2.db
DB += NB1_PS_AI_LN2.template
DB += NB1_PS_AI_ARC.template
DB += NB1_PS_AI_BIAS.template
DB += NB1_PS_AI_CALC.db
DB += NB1_PS_AI.db
DB += NB1_PS_AI_FILAM.template
DB += NB1_PS_AI_G1.template
DB += NB1_PS_AI_G3.template
DB += NB1_PS_AI_MOD.template
DB += NB1_PS_AI_VME.db
DB += NB1_PS_AI_VME.template
DB += NB1_PS_AO_ARC.template
DB += NB1_PS_AO_BIAS.template
DB += NB1_PS_AO_CALC.db
DB += NB1_PS_AO_CALC.template
DB += NB1_PS_AO.db
DB += NB1_PS_AO_FILAM.template
DB += NB1_PS_AO_FILA_TF.db
DB += NB1_PS_AO_FILA_VCMD.db
DB += NB1_PS_AO_G1.template
DB += NB1_PS_AO_G3.template
DB += NB1_PS_AO_MOD.template
DB += NB1_PS_AO_VME.template
DB += NB1_PS_BI_ARC.template
DB += NB1_PS_BI_BIAS.template
DB += NB1_PS_BI_CALC.db
DB += NB1_PS_BI.db
DB += NB1_PS_BI_ETC.template
DB += NB1_PS_BI_FILAM.template
DB += NB1_PS_BI_G1.template
DB += NB1_PS_BI_G3.template
DB += NB1_PS_BI_VME.template
DB += NB1_PS_BO_ARC.template
DB += NB1_PS_BO_BIAS.template
DB += NB1_PS_BO.db
DB += NB1_PS_BO_FILAM.template
DB += NB1_PS_BO_G1.template
DB += NB1_PS_BO_G3.template
DB += NB1_PS_BO_VME.template
# woong add 2013. 6. 28
DB += NB1_PS_BO_ETC.template
DB += NB1_PS_RGA.db

DB += NB1_PS_CALC_ETC.db
DB += NB1_BL_PS_INT.db
DB += NB1_BM_FUNC2.db
DB += NB1_KSTAR_DUMMY.db
DB += NB1_PS_OPER_MODE.db
DB += NB1_PS_P2FILA_T.db
DB += NB1_UTIL.db

DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += NBI1A_CLTU_CFG.template
DB += NBI1D_CLTU_CFG.template

# YSW added 2013.07.19 for permission and ready
DB += NB1_PermReady.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

