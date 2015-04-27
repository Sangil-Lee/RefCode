#! /bin/bash

export EPICS_CA_ADDR_LIST="172.17.101.200 172.17.101.201 172.17.100.107"
export EPICS_CA_AUTO_ADDR_LIST=NO

../../bin/linux-x86/sncMdsEventPut
