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
 * $Id: TestRead.cpp 3 2012-01-15 16:26:07Z aneto $
 *
**/

#include "Memory.h"
#include "Sleep.h"

#define MEM_SIZE 4

// based on TestRead.cpp
int main(int argc, char **argv){
    if(argc < 2){
        printf("Usage: TestRead.ex key (as specified in cfg file) \n");
        return -1;
    }

    int  key  = atoi(argv[1]);
    char *mem = (char *)SharedMemoryAlloc(key, MEM_SIZE);
    if(mem == NULL){
        printf("Failed to allocated shared memory with key: %d\n", key);
        return -1;
    }

    while (1) {
        SleepSec(1.0);
		printf("Received (int version: %d) (float version: %f)\n", *((int32 *)mem), *((float *)mem));
    }
    SharedMemoryFree(mem);
}


