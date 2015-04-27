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
DB += vftc.db
DB += ps_main.db
DB += ps_tok.db
DB += decoupler.db
DB += tune.db
DB += 2MW.db
DB += lowRF.db
DB += lowRF-new.db
DB += interlock.db
DB += daqRfd.db
DB += daq_mcontrol.db
DB += daq_central.db
DB += daq_m21.db
DB += daq_m22.db
DB += heartbeat.db
DB += intercomm.db
DB += mptest.db
DB += temper_levdet.db
DB += daq_m31.db
DB += daq_m32_temp.db
DB += daq_m32.db
DB += message.db
DB += dbSysMon.db
#DB += SYS_CLTU.db
#DB += ICRH_LTU.db
DB += ICRF_RF_Select.db

DB += $(EPICS_SITE)/db/SYS_CLTU.db
DB += $(EPICS_SITE)/db/SYS_CLTU_CFG.db
DB += ICRH_CLTU_CFG.template



#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

