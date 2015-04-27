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

DB += dbCRP.vdb
DB += dbLoadCRP.template

DB += dbGP307.vdb
DB += dbLoadGP307.template

DB += dbMBP.vdb
DB += dbLoadMBP.template

DB += dbTMP.vdb
DB += dbLoadTMP.template

DB += dbTPG262.vdb
DB += dbLoadTPG262.template

DB += VMS_CR_BI_CONN.db
DB += VMS_CR_BI_READ_CONN.template

DB += VMS_CR_BI.db
DB += VMS_CR_BI_READ.template
DB += VMS_CR_BO_SW.db
DB += VMS_CR_BO_SW_WRITE.template
DB += CRP_Rxx.db
DB += VMS_CR_CRP_Rxx.template
DB += VMS_CR_GAUGE_OUT.db
DB += VMS_CR_GAUGE_OUT.template
DB += VMS_CR_HEART.db
DB += VMS_CR_FL.db
DB += VMS_CR_INIT.db
DB += VMS_CR_READ.db
DB += VMS_CR_READ.template
DB += TMP_RSS.db
DB += VMS_CR_TMP_RSS.template

DB += dbSysMon.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

