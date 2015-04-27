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
DB += dbPXISCXICernox.db
DB += dbPXISCXICernox.template
DB += dbFanout.db
#DB += cernox.db
#DB += tmsCSStruct.template
#DB += tmsPFBusLine.template
#DB += tmsPFCccs.template
#DB += tmsPFStruct.template
DB += tmsInterlock.db
DB += tmsTimeStamp.db
DB += tmsMainCDCTLevel.db
DB += tmsMainCDCTLevel.template
DB += cernoxAll.db
DB += cernoxAllRM.db
DB += cernoxAllRaw.db
DB += tmsCernoxAll.template
DB += tmsCernoxAllRM.template
DB += tmsCernoxAllRaw.template
DB += tmsHall.db
#DB += tmsVHall.db
#DB += tmsVVHall.db
DB += tmsPT100.db
DB += tmsPT100_V2.db
DB += tmsMainValue.db
DB += tmsMaxValue.db
DB += tmsMaxValue.template
DB += tmsMainMaxValue.template
DB += tmsHall.template
#DB += tmsVHall.template
#DB += tmsVVHall.template
DB += tmsPT100.template
DB += tmsPT100_V2.template
DB += tmsRelay.db
DB += tmsHallRelay.db
DB += tmsRelay.template
DB += tmsHallRelay.template
DB += dbSysMon.db

# for Cernox sensor select from primary and secondary sensor
DB += tmsPFBLineMasterSelect.db
DB += tmsPFBLineMasterSelect.template
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

