#ifndef SDN_TOPIC1_TEST_MODIFIED_H
#define SDN_TOPIC1_TEST_MODIFIED_H

#include "sdn/sdn_typedef.h"

#define	topic1_test_modified	topic1_test_modified_metadata,topic1_test_modified_userdata

/*
 * [available variable type]
 *   uint8_t, uint16_t, uint32_t, uint64_t
 *   int8_t,  int16_t,  int32_t,  int64_t
 *   float,   double
 *   * support array type
*/

class PACKED topic1_test_modified_userdata {
	public:
/*-------- Topic specific userdata should be define here ------------- */
/*----------------- Edit here to add userdata items ------------------ */
		uint8_t 	actuator1[1];
		uint8_t 	added1;			// for integrity check test
};


class topic1_test_modified_metadata
{
#define	_TOPIC1_TEST_MODIFIED_NAME			"topic1_test"
#define	_TOPIC1_TEST_MODIFIED_DESCRIPTION	"This is a modified topic definition file for topic1_test."
#define	_TOPIC1_TEST_MODIFIED_VERSION		1
#define	_TOPIC1_TEST_MODIFIED_EVENT			0

	public:
		topic1_test_modified_metadata()
		{
			name = _TOPIC1_TEST_MODIFIED_NAME;
			description = _TOPIC1_TEST_MODIFIED_DESCRIPTION;
			version = _TOPIC1_TEST_MODIFIED_VERSION;
			payloadsize = sizeof(topic1_test_modified_userdata);
			is_event = _TOPIC1_TEST_MODIFIED_EVENT;
			
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
