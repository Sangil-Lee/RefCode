//+============================================================================
// $HeadURL$
// $Id$
//
// Project       : CODAC Core System
//
// Description   : sdn-monitor program
//
// Author        : Hyung Gi KIM
//
// Copyright (c) : 2010-2013 ITER Organization,
//                 CS 90 046
//                 13067 St. Paul-lez-Durance Cedex
//                 France
//
// This file is part of ITER CODAC software.
// For the terms and conditions of redistribution or use of this software
// refer to the file ITER-LICENSE.TXT located in the top level directory
// of the distribution package.
//
//-============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>

#include "sdn_common/sdnmon_msg.h"
#include "sdn_common/sdnmon_msg_reg.h"
#include "sdn_common/sdnmon_msg_ui.h"
#include "sdn_common/log_buffer.h"
#include "list_topic.h"
#include "list_node.h"

using namespace std; 


#define MAX_CMD_LEN 512
#define	printStr	printf

static int gbExit = 0;


static void UIParseCmdLine(char* pszCmdBuf);
static int GetString(char *pBuff, int nLen);
static int MyStrToUL(char* pStr, char nBase);

static int ExitCmdHdlr(char **argv, int argc);
static int HelpCmdHdlr(char **argv, int argc);
static int TestCmdHdlr(char **argv, int argc);
static int LogLevelCmdHdlr(char **argv, int argc);
static int TopicCmdHdlr(char **argv, int argc);
static int NodeCmdHdlr(char **argv, int argc);


typedef struct {
	const char* cmd;
	int (*func)(char **, int);
	const char* description;
} CMD_TABLE;

static const CMD_TABLE gCmdTable[] = 
{
    { "exit",		ExitCmdHdlr,		"exit application"},
	{ "help",		HelpCmdHdlr, 		"display help"},
	{ "test",		TestCmdHdlr,		"test"},
	{ "loglevel",	LogLevelCmdHdlr,	"set log level (1~5)"},
	{ "topic",		TopicCmdHdlr,		"view topic status"},
	{ "node",		NodeCmdHdlr,		"view node status"},
    { NULL, NULL, NULL}
};



int ProcessCommand()
{
    char szCmdBuf[MAX_CMD_LEN];

    memset(szCmdBuf, 0, MAX_CMD_LEN);

    printStr("\n\n");
    printStr("Type \"help\" to get a list of commands.\n");

    while (!gbExit)
    {
        printStr("\n>> ");

        if (!GetString(szCmdBuf, MAX_CMD_LEN))
            UIParseCmdLine(szCmdBuf);
    }

	return 0;
}



static void UIParseCmdLine(char* pszCmdBuf)
{
    int nCount=0;

    #define MAX_ARGC	10
    #define SKIP_CHAR(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '"')
    char nArgc=0;
    char* pArgv[MAX_ARGC];
    char* pTmp=NULL;

    if (pszCmdBuf == NULL)
        return;

    for (pTmp=pszCmdBuf ; SKIP_CHAR(*pTmp) ; pTmp++)
    {
        ;
    }

    if (*pTmp == '\0')
        return;
        
    // Tokenize the command line (must be null-terminted).
    for (nArgc=1, pArgv[0]=pTmp ; *pTmp != '\0' ; pTmp++)
    {
        if(SKIP_CHAR(*pTmp))
        {
            *pTmp = '\0';
            do
            {
                ++pTmp;
            } while(*pTmp != '\0' && SKIP_CHAR(*pTmp));
            if (*pTmp != '\0')
            {
                if (++nArgc >  MAX_ARGC)
                {
                    break;
                }

                pArgv[nArgc - 1]=pTmp; 
            }
        }
    
    }
  
    // Look up command in command table.
    while(gCmdTable[nCount].cmd != NULL)
    {
        if(!strcmp(pArgv[0], gCmdTable[nCount].cmd))
        {
                int (*fp)(char **, int) = gCmdTable[nCount].func;
                fp((char**)pArgv, (int)nArgc);
                return;
        }
        ++nCount;
    }   

    printStr("\nERROR: Unknown command (%s)\n", pszCmdBuf);
}



