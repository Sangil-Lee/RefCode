#!/bin/bash 

#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/branches/codac-core-4.1/src/main/scripts/sdn-create-topic.sh $
# $Id: sdn-create-topic.sh 36365 2013-04-30 13:18:59Z kimh12 $
#
# Project       : CODAC Core System
#
# Description   : SDN Software - Communication API Makefile.
#
# Author        : Eui Jae LEE, Hyung Gi KIM (Mobiis Co., Ltd)
#
# Co-Author     : Mahajan Kirti
#
# Copyright (c) : 2010-2013 ITER Organization,
#                 CS 90 046
#                 13067 St. Paul-lez-Durance Cedex
#                 France
#
# This file is part of ITER CODAC software.
# For the terms and conditions of redistribution or use of this software
# refer to the file ITER-LICENSE.TXT located in the top level directory
# of the distribution package.
#
#-======================================================================


trap bashtrap INT

if [ $# -lt 3 ]; then
 echo "Usage -> sdn-create-topic <project_dir> <topic_name> \"<topic_description>\""
 exit
fi


if [ -d $1 ]; then
 PROJDIR=$1
else
 echo "$1 project directory does not exist!"
 exit
fi

TOPIC_NAME=`echo $2 | tr ':-' '_' | tr a-z A-Z`
TOPIC_DESCRIPTION=$3


PROJ_TOPIC_DIR_PATH="$PROJDIR/src/main/c++"

if [ -d $PROJ_TOPIC_DIR_PATH ]; then
  echo "Setting topic directory path $PROJ_TOPIC_DIR_PATH/include"
else
  echo "$PROJ_TOPIC_DIR_PATH does not exist or is not a standard project directory"
exit
fi

# Topic definition header file directory (default: /opt/codac/apps/include)
export SDN_TOPIC_DIRECTORY="$PROJ_TOPIC_DIR_PATH/include"

# create include dir and if exist then do nothing
mkdir -pv $SDN_TOPIC_DIRECTORY


# topic_multicast_map.dat directory (default: /etc/opt/codac/sdn)
export SDN_CONFIG_DIRECTORY="$PROJ_TOPIC_DIR_PATH/include"

# Log file directory (default: /var/opt/codac/sdn)
export SDN_LOG_DIRECTORY="$PROJDIR/target/var/log/"


TEMPLATE_FILE_DIRECTORY="${CODAC_ROOT}/include/sdn"
TOPIC_TEMPLATE_FILE="topicdefinition.template"
MULTICAST_TEMPLATE_FILE="topic_multicast_map.template"
MULTICAST_DATA_FILE="topic_multicast_map.dat"

#if [ -z ${SDN_TOPIC_DIRECTORY} ]; then
#SDN_TOPIC_DIRECTORY="${CODAC_ROOT}/apps/include"
#fi

#if [ -z ${SDN_CONFIG_DIRECTORY} ]; then
#SDN_CONFIG_DIRECTORY="/etc/opt/codac/sdn"
#fi


echo "SDN_TOPIC_DIRECTORY=${SDN_TOPIC_DIRECTORY}"
echo "SDN_CONFIG_DIRECTORY=${SDN_CONFIG_DIRECTORY}"

#if [ ! -d ${SDN_TOPIC_DIRECTORY}  ]; then
#	# create ${SDN_TOPIC_DIRECTORY} 
#	mkdir -pv ${SDN_TOPIC_DIRECTORY}
#fi


#if [ ! -d ${SDN_CONFIG_DIRECTORY}  ]; then
#	# create ${SDN_CONFIG_DIRECTORY}
#	mkdir -pv ${SDN_CONFIG_DIRECTORY}
#fi


if [ ! -f ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE}  ]; then
	# copy to ${MULTICAST_TEMPLATE_FILE} to ${MULTICAST_DATA_FILE}
	cp ${TEMPLATE_FILE_DIRECTORY}/${MULTICAST_TEMPLATE_FILE} ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE}
fi

if [ ! -s ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE}  ]; then
	# if file size is 0, copy to ${MULTICAST_TEMPLATE_FILE} to ${MULTICAST_DATA_FILE}
	cp ${TEMPLATE_FILE_DIRECTORY}/${MULTICAST_TEMPLATE_FILE} ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE}
fi

# create .h and .cpp files from the template by 
# replacing <topicname> tag with the actual topic name 

echo topic_name: ${TOPIC_NAME}
echo topic_description: ${TOPIC_DESCRIPTION}

sed -e "s/<TOPICNAME>/\U${TOPIC_NAME}/g" -e "s/<topic_description>/${TOPIC_DESCRIPTION}/" \
	${TEMPLATE_FILE_DIRECTORY}/${TOPIC_TEMPLATE_FILE} > ${SDN_TOPIC_DIRECTORY}/${TOPIC_NAME}.h

if [ '0' == `grep -c "^${TOPIC_NAME}\b" ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE}` ]; then
sed -e "0,/\*/s/\*/${TOPIC_NAME}/" ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE} > ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE}.tmp
mv -f ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE}.tmp ${SDN_CONFIG_DIRECTORY}/${MULTICAST_DATA_FILE}
fi


echo "$SDN_TOPIC_DIRECTORY/$TOPIC_NAME -> Topic header file is created and now edit the header file to define topic user data"

bashtrap(){
 echo "CTRL+C Detected !...catching trap !" 
}
