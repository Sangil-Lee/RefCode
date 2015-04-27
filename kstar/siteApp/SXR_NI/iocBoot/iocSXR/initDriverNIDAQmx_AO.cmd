#           ( Type,   TaskName, DeviceName(createPhyChan),TriggerChannel, SigType, ClockChannel)
createDevice("NI6259", "Dev1_AO", "Dev1", "/Dev1/PFI0", "ao",  "/Dev1/PFI12")

#            ( TaskName, ChannelID, InputType(pSTDCh->strArg0), TagName, arg(MDS_Param or SCXI phyChannel))
createChannel("Dev1_AO",     "0",      "AO",                    "ECH_VC","")
createChannel("Dev1_AO",     "1",      "AO",                    "ECH_VB","")
createChannel("Dev1_AO",     "2",      "AO",                    "ECH_IB","")
#createChannel("Dev1_AO",     "3",      "AO",                    "ECH_IA","")