static int GetString(char *pBuff, int nLen)
{
	int i;
	for(i=0; i<nLen; i++)
	{
		pBuff[i] = getchar();
		if((pBuff[i] == '\r') || (pBuff[i] == '\n'))
			break;
	}

	i++;
	if(i < nLen)
		pBuff[i] = 0;
	else if(nLen > 0)
		pBuff[nLen-1] = 0;

	return 0;
}



static int MyStrToUL(char* pStr, char nBase)
{
    int nPos=0;
    char c;
    int nVal = 0;
    int nCnt=0;
    int n=0;
    int len;

    if (pStr == NULL)
        return(0);

	for(len=1; len < (int)strlen(pStr); len++)
	{
        c = *(pStr + len);
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))
        	continue;
        else
        	break;
	}
	
    for (nPos=0 ; nPos < len ; nPos++)
    {
        c = *(pStr + len - 1 - nPos);
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'a' && c <= 'f')
            c = c - 'a' + 10;
        else
        	break;

        for (nCnt=0, n=1 ; nCnt < nPos ; nCnt++)
            n *= nBase;
        nVal += (n * c);
    } 

    return(nVal);    
}



static int ExitCmdHdlr(char **argv, int argc)
{
	gbExit = 1;
	return 0;
}


  
static int HelpCmdHdlr(char **argv, int argc)
{
	int nCnt=0;

	#define MAXCMDNAMELEN 10
	char szFill[MAXCMDNAMELEN];
	int nTmp=0;

	printStr("\n\n");
	for(nCnt=0 ; gCmdTable[nCnt].cmd != NULL ; nCnt++)
	{
		memset(szFill, ' ', MAXCMDNAMELEN);
		nTmp = ((strlen(gCmdTable[nCnt].cmd) < MAXCMDNAMELEN) ? (MAXCMDNAMELEN - strlen(gCmdTable[nCnt].cmd) - 1) : 0);
		szFill[nTmp] = '\0';
		printStr("%s%s - %s\n", gCmdTable[nCnt].cmd, szFill, gCmdTable[nCnt].description);
	}

	return 0;
}
  
  
  
static int TestCmdHdlr(char **argv, int argc)
{
	int i_param1 = 0;

	if(argc >= 2)
	{
		if ( strncmp(argv[1], "0x", 2) == 0 )
			i_param1 = MyStrToUL(&argv[1][2], 16);
		else
			i_param1 = MyStrToUL(&argv[1][0], 10);
	}

	if(i_param1 == 1)
	{
		KeyValue kv;
		KeyValue sub_kv;
		list<KeyValue> list_kv;
	
		while(1)
		{
			for(int i=0; i<1000; i++)
			{
				list_kv.clear();
				
				kv.clean();
				kv.m_strKey = "aaaaaaaa";
				kv.m_strValue = "11111111";
	
				sub_kv.clean();
				sub_kv.m_strKey = "bbbbbbbb";
				sub_kv.m_strValue = "22222222";
				kv.m_listKeyValue.push_back(sub_kv);
	
				list_kv.push_back(kv);
	
				kv.clean();
				kv.m_strKey = "cccccccc";
				kv.m_strValue = "33333333";
	
				sub_kv.clean();
				sub_kv.m_strKey = "dddddddd";
				sub_kv.m_strValue = "44444444";
				kv.m_listKeyValue.push_back(sub_kv);
	
				list_kv.push_back(kv);
			}
	
			sleep(1);			
		}
	}

	return 0;
}



