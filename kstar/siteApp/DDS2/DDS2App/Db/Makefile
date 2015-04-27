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
DB += dds2.db
DB += DDS2_AO.db
DB += DDS2_AO.template
DB += DDS2_BO.db
DB += DDS2_BO.template
DB += DDS2_String.db
DB += DDS2_String.template
DB += DDS2_BO_ZCAL.db
DB += DDS2_BO_ZCAL.template
#DB += dbExample2.db
#DB += dbSubExample.db
DB += dds2TimeStamp.db
# Delete DDS_SoftPV.vdb and DDS_SoftPV_INTN.vdb  . But Add DDS2_BI.db
#DB += DDS_SoftPV.vdb
#DB += DDS_SoftPV_INTN.vdb
DB += DDS2_BI.db
#DB += CCS_CTU.db
#DB += PCS_LTU.db
#DB += MPS1_LTU.db
#DB += MPS2_LTU.db
#DB += ECH_LTU.db
#DB += ICRH_LTU.db
#DB += DDS1_LTU.db
DB += SYS_CLTU.db
# Add to Sys Monitor by TG Lee  2008.04.05
DB += dbSysMon.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

