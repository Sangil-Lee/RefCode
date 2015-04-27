#file db/dlsIntLockInterFace.db {
#            pattern {REC,                      SHUTTER,             INTERLOCK}
#                    {"DLS_ILLUM01_A_SHUT_CON", "DLS_ILLUM01_A_SHUT", "DLS_ILLUM01_A_SHUT_INTL"},
#                    {"DLS_ILLUM02_E_SHUT_CON", "DLS_ILLUM02_E_SHUT", "DLS_ILLUM02_E_SHUT_INTL"},
#                    {"DLS_ILLUM03_I_SHUT_CON", "DLS_ILLUM03_I_SHUT", "DLS_ILLUM03_I_SHUT_INTL"},
#                    {"DLS_ILLUM04_M_SHUT_CON", "DLS_ILLUM04_M_SHUT", "DLS_ILLUM04_M_SHUT_INTL"},
#                    {"DLS_VSPEC01_J_SHUT_CON", "DLS_VSPEC01_J_SHUT", "DLS_VSPEC01_J_SHUT_INTL"},
#                    {"DLS_VSPEC02_J_SHUT_CON", "DLS_VSPEC02_J_SHUT", "DLS_VSPEC02_J_SHUT_INTL"},
#                    {"DLS_THAMON01_J_SHUT_CON","DLS_THAMON01_J_SHUT", "DLS_THAMON01_J_SHUT_INTL"},
#                    {"DLS_PHAMON01_J_SHUT_CON", "DLS_PHAMON01_J_SHUT", "DLS_PHAMON01_J_SHUT_INTL"},
#                    {"DLS_PHAMON02_J_SHUT_CON",	"DLS_PHAMON02_J_SHUT", "DLS_PHAMON02_J_SHUT_INTL"},
#                    {"DLS_MMW01_G_SHUT_CON", "DLS_MMW01_G_SHUT", "DLS_MMW01_G_SHUT_INTL"},
#                    {"DLS_TV01_A_SHUT_CON", "DLS_TV01_A_SHUT", "DLS_TV01_M_SHUT_INTL"},
#                    {"DLS_TV02_I_SHUT_CON", "DLS_TV02_I_SHUT", "DLS_TV02_M_SHUT_INTL"},
#}
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_ILLUM01_A_SHUT_CON, SHUTTER=DLS_ILLUM01_A_SHUT, INTERLOCK=DLS_ILLUM01_A_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_ILLUM02_E_SHUT_CON, SHUTTER=DLS_ILLUM02_E_SHUT, INTERLOCK=DLS_ILLUM02_E_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_ILLUM03_I_SHUT_CON, SHUTTER=DLS_ILLUM03_I_SHUT, INTERLOCK=DLS_ILLUM03_I_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_ILLUM04_M_SHUT_CON, SHUTTER=DLS_ILLUM04_M_SHUT, INTERLOCK=DLS_ILLUM04_M_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_VSPEC01_J_SHUT_CON, SHUTTER=DLS_VSPEC01_J_SHUT, INTERLOCK=DLS_VSPEC01_J_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_VSPEC02_J_SHUT_CON, SHUTTER=DLS_VSPEC02_J_SHUT, INTERLOCK=DLS_VSPEC02_J_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_THAMON01_J_SHUT_CON, SHUTTER=DLS_THAMON01_J_SHUT, INTERLOCK=DLS_THAMON01_J_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_PHAMON01_J_SHUT_CON, SHUTTER=DLS_PHAMON01_J_SHUT, INTERLOCK=DLS_PHAMON01_J_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_PHAMON02_J_SHUT_CON, SHUTTER=DLS_PHAMON02_J_SHUT, INTERLOCK=DLS_PHAMON02_J_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_MMW01_G_SHUT_CON, SHUTTER=DLS_MMW01_G_SHUT, INTERLOCK=DLS_MMW01_G_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_TV01_A_SHUT_CON, SHUTTER=DLS_TV01_A_SHUT, INTERLOCK=DLS_TV01_M_SHUT_INTL")
epicsThreadSleep(1.)
dbLoadRecords("db/dlsIntLockInterFace.db", "REC=DLS_TV02_I_SHUT_CON, SHUTTER=DLS_TV02_I_SHUT, INTERLOCK=DLS_TV02_M_SHUT_INTL")

