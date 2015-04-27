# ------------------------------------------
# Unit Test for Remote operation
# ------------------------------------------

#caput NB1_LODAQ_OP_MODE 1

sleep 5

caget YUNSW_SHOTSEQ_START YUNSW_CTU_shotStart

while true
do
	echo ""
	echo "--------------------------------------"
	echo "Enter for Shot Sequence Start"
	read val
	caput YUNSW_SHOTSEQ_START 1

	echo ""
	echo "Enter for Shot Start"
	read val
	caput YUNSW_CTU_shotStart 1

	#echo ""
	#echo "Enter for Shot Stop"
	#read val
	#caput YUNSW_CTU_shotStart 0

	echo ""
	echo "Enter for Shot Sequence Stop"
	read val
	caput YUNSW_SHOTSEQ_START 0
done

# ------------------------------------------
# End of file
# ------------------------------------------
