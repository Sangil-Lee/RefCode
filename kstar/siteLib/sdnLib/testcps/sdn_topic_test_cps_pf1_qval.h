#ifndef SDN_TEST_CPS_PF1_QVAL_H
#define SDN_TEST_CPS_PF1_QVAL_H

#include "sdn/sdn_typedef.h"

#define	test_cps_pf1_qval	test_cps_pf1_qval_metadata,test_cps_pf1_qval_userdata

/*
 * [available variable type]
 *   uint8_t, uint16_t, uint32_t, uint64_t
 *   int8_t,  int16_t,  int32_t,  int64_t
 *   float,   double
 *   * support array type
*/

class PACKED test_cps_pf1_qval_userdata {
	public:
/*-------- Topic specific userdata should be define here ------------- */
/*----------------- Edit here to add userdata items ------------------ */
		uint16_t converter1;
		uint16_t converter2;
};


class test_cps_pf1_qval_metadata
{
#define	_TEST_CPS_PF1_QVAL_NAME			"test_cps_pf1_qval"
#define	_TEST_CPS_PF1_QVAL_DESCRIPTION	"This is test CPS PF1 PS QVAL data"
#define	_TEST_CPS_PF1_QVAL_VERSION		0x10
#define	_TEST_CPS_PF1_QVAL_EVENT		1

	public:
		test_cps_pf1_qval_metadata()
		{
			name = _TEST_CPS_PF1_QVAL_NAME;
			description = _TEST_CPS_PF1_QVAL_DESCRIPTION;
			version = _TEST_CPS_PF1_QVAL_VERSION;
			payloadsize = sizeof(test_cps_pf1_qval_userdata);
			is_event = _TEST_CPS_PF1_QVAL_EVENT;
			
/* Initialization for the topic specific metadata should be set here  */
/*--------------- Edit here to initialize metadata items ------------ */
			// dummy=1;
		}
		
		const char * getName() const {return name;}
		const char * getDescription() const {return description;}
		unsigned short  getVersion() const {return version;}
		unsigned short  getPayloadsize() const {return payloadsize;}
		char isEvent() const {return is_event;}
		
	private:
		const char *name; 
		const char *description;
		unsigned short version;
		unsigned short payloadsize; 
		char is_event;
		
	public:
/* Functions which get topic specific metadata should be define here  */
/*--------- Edit here to add functions to get metadata items -------- */
		// int32_t  getDummy() const {return dummy;}

	private:
/*---- Topic specific metadata should be defined here (optional)----- */
/*------------------ Edit here to add metadata items ---------------- */
		// int32_t dummy;
};

#endif
