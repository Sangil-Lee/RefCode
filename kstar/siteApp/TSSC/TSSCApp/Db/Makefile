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
DB += dbTSSC.db
DB += $(EPICS_SITE)/db/dbSFW.db
DB += dbGPIO_in.db
DB += dbGPIO_out.db
DB += dbGPIO_dir.db
DB += dbGPIO_in.template
DB += dbGPIO_out.template
DB += dbGPIO_dir.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

