/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn/trunk/src/main/c/include/sdn/sdn_log_buf.h $
* $Id: sdn_log_buf.h 36365 2013-04-30 13:18:59Z kimh12 $
*
* Project		: CODAC Core System
*
* Description	: SDN Software - Communication API SdnLogBuf header file.
*
* Author        : Eui Jae LEE, Hyung Gi KIM (Mobiis Co., Ltd)
*
* Copyright (c) : 2010-2013 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/*! \file log_buffer.h 
 *   \brief SDN Log Functions Header File
 */
#ifndef LOG_BUFFER_H
#define LOG_BUFFER_H

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <sys/mman.h>
#include <sched.h>
#include <stdio.h>
#include <sys/ioctl.h> 
#include <stdarg.h>
#include <sys/resource.h> 
#include <sys/time.h>
#include <sys/stat.h> 
#include <errno.h> 

using namespace std; 

extern class SdnLogBuf theSdnLogBuf;
extern int g_iLogAuditLevel;


#define SDN_LOG_LEVEL_ENVNAME		"SDN_LOG_LEVEL"
#define SDN_LOG_DIRECTORY_ENVNAME	"SDN_LOG_DIRECTORY"
#define SDN_LOG_DIRECTORY_DEFAULT	"/var/opt/codac/sdn"

