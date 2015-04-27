/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id: SharedMemoryMapDrv.cpp 3 2012-01-15 16:26:07Z aneto $
 *
**/

#include "SharedMemoryMapDrv.h"
#include "GlobalObjectDataBase.h"

bool SharedMemoryMapDrv::ObjectLoadSetup(ConfigurationDataBase &info,StreamInterface *err){
    AssertErrorCondition(Information, "SharedMemoryMapDrv::ObjectLoadSetup: %s Loading signals", Name());

    CDBExtended cdb(info);
    if(!GenericAcqModule::ObjectLoadSetup(info,err)){
        AssertErrorCondition(InitialisationError,"SharedMemoryMapDrv::ObjectLoadSetup: %s GenericAcqModule::ObjectLoadSetup Failed",Name());
        return False;
    }

    cdb.ReadInt32(readMemKey,  "ReadMemKey", -1);
    cdb.ReadInt32(writeMemKey, "WriteMemKey", -1);

    if(readMemKey == -1 && writeMemKey == -1){
        AssertErrorCondition(InitialisationError,"SharedMemoryMapDrv::ObjectLoadSetup: either a read or a write key must be specified. Both are -1",Name());
        return False;
    }

    if(readMemKey != -1){
        readMemSize = NumberOfInputs() *  sizeof(int32);
        readMem     = (char *)SharedMemoryAlloc(readMemKey, readMemSize);
        if(readMem == NULL){
            AssertErrorCondition(InitialisationError,"SharedMemoryMapDrv::ObjectLoadSetup: failed to allocated %d bytes for the read memory",Name(),readMemSize);
            return False;
        }
    }

    if(writeMemKey != -1){
        writeMemSize = NumberOfOutputs() *  sizeof(int32);
        writeMem     = (char *)SharedMemoryAlloc(writeMemKey, writeMemSize);
        if(writeMem == NULL){
            AssertErrorCondition(InitialisationError,"SharedMemoryMapDrv::ObjectLoadSetup: failed to allocated %d bytes for the write memory",Name(),writeMemSize);
            return False;
        }
    }

    return True;
}

/**
 * GetData
 */
int32 SharedMemoryMapDrv::GetData(uint32 usecTime, int32 *ibuffer, int32 bufferNumber){
    lastUsecTime = *ibuffer;
    memcpy(ibuffer, readMem, readMemSize);
    return 1;
}

bool SharedMemoryMapDrv::WriteData(uint32 usecTime, const int32* buffer){
    memcpy(writeMem, buffer, writeMemSize);
    return True;
}

OBJECTLOADREGISTER(SharedMemoryMapDrv, "$Id: SharedMemoryMapDrv.cpp 3 2012-01-15 16:26:07Z aneto $")

