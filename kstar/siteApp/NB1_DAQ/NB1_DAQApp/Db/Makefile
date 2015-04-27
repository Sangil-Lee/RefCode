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

DB += dbccsPT100.db
DB += dbccsPT100.template
DB += devAcq196.db
DB += devMdsGainA.db
DB += devMdsGainA.template
DB += devMdsGainB.db
DB += devMdsGainB.template
DB += NB1_DTACQ_HeartBeat.db
DB += NB1_SHOT.db

DB += devAcq196_1.db
DB += devAcq196_2.db
DB += devFilaGainAB.db
DB += devFilaGainAB.template
DB += devMdsEtcGain.db
DB += devTCdatasave.db

DB += $(EPICS_SITE)/db/sysTimeStamp.db
DB += $(EPICS_SITE)/db/dbSysMon.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