/** \brief SDN log message macro.
*	Log messages of which level is greater or equal than SM_LEVEL is displayed.
*/
#if 0
#define SDNMSG 							theSdnLogBuf.sdnMsg
#elif 1
#define SDNMSG(level, fmt, args...) \
	do { \
		if(level >= g_iLogAuditLevel) \
		{ \
			if(level == SM_RESULT) \
			{ \
				theSdnLogBuf.sdnMsg(level, fmt, ## args); \
			} \
			else \
			{ \
				struct timeval tv; \
				struct tm *lt; \
				char timebuf[256]; \
				gettimeofday(&tv, NULL); \
				lt = localtime((const time_t*)&tv.tv_sec); \
				strftime(timebuf, 256, "%H:%M:%S %d/%b/%Y", lt); \
				theSdnLogBuf.sdnMsg(level, fmt "  [%s][%s][%s]\n" , \
					## args, theSdnLogBuf.getHostName(), __FUNCTION__, timebuf); \
			} \
		} \
	} while(0)
#else
#define SDNMSG(level, fmt, args...) \
	do { \
		struct timeval tv; \
		gettimeofday(&tv, NULL); \
		theSdnLogBuf.sdnMsg(level, fmt "  %s  [%s][%s]\n", ## args, \
			ctime((const time_t*)&tv.tv_sec), g_NodeInfo.getHostName(), __FUNCTION__); \
	} while(0)
#endif
	// Log message level value
	#define	SM_TEST		0
	#define	SM_LOG		1
	#define	SM_INFO		2
	#define	SM_WARN		3
	#define	SM_ERR		4
	#define	SM_FATAL	5
	#define	SM_RESULT	6

// Internal
#define	LOG_BUF_NUM		1000
#define	LOG_BUF_SIZE	256
	
// m_logBufStatus_
#define	LBS_EMPTY		0
#define	LBS_WRITING		1
#define	LBS_COMPLETE	2
	

/// class SdnLogInfo
/// manages SDN log messages in the separate thread with low priority.
/// This class is initialized in initializeSDN() and deinitialized in terminateSDN()
///
class SdnLogBuf {
public:
	SdnLogBuf()
	{
		m_levelAudit_ = -1;
		m_in_ = 0;
		m_out_ = 0;
		m_idLogThread_ = (pthread_t)0;
		memset(m_logBufStatus_, 0, sizeof(m_logBufStatus_));
		memset(m_logBuf_, 0, sizeof(m_logBuf_));
	};


	/** \fn int initialize()
	 *	\brief Initialize SdnLogBuf object. 
	 *  Log thread, which manages SDN log messages with lower priority, is created by this function.
	 */
	int initialize()
	{
		int ret;
		string line;
	
		if(gethostname(hostName_, sizeof(hostName_)) != 0)
		{
			strcpy(hostName_, "unknown");
			fprintf(stderr,"[SdnLogBuf::initialize] gethostname() failed.\n");
		}
	
		if(m_idLogThread_ != (pthread_t)0)
		{
			fprintf(stderr,"[SdnLogBuf::initialize] already initialized.\n");
			return 1;
		}
	
		if(m_levelAudit_ < 0)
		{
			char* log_level = getenv(SDN_LOG_LEVEL_ENVNAME);
			if(log_level)
				if(strlen(log_level) > 0)
					m_levelAudit_ = atoi(log_level);
		}
	
		// set default value to SM_INFO if not initialized
		if(m_levelAudit_ < 0)
			m_levelAudit_ = SM_INFO;
	
		g_iLogAuditLevel = m_levelAudit_;
		
		ret = pthread_create((pthread_t*)&m_idLogThread_, NULL, &threadLog_, this);
		if(ret != 0)
		{
			fprintf(stderr,"[SdnLogBuf::initialize] pthread_create() failed.\n");
			return 2;
		}
	
		if(m_idLogThread_ == (pthread_t)0)
		{
			fprintf(stderr,"[SdnLogBuf::initialize] pthread_create() failed. m_idLogThread_=0.\n");
			return 3;
		}
	
		return 0;
	};

	
	/** \fn int deinitialize()
	 *	\brief Release resources used by SdnLogBuf object.
	 *	It also terminates log thread created in initialize()
	 */
	int deinitialize()
	{
		int count = 0;
	
		if(m_idLogThread_ == (pthread_t)0)
		{
			fprintf(stderr,"[SdnLogBuf::deinitialize] m_idLogThread_ is NULL.\n");
			return 1;
		}
	
		while(count++ < 5)
		{
			if(isLogEmpty_())
				break;
	
			sleep(1);
		}
	
		pthread_cancel(m_idLogThread_);
		m_idLogThread_ = (pthread_t)0;
	
		return 0;
	};

	
	/** \fn int setLogLevel(int log_level)
	 *	\brief Set log level of SDN log message.
	 *
	 *	The SDN log messages whose level is lower than the log_level parameter are not displayed nor saved to log file.
	 *	SDN log levels are defined adn arranged in ascending order as SM_LOG (lowest), SM_INFO, SM_WARN, SM_ERR, 
	 *	and SM_FATAL (highest). This API can be called multiple times, and the last log level is applied.
	 *	@param[in] log_level SM_LOG, SM_INFO, SM_WARN, SM_ERR or SM_FATAL
	 */
	void setLogLevel(int log_level)
	{
		m_levelAudit_ = log_level;
		g_iLogAuditLevel = m_levelAudit_;
	};
	

	/** \fn int getLogLevel()
	 *	\brief Get the current log level of SDN log message. 
	 */
	int getLogLevel()
	{
		return m_levelAudit_;
	};


	/** \fn pthread_t getLogThreadId()
	 *	\brief Get log thread ID
	 *
	 *	The thread ID may be used to change the scheduling policies to other than default values.
	 *	The thread ID is used to change thread attribute such as scheduling policy and priority.
	 *	Log thread is internal thread which manages log messages of SDN software.
	 *	This thread should run in lower priority not to impact the performance of SDN software.
	 */
	pthread_t getLogThreadId()
	{
		return m_idLogThread_;
	};


	/** \fn char* getHostName()
	 *	\brief Gets the host name of the controller.
	 */
	char* getHostName()
	{
		return hostName_;
	};

	
	/** \fn void sdnMsg(int log_level, const char* fmt, ...)
	 *	\brief Print log message to the terminal and save to the log file. 
	 *	The SDN log message is constructed by fmt and the following variable number of aguments.
	 *	This message is printed to terminal and saved to the log file if its log_level is greater
	 *	or equal than the log_level set by setLogLevel(int log_level)
	 *	@param[in] log_level is SM_LOG, SM_INFO, SM_WARN, SM_ERR or SM_FATAL.
	 *	@param[in] fmt specifies message format as in the printf()
	 */
	void sdnMsg(int log_level, const char* fmt, ...)
	{
		int in;
		va_list ap;
	
		// discard log messages of which log level is below m_levelAudit_
		if(log_level < m_levelAudit_)
			return;
			
		in = m_in_;
	
		// m_in_ : ~ m_out_-1 (cannot go through m_out_-1)
		//	- This protects the useful messages overwritten by a vast amount of repeated error messages.
		if(((in + 1) % LOG_BUF_NUM) == m_out_)
		{
			fprintf(stderr,"LogBuf Full. in=%d(%d), out=%d\n", m_in_, ((in + 1) % LOG_BUF_NUM), m_out_);
			return;
		}
	
		// m_in_ is incremented first for other messages from other threads could use the next buffer
		m_in_ = (in + 1) % LOG_BUF_NUM;
	
		m_logBufStatus_[in] = LBS_WRITING;
	
		va_start(ap, fmt);
		vsnprintf(m_logBuf_[in], LOG_BUF_SIZE-1, fmt, ap);
		va_end(ap);
	
		m_logBufStatus_[in] = LBS_COMPLETE;
	};



	//////////////////////////////////////////////////////////////////
	// Private variables and functions
private:	
	/// Log message audit level
	int m_levelAudit_;

	// m_in_ : ~ m_out_-1 (cannot go through m_out_-1)
	// m_out_: m_in_+1 ~ m_in_ (cannot go through m_in_)
	int m_in_;
	int m_out_;
	int m_logBufStatus_[LOG_BUF_NUM];
	char m_logBuf_[LOG_BUF_NUM][LOG_BUF_SIZE];
	pthread_t m_idLogThread_;
	char hostName_[128];
	
	static void* threadLog_(void* p_param)
	{
		SdnLogBuf* pSdnLogBuf = (SdnLogBuf*)p_param;
		ofstream outFile;
		string file_name;
		int in;
		int out;
		int ret;
		char app_name[80];
	
		ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (ret != 0)
			SDNMSG(SM_ERR,"threadLog_: pthread_setcancelstate(PTHREAD_CANCEL_ENABLE) failed.\n");
	
		file_name = getenv(SDN_LOG_DIRECTORY_ENVNAME) ? getenv(SDN_LOG_DIRECTORY_ENVNAME) : "";
		if(strlen(file_name.c_str()) == 0)
			file_name = SDN_LOG_DIRECTORY_DEFAULT;
		
		if(pSdnLogBuf->mkPath_(file_name) != 0)
			fprintf(stderr, "[threadLog_] create directory [%s] failed.\n", file_name.c_str());
	
		ret = pSdnLogBuf->getProgramName_(app_name, sizeof(app_name));
		if(ret != 0)
			strncpy(app_name, "sdnapp", sizeof(app_name)-1);
	
		file_name += "/";
		file_name += app_name;
		file_name += ".log";
	
		SDNMSG(SM_INFO, "log file = %s\n", file_name.c_str());
		
		outFile.open(file_name.c_str(), ios::out);
		if(outFile.is_open())
			outFile.close();
	
		while(1)
		{
			in = pSdnLogBuf->m_in_;
			out = pSdnLogBuf->m_out_;
			
			// m_out_: m_in_+1 ~ m_in_ (cannot go through m_in_)
			if(pSdnLogBuf->isLogEmpty_())
			{
				usleep(10000);
				continue;
			}
	
			if(pSdnLogBuf->m_logBufStatus_[out] == LBS_WRITING)
			{
				usleep(1000);
				continue;
			}
	
			printf(pSdnLogBuf->m_logBuf_[out]);
	
			outFile.open(file_name.c_str(), ios::app);
			if(outFile.is_open())
			{
				outFile << pSdnLogBuf->m_logBuf_[out];
				outFile.close();
			}
			
			pSdnLogBuf->m_logBufStatus_[out] = LBS_EMPTY;
	
			pSdnLogBuf->m_out_ = (out+1) % LOG_BUF_NUM;
		}
		
		return (void*)0;
	};
	
	
	int getProgramName_(char* buf, int buf_len)
	{
		int found=0;
		string line;
		ifstream status_file;
		char* cur_ptr;
		char* next_ptr;
		char* tmp;
		char* app_name;
		
		if((buf == NULL) || (buf_len < 1))
			return 1;
		
		app_name = getenv("SDN_APP_NAME") ? getenv("SDN_APP_NAME") : (char*)"";
		if(strlen(app_name) > 0)
		{
			strncpy(buf, app_name, buf_len-1);
			buf[buf_len-1] = 0;
			found = 1;
		}
		else
		{
			status_file.open("/proc/self/status", ios::in);
		
			if (status_file.is_open())
			{
				while (getline(status_file, line)) {
					if (line.empty())
						break;
		
					cur_ptr = (char*)line.c_str();
					tmp = strtok_r(cur_ptr, "'\t'", &next_ptr);
					if(tmp != NULL)
					{
						if(strcmp("Name:", tmp) == 0)
						{
							if(next_ptr != NULL)
							{
								strncpy(buf, next_ptr, buf_len-1);
								buf[buf_len-1] = 0;
								found = 1;
								break;
							}
						}
					}
				}
		
				status_file.close();
			}
		}
		
		if (found == 0)
			return 2;
	
		return 0;
	};
	
	
	int isLogEmpty_()
	{
		if(m_out_ == m_in_)
			return 1;
	
		return 0;
	};
	
	
	int mkPath_(std::string s)
	{
		size_t pre=0,pos;
		string dir;
		int mdret;
		mode_t mode = 0755;
	
		if(s[s.size()-1]!='/'){
			// force trailing / so we can handle everything in loop
			s+='/';
		}
	
		while((pos=s.find_first_of('/', pre)) != string::npos)
		{
			dir=s.substr(0,pos++);
			pre=pos;
			if(dir.size()==0) continue; // if leading / first time is 0 length
			if((mdret=mkdir(dir.c_str(), mode)) && (errno != EEXIST))
			{
				return mdret;
			}
		}
		
		return 0;
	};
};

#endif 
