#           ( Type,   TaskName, DeviceName(createPhyChan),TriggerChannel, SigType, ClockChannel)
createDevice("NI6259", "Dev1_AI", "Dev1", "/Dev1/PFI0", "ai",  "/Dev1/PFI12")

#            ( TaskName, ChannelID, InputType, TagName, arg(MDS_Param or SCXI phyChannel))
createChannel("Dev1_AI",    "0",     "AI",  "TEMP1","")
#createChannel("Dev1_AI",    "1",     "AI",  "TEMP2","")
