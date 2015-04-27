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
DB += tmsInterlock.db
DB += tmsTimeStamp.db

DB += tmsStrain.db 
DB += tmsStrain_HighSample.db
#DB += tmsDisplacement.db
DB += tmsDisplacement_zero.db
DB += tmsDisplacement_TFS.db
DB += tmsStrain.template 
DB += tmsStrain_HighSample.template
#DB += tmsDisplacement.template
DB += tmsDisplacement_zero.template
DB += tmsDisplacement_TFS.template
DB += tmsInterlock_bo.db
DB += tmsInterlock_bo.template
DB += tmsInterlock_bi.db
DB += tmsInterlock_bi.template
DB += tmsInterlock_mbbi.db
DB += tmsInterlockMax.db
DB += tmsInterlockMax.template
DB += tms2MainValue.db
DB += tms2MainValue.template
DB += tmsTaskChange.db 
DB += tmsTaskChange.template
DB += tmsTaskChangeFanout.db 
#DB += dbPXISCXICernox.db
#DB += dbPXISCXICernox.template
#DB += dbFanout.db
#DB += cernox.db
#DB += tmsCSStruct.template
#DB += tmsPFBusLine.template
#DB += tmsPFCccs.template
#DB += tmsPFStruct.template
#DB += cernoxAll.db
#DB += tmsCernoxAll.template
#DB += tmsHall.db
#DB += tmsPT100.db
#DB += tmsMainValue.db
#DB += tmsHall.template
#DB += tmsPT100.template
#DB += stjSoft.template
DB += stjSoftRvt.db
DB += stjSoftRvt.template
DB += dbSysMon.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

