TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# ECH DB for PLC Interface 
DB += ECH.db

# ECH CLTU DB Upload   20071114 TGLee

#DB += ECH_LTU.db
#DB += SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += $(EPICS_SITE)/db/dbSFW.db
DB += ECH_CLTU_CFG.template

# MDSplus and EPICS Interface DB & PCI6220 Upload 20090723 TGLee
#DB += NI6123.db
#DB += NI6123_AOCH.db
#DB += NI6123_AOCH.template
#DB += ECH_READ_CH_DAQ.db
#DB += ECH_READ_CH_DAQ.template
#DB += ECH_MDS_PARAM.db

# MDSplus and EPICS Interface DB & PCIe 6259 Upload 20130417 TGLee
DB += NIDAQmx.db
DB += NIDAQmx_AI.db
DB += NIDAQmx_AI.template
DB += NIDAQmx_BO.db
DB += NIDAQmx_BO_SOFT.db
DB += NIDAQmx_BO_SOFT.template
DB += NIDAQmx_AO.db
DB += NIDAQmx_AO_CH0_PWRtoVAL.db
DB += NIDAQmx_AO_CH0_Pow.template

DB += NIDAQmx_AO_CH1_FWD_VAL.db
DB += NIDAQmx_AO_CH1_FWD.template

DB += NIDAQmx_AO_CH2_FWD_VAL.db
DB += NIDAQmx_AO_CH2_FWD.template

DB += NIDAQmx_AO_CH0_Val.template
DB += NIDAQmx_AO_CH1_Val.template
DB += NIDAQmx_AO_CH2_Val.template
DB += NIDAQmx_AO_CH0_Time.template

DB += NIDAQmx_AO_CH1_FWD_TIME.db
DB += NIDAQmx_AO_CH1_Time.template
DB += NIDAQmx_AO_CH2_Time.template

DB += NIDAQmx_AO_CH_PATTERNtoPWR.db
DB += NIDAQmx_AO_CH_PATTERNtoPWR.template

DB += NIDAQmx_AO_RTIME.db
DB += NIDAQmx_AO_RTIME.template
DB += NIDAQmx_AO_RTIME_CAL.db

# for ECH param gain, offset, abc param
#DB += ECH_DAQ_AOCH.db
#DB += ECH_DAQ_AOCH.template

DB += ECH_timeStamp_DAQ.db 

# Include HMT... DB    ADD  TG.Lee
DB += ECH_AI.db
DB += ECH_AI_CALC.db
DB += ECH_AO.db
DB += ECH_AO_INP.db
DB += ECH_BI.db
DB += ECH_BO.db
DB += ECH_LO_INP.db
DB += ECH_AI.template
DB += ECH_AI_CALC.template
DB += ECH_AO.template
DB += ECH_AO_INP.template
DB += ECH_BI.template
DB += ECH_BO.template
DB += ECH_LO_INP.template

# move db & temp file from ECHApp/db to ECHApp/Db folder. compared with st.cmd file. Add TG.Lee 20110506
DB += ECH_INFO.db
DB += ECH_FAULT_LIST.db
DB += ECH_AI_ALARM.db
DB += ECH_AI_ALARM.template
DB += ECH_AI_ARM.db
DB += ECH_AI_ARM.template

DB += dbSysMon.db
# TPG256, DG535,ESP300,VACION Interface DB Upload 20081107 mkkim
DB += TPG256.db
DB += TPG256.template
DB += DG535_Read.db
DB += DG535_Write.db
DB += VACION_AI.db
DB += ESP300_AI.db
DB += ESP300_AI_TB.db
DB += ESP300_AO.db
DB += ESP300_BI.db
DB += ESP300_BO.db
DB += ESP300_SI.db
DB += ESP300_AI.template
DB += ESP300_AO.template
DB += ESP300_BI.template
DB += ESP300_BO.template
DB += ESP300_SI.template
DB += ECH_PermReady.db
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

