#           ( DevType,  TaskName, DeviceName(createPhyChan),TriggerChannel, SigType, ClockChannel))
createDevice("NI6259", "Dev2", "Dev2", "/Dev2/PFI0", "ai", "/Dev2/PFI12")

#            ( TaskName, ChannelID, InputType, TagName, arg(MDS_Param or SCXI phyChannel))
createChannel("Dev2",     "0",      "FUEL",  "O_GFLOW_IN",	"Dev2/ai0")
createChannel("Dev2",     "1",      "FUEL",  "O_GFLOW_OUT", 	"Dev2/ai1")
createChannel("Dev2",     "2",      "FUEL",  "O_VALVE_DRI", 	"Dev2/ai2")
createChannel("Dev2",     "3",      "FUEL",  "D_GFLOW_IN", 	"Dev2/ai3")
createChannel("Dev2",     "4",      "FUEL",  "D_GFLOW_OUT", 	"Dev2/ai4")
createChannel("Dev2",     "5",      "FUEL",  "D_VALVE_DRI", 	"Dev2/ai5")
createChannel("Dev2",     "6",      "FUEL",  "K_VG4",    	"Dev2/ai6")
#createChannel("Dev2",     "7",      "FUEL",  "C_P0_TPGC2", 	"Dev2/ai7")
#createChannel("Dev2",     "8",      "FUEL",  "C_P0_TPGC3", 	"Dev2/ai16")
#createChannel("Dev2",     "9",      "FUEL",  "F_PKR251", 	"Dev2/ai17")
#createChannel("Dev2",     "10",     "FUEL",  "A_IMR265",	"Dev2/ai18")
#createChannel("Dev2",     "11",     "FUEL",  "N_PKR251A",	"Dev2/ai19")
#createChannel("Dev2",     "12",     "FUEL",  "N_PKR251B",	"Dev2/ai20")
#createChannel("Dev2",     "13",     "FUEL",  "PRESS_IN",	"Dev2/ai21")
#createChannel("Dev2",     "14",     "FUEL",  "V_P0_TPGC2",	"Dev2/ai22")
#createChannel("Dev2",     "15",     "FUEL",  "V_P0_TPGC3",	"Dev2/ai23")