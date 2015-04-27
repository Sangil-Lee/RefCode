#           ( Type,   TaskName, DeviceName,TriggerChannel, SigType))
createDevice("NI6133", "Dev1", "Dev1", "/Dev1/PFI0", "ai")

#            ( TaskName, ChannelID, InputType, TagName, arg
createChannel("Dev1",     "0",      "GCDS",  "GND_CUR01")
createChannel("Dev1",     "1",      "GCDS",  "GND_VMS")
createChannel("Dev1",     "2",      "GCDS",  "GND_MPS")
#createChannel("Dev1",     "3",      "GCDS",  "GND_DIAG") - 2012 Not acquire data
#createChannel("Dev1",     "4",      "GCDS",  "","MDS_Param")
#createChannel("Dev1",     "5",      "GCDS",  "","MDS_Param")
#createChannel("Dev1",     "6",      "GCDS",  "","MDS_Param")
#createChannel("Dev1",     "7",      "GCDS",  "","MDS_Param")