static int LogLevelCmdHdlr(char **argv, int argc)
{
	int i_param1 = 0;

	if(argc < 2)
	{
		printStr("current log level is %d\n", theSdnLogBuf.getLogLevel());
		return 0;
	}

	if ( strncmp(argv[1], "0x", 2) == 0 )
		i_param1 = MyStrToUL(&argv[1][2], 16);
	else
		i_param1 = MyStrToUL(&argv[1][0], 10);

	printStr("set log level to %d\n", i_param1);
	theSdnLogBuf.setLogLevel(i_param1);

	return 0;
}



static int TopicCmdHdlr(char **argv, int argc)
{
	std::vector<TopicInfo*>::iterator iter;
	std::vector<TopicPubsubInfo*>::iterator iter_pubsub;
	int count;

	if(argc < 2)
	{
		printStr("-----------------------------------------\n");
		printStr("SDN status : [%s]\n", (g_csSdnTopicDb.getStatusSdn() == STATUS_SDN_READY) ? "READY" : "NOT READY");
		printStr("-----------------------------------------\n");

		printStr("\n");
		printStr("[Topic status]\n");
		for(iter = g_csSdnTopicDb.m_vectorTopicInfo.begin(); iter != g_csSdnTopicDb.m_vectorTopicInfo.end(); iter++)
		{
			printStr("%s: %s\n", (*iter)->m_csMetadata.m_strName.c_str(), 
					((*iter)->getStatusTopic() == STATUS_TOPIC_READY) ? "READY" : "NOT_READY");
		}
		
		return 0;
	}

	for(iter = g_csSdnTopicDb.m_vectorTopicInfo.begin(); iter != g_csSdnTopicDb.m_vectorTopicInfo.end(); iter++)
	{
		if((*iter)->m_csMetadata.m_strName.compare(argv[1]) == 0)
		{
			printStr("-----------------------------------------\n");
			printStr("Topic [%s]\n", (*iter)->m_csMetadata.m_strName.c_str());
			printStr("status: %s\n", ((*iter)->getStatusTopic() == STATUS_TOPIC_READY) ? "READY" : "NOT_READY");
			printStr("\n");

			printStr("[Metadata]\n");
			printStr("topic name : [%s]\n", (*iter)->m_csMetadata.m_strName.c_str());
			printStr("description: [%s]\n", (*iter)->m_csMetadata.m_strDescription.c_str());
			printStr("version    : [%d]\n", (*iter)->m_csMetadata.m_version);
			printStr("is_event   : [%d]\n", (*iter)->m_csMetadata.m_bEvent); 
			printStr("\n");

			printStr("[Publisher]\n");
			printStr("host name  : [%s]\n", (*iter)->m_csPubInfo.m_strHostName.c_str());
			printStr("app name   : [%s]\n", (*iter)->m_csPubInfo.m_strAppName.c_str());
			printStr("ip address : [%s]\n", (*iter)->m_csPubInfo.m_strIpAddr.c_str());
			printStr("status     : [%s]\n", ((*iter)->m_csPubInfo.getStatusTopicPubsub() == STATUS_TOPIC_PUBSUB_REGISTERED) ? 
					"REGISTERED" : "NOT REGISTERED");
			printStr("\n");

			count = 0;
			for(iter_pubsub = (*iter)->m_vectorSubInfo.begin(); iter_pubsub != (*iter)->m_vectorSubInfo.end(); iter_pubsub++)
			{
				printStr("[Subscriber %d]\n", count++);
				printStr("host name  : [%s]\n", (*iter_pubsub)->m_strHostName.c_str());
				printStr("app name   : [%s]\n", (*iter_pubsub)->m_strAppName.c_str());
				printStr("ip address : [%s]\n", (*iter_pubsub)->m_strIpAddr.c_str());
				printStr("status     : [%s]\n", ((*iter_pubsub)->getStatusTopicPubsub() == STATUS_TOPIC_PUBSUB_REGISTERED) ? 
						"REGISTERED" : "NOT REGISTERED");
				printStr("\n");
			}
			
			printStr("\n");
			return 0;
		}
	}

	printStr("No matched topic [%s]\n", argv[1]);

	return 0;
}



