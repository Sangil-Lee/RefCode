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
#DB += dbExample1.db
#DB += dbExample2.db
#DB += dbSubExample.db
#DB += user.substitutions
#DB += userHost.substitutions

#DB += $(EPICS_SITE)/db/SYS_CLTU.db
#DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
#DB += ECH_CLTU_CFG.template

DB += $(EPICS_SITE)/db/dbSFW.db
DB += $(EPICS_SITE)/db/dbSysMon.db

# MDSplus and EPICS Interface DB & PCI6220 Upload 20090723 TGLee
#DB += NI6123.db
#DB += NI6123_AOCH.db
#DB += NI6123_AOCH.template
DB += NIDAQ.db
DB += NIDAQ_READ_CH_DAQ.db
DB += NIDAQ_READ_CH_DAQ.template
DB += NIDAQ_WRITE_CH_DAQ.db 
DB += NIDAQ_WRITE_CH_DAQ.template
DB += NIDAQ_READ_CAL_PRESSURE.db

DB += ECH_DAQ_AOCH.db 
DB += ECH_DAQ_AOCH.template

#DB += ECH_READ_CH_DAQ.db
#DB += ECH_READ_CH_DAQ.template

DB += ECH_MDS_PARAM.db


DB += ECH_timeStamp_DAQ.db


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

