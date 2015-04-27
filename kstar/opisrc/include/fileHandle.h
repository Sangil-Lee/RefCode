// Filename : fileHandle.h
//
// ***************************************************************************
// 
// Copyright(C) 2005, BNF Technology, All rights reserved.
// Proprietary information, BNF Technology
// 
// ***************************************************************************
//
// ***** HIGH LEVEL DESCRIPTION **********************************************
//
// Module for handling text ini files
//
// **** REVISION HISTORY *****************************************************
//
// Revision 1 2003-12-23
// Author: ysw
// Initial revision
//
// ***************************************************************************

#ifndef	_FILEHANDLE_H
#define	_FILEHANDLE_H

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

// for getrlimit, getrusage, setrlimit
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include "typedefine.h"

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#if 0
typedef struct tokened_st
{
	int type;
	char *sta;
	char *end;
	struct tokened_st *next;
} tokened;
#endif

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

//class FileHandle : public RtObject
class FileHandle
{

public :

	static const uint8	TOKEN_DELIMITER		= '=';
	static const uint8	COMMENT_DELIMITER	= '#';
	static const uint32	MAX_LINE_SIZE		= 1024;
	static const uint32	MAX_RECORD_SIZE		= 4096;

	enum FileType_e { REGULAR, DIRECTORY };
	//enum TokenType_e { TOK_TOKEN_X, TOK_TOKEN_O };

	// Constructor
	FileHandle ();

	// Destructor
	~FileHandle ();

	// open file with filename
	void open (const char *fileName);

	// open file with file pointer
	void open (FILE *fp);

	// set token delimiter
	inline void setTokenDelimiter (const uint8 delimiter = TOKEN_DELIMITER) { m_tokenDelimiter = delimiter; };

	// set comment delimiter
	inline void setCommentDelimiter (const uint8 delimiter = COMMENT_DELIMITER) { m_commentDelimiter = delimiter; };

	// get line from file
	const char *getLine ();

	// get token from file
	int getToken (char *name, char *comment);

	// get token from string
	int getToken (const char *buffer, char *name, char *comment);
	
	// get token from string
	static int getToken (const char *buffer, const uint8 delimiter, char *name, char *comment);
	
	// get token from string
	static char *getToken (const char *buffer, const char *delimiters, char *token);
	
	// delete front and near white space
	static void delSpace (char *str);
	
	// delete rear white space
	static void delSpaceRear (const char *src, char *dest);
	
	// delete all white space
	static void delSpaceAll (char *str);

#if 0
	bool strToken(const char *str, const char *token, tokened **tokened_head);
	bool strTokenfree ( tokened *tokened_head );
	bool getTokenStr ( const char *str, const char *token, int &tokentype);
#endif
	
	const int getValidUser (int &uid, int &gid);
	// close file
	void close ();

	// -----------------------------------------------------------
	// static operations :
	// below operations can be called without object instance
	// -----------------------------------------------------------

	// check that file is exist or not
	static const boolean isExist (const char *path);

	static const int size (const char *path);
	static const int type (const char *path);
	static const int copy (const char *src, const char *dest);
	static const int append (const char *src, const char *dest);	// added by EHS on 2004/07/20
	static const int remove (const char *path);
	static const int move (const char *src, const char *dest);		// same as rename
	static const int rename (const char *src, const char *dest);	// same as move
	static const int getStat (const char *path, int &type, int &size);	// get type and size of file
	static const int setValidUser (const char *path);				// change the owner (uid and gid)

	// seperate directory and file name from path
	static const int getDirFileName (const char *path, char *pDirName, char *pFileName);

	// make directory recursively
	static const int makeRecurDir (const char *path);
	static const int makeDir (const char *dirName);

	// limits
	static const int getLimits (const int resource, struct rlimit &rp);
	static const int getMaxFileSize ();
	static const int getMaxNumOpenFiles ();
	static const int getCurrMaxNumOpenFiles ();
	static const int setMaxNumOpenFiles (const int num);
	static const int getUsedUserTime (int &sec, int &usec);
	static const int getUsedSysTime (int &sec, int &usec);

	// write lock
	static const int isLock (const int fd, const int offset = 0, const int size = 0);
	static const int lock (const int fd, const boolean fBlocking = TRUE, const int offset = 0, const int size = 0);
	static const int unlock (const int fd, const int offset = 0, const int size = 0);
	static const int getLockStat (const int fd, struct flock &lock, const int offset = 0, const int size = 0);

private :

	FILE	*m_fp;
	char	m_lineBuffer[MAX_LINE_SIZE];	// buffer to save one line data
	boolean	m_fImportFp;

	uint8	m_tokenDelimiter;
	uint8	m_commentDelimiter;
	
	void isOK ();
	static const int write (const char *src, const char *dest, const char *mode);
};

// ---------------------------------------------------------------------------
// Function Declarations
// ---------------------------------------------------------------------------

#endif	// _FILEHANDLE_H