static int NodeCmdHdlr(char **argv, int argc)
{
	std::list<NodeInfo*>::iterator iter;
	std::list<NodePubsubInfo*>::iterator iter_pubsub;
	int count;
	int pub_count;
	int sub_count;
	struct tm* pGmtime;
	char* p_status_node;

	if(argc < 2)
	{
		printStr("-----------------------------------------\n");
		printStr("SDN status : [%s]\n", (g_csSdnTopicDb.getStatusSdn() == STATUS_SDN_READY) ? "READY" : "NOT READY");
		printStr("-----------------------------------------\n");

		printStr("\n");

		count = 0;
		for(iter = g_csSdnNodeDb.m_listNodeInfo.begin(); iter != g_csSdnNodeDb.m_listNodeInfo.end(); iter++)
		{
			if((*iter)->getStatusNode() == STATUS_NODE_NOT_CONNECTED)
				p_status_node = (char*)"NOT CONNECTED";
			else if((*iter)->getStatusNode() == STATUS_NODE_CONNECTED)
				p_status_node = (char*)"CONNECTED";
			else if((*iter)->getStatusNode() == STATUS_NODE_REGISTERED)
				p_status_node = (char*)"REGISTERED";
			else
				p_status_node = (char*)"UNKNOWN";
			
			printStr("-----------------------------------------\n");
			printStr("[Node %d]\n", count++);
			printStr("host name  : [%s]\n", (*iter)->m_strHostName.c_str());
			printStr("app name   : [%s]\n", (*iter)->m_strAppName.c_str());
			printStr("ip address : [%s]\n", (*iter)->m_strIpAddr.c_str());
			printStr("status     : [%s]\n", p_status_node);

			pub_count = 0;
			sub_count = 0;
			for(iter_pubsub = (*iter)->m_listNodePubsubInfo.begin();
				iter_pubsub != (*iter)->m_listNodePubsubInfo.end(); iter_pubsub++)
			{
				if((*iter_pubsub)->m_bPublisher)
					pub_count++;
				else
					sub_count++;
			}
			
			printStr("number of publisher  : [%d]\n", pub_count);
			printStr("number of subscriber : [%d]\n", sub_count);
			printStr("\n");
		}
		
		return 0;
	}

	for(iter = g_csSdnNodeDb.m_listNodeInfo.begin(); iter != g_csSdnNodeDb.m_listNodeInfo.end(); iter++)
	{
		if((*iter)->m_strHostName.compare(argv[1]) == 0)
		{
			printStr("[Node Information]\n");
			printStr("host name : [%s]\n", (*iter)->m_strHostName.c_str());
			printStr("app name	: [%s]\n", (*iter)->m_strAppName.c_str());
			printStr("ip address: [%s]\n", (*iter)->m_strIpAddr.c_str());
			printStr("status    : [%d]\n", (*iter)->getStatusNode());
			
			pGmtime = gmtime(&((*iter)->m_timeConnected));
			if(pGmtime)
			{
				printStr("connected time : %04d/%02d/%02d %02d:%02d:%02d", pGmtime->tm_year+1900, pGmtime->tm_mon+1, 
					pGmtime->tm_mday, pGmtime->tm_hour, pGmtime->tm_min, pGmtime->tm_sec);
			}

			count = 0;
			for(iter_pubsub = (*iter)->m_listNodePubsubInfo.begin();
				iter_pubsub != (*iter)->m_listNodePubsubInfo.end(); iter_pubsub++)
			{
				printStr("-----------------------------------------\n");
				printStr("[Pubsub %d]\n", count++);
				printStr("topic name   : [%s]\n", (*iter_pubsub)->m_strTopicName.c_str());
				printStr("is_publisher : [%d]\n", (*iter_pubsub)->m_bPublisher);
				printStr("\n");
			}
			
			printStr("\n");
			return 0;
		}

		return 0;
	}

	printStr("No matched node for host name [%s]\n", argv[1]);

	return 0;
}


