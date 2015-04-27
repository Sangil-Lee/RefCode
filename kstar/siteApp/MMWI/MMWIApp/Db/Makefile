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
DB += dbExample1.db
DB += dbExample2.db
DB += dbSubExample.db
DB += user.substitutions
DB += userHost.substitutions
#DB += dbAdmin.db
DB += NI6123.db
DB += NI6123_DEV2.db
DB += NI6123_AOCH.db
DB += NI6123_AOCH_DEV2.db
DB += MMWI_READ_CH.db
DB += MMWI_READ_CH_DEV2.db
DB += MMWI_READ_CAL.db
DB += MMWI_READ_WAVE.db
DB += MMWI_READ_CH.template
DB += MMWI_READ_CH_DEV2.template
DB += MMWI_READ_CAL.template
DB += MMWI_READ_CAL_DEV2.template
DB += MMWI_READ_WAVE.template
DB += MMWI_READ_WAVE_DEV2.template
#DB += NI6259.db
#DB += IT6322.db
#DB += DDS1_cfg.db
DB += sysTimeStamp.db
#DB += dbSysMon.db

#DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db
DB += $(EPICS_SITE)/db/dbSFW.db

DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += MMWI_CLTU_CFG.template


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

