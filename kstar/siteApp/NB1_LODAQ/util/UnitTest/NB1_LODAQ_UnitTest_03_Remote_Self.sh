# ------------------------------------------
# Unit Test for Remote operation
# ------------------------------------------

caput NB1_LODAQ_OP_MODE 1

sleep 5

while true
do
	echo ""
	echo "--------------------------------------"
	echo "Enter for NB1_LODAQ_SYS_ARMING"

	read val
	caput NB1_LODAQ_SYS_ARMING 1

	echo ""
	echo "Enter for NB1_LODAQ_SYS_RUN"

	read val
	caput NB1_LODAQ_SYS_RUN 1
done

# ------------------------------------------
# End of file
# ------------------------------------------
