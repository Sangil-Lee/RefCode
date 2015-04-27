#ifndef SDN_TOPIC2E_TEST_H
#define SDN_TOPIC2E_TEST_H

#include "sdn/sdn_typedef.h"

#define	topic2e_test	topic2e_test_metadata,topic2e_test_userdata

/*
 * [available variable type]
 *   uint8_t, uint16_t, uint32_t, uint64_t
 *   int8_t,  int16_t,  int32_t,  int64_t
 *   float,   double
 *   * support array type
*/

class PACKED topic2e_test_userdata {
	public:
/*-------- Topic specific userdata should be define here ------------- */
/*----------------- Edit here to add userdata items ------------------ */
		uint8_t 	actuator1[1024];
};


class topic2e_test_metadata
{
#define	_TOPIC2E_TEST_NAME			"topic2e_test"
#define	_TOPIC2E_TEST_DESCRIPTION	"This is a topic definition file for topic2e_test event."
#define	_TOPIC2E_TEST_VERSION		1
#define	_TOPIC2E_TEST_EVENT			1

	public:
		topic2e_test_metadata()
		{
			name = _TOPIC2E_TEST_NAME;
			description = _TOPIC2E_TEST_DESCRIPTION;
			version = _TOPIC2E_TEST_VERSION;
			payloadsize = sizeof(topic2e_test_userdata);
			is_event = _TOPIC2E_TEST_EVENT;
			
/* Initialization for the topic specific metadata should be set here  */
/*--------------- Edit here to initialize metadata items ------------ */
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

	private:
/*---- Topic specific metadata should be defined here (optional)----- */
/*------------------ Edit here to add metadata items ---------------- */
};

#endif
