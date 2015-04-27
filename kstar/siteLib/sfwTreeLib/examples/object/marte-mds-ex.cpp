#include <mdsobjects.h>

#define statusOk(status)((status) & 1) 

/** Stores collected data to MDSplus */
bool MDSPlusHandler::StoreToMds() {

		// allocate a new Tree object
		MDSplus::Tree* mdsTreeObj;

		try {
			mdsTreeObj = new MDSplus::Tree((char*)mdsTree.Buffer(), mdsShot);             	
		} // end try
		catch (MDSplus::MdsException* eObj) {
			AssertErrorCondition(FatalError,"STORESIGNALS MDSplus::Tree(%s,%d) error opening tree for shot [%s]", mdsTree.Buffer(), mdsShot, eObj->what());
			cdbx->MoveToFather();
			return False;
		} // end catch
		
				BString nodeName;

					// check data type to create a data vector of MDSplus
					MDSplus::Array* descDataObj;
					int lenData = signalGCO->NumberOfSamples();
					try {
						if(signalGCO->Type() == BTDFloat)
							descDataObj = new MDSplus::Float32Array((float*)signalGCO->Buffer(), lenData);
						}
						else {
							AssertErrorCondition(FatalError,"STORESIGNALS Signal %s data type not supported", nodeName.Buffer());
						} // end else
					} // end try
					catch (MDSplus::MdsException* eObj) {
						// return False; ITS sostituited with:
						continue;
					} // end catch
					
					// create symbolic node name
					char mdsSignalName[256] = ":";
					strcat(mdsSignalName, nodeName.Buffer());
					// create the node object
					MDSplus::TreeNode* mdsNodeObj;
					try {
						mdsNodeObj = mdsTreeObj->getNode(mdsSignalName);
						mdsNodeObj->putData(descDataObj);
						// delete created MDSplus objects
						delete mdsNodeObj;
						delete descDataObj;
					}
					catch (MDSplus::MdsException* eObj) {
						printf ("STORESIGNALS mdsTreeObj->getNode(%s) OR mdsNodeObj->putData(...) error [%s]\n",
								mdsSignalName, eObj->what());
					}
				} // end cdbx->NodeName(nodeName)
				cdbx->MoveToFather();
			} // end cdbx->MoveToChildren(i)
		} // end for
		cdbx->MoveToFather();            

		// delete MDsplus Tree object
		try {
			delete mdsTreeObj;
		}
		catch (MDSplus::MdsException* eObj) {
			 printf ("STORESIGNALS cannot delete mdsTreeObj [%s]\n", eObj->what());
		}

		// MDSplus disconnect socket
		// TODO

}

