// Filename : fileHandle.cpp
// ***** HIGH LEVEL DESCRIPTION **********************************************
//
// Module for handling text ini files
// Don't use Debug API
//
// ***************************************************************************
//
// **** REVISION HISTORY *****************************************************
//
// Revision 1  2003-12-23
// Initial revision
//
// ***************************************************************************

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "fileHandle.h"

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Import Global Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Export Global Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Module Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Function Definitions 
// ---------------------------------------------------------------------------

FileHandle::FileHandle ()
{
	m_fp		= NULL;
	m_fImportFp	= FALSE;

	// set default token delimiter
	m_tokenDelimiter	= TOKEN_DELIMITER;
	m_commentDelimiter	= COMMENT_DELIMITER;
}

FileHandle::~FileHandle ()
{
	try {
		close ();
	}
	catch (...) { }

	m_fp = NULL;
}

void FileHandle::open (const char *fileName)
{
	// check if fileName is NULL
	if (NULL == fileName) {
		//setErrNum (RT_ENAME);

		printf ("[FileHandle::open] input file name is NULL\n");
		return;
	}
	
	// checkif file is not exist
	if (TRUE != isExist (fileName)) {
		//setErrNum (RT_ENOTFOUND);

		printf ("[FileHandle::open] file %s not found\n", fileName);
		return;
	}
	
	m_fp = fopen (fileName, "r");

	if (NULL == m_fp) {
		//setErrNum (errno);

		printf ("[FileHandle::open] file %s open failed\n", fileName);
		return;
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::open] file %s open ok (0x%x)\n", fileName, (int)m_fp);
#endif
}

void FileHandle::open (FILE *fp)
{
	m_fp		= fp;
	m_fImportFp	= TRUE;
}

const char *FileHandle::getLine ()
{
	isOK();

	while (1) {
		if (feof(m_fp)) {
#ifdef SYS_DEBUG
			printf ("[FileHandle::getToken] feof 1\n");
#endif
			return (NULL);
		}

		if (NULL == fgets (m_lineBuffer, sizeof(m_lineBuffer), m_fp)) {
#ifdef SYS_DEBUG
			printf ("[FileHandle::getToken] feof 2\n");
#endif
			return (NULL);
		}

		delSpace (m_lineBuffer);

		if ((m_commentDelimiter == (m_lineBuffer[0])) || 
				('\n' == m_lineBuffer[0]) ||
				(0x00 == m_lineBuffer[0])) {
			continue;
		}

		break;
	}

	return (m_lineBuffer);
}

int FileHandle::getToken (char *name, char *comment)
{
	isOK();

	if (NULL == getLine()) {
		return (NOK);
	}

	return (getToken (m_lineBuffer, name, comment));
}

int FileHandle::getToken (const char *buffer, char *name, char *comment)
{
	isOK();

	return (getToken (buffer, m_tokenDelimiter, name, comment));
}

int FileHandle::getToken (const char *buffer, const uint8 delimiter, char *name, char *comment)
{
	char	*ptr = NULL;

	if ((NULL == buffer) || (NULL == name) || (NULL == comment)) {
		//setErrNum (RT_EARGINVALID);
		return (NOK);
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::getToken] %s\n", buffer);
#endif

	if (NULL == (ptr = strchr (buffer, delimiter))) {
		strcpy (name, buffer);
#ifdef SYS_DEBUG
		printf ("[FileHandle::getToken] syntax invalid : %s\n", buffer);
#endif
		return (NOK);
	}

	char	t1[MAX_LINE_SIZE], t2[MAX_LINE_SIZE];

	strcpy (t1, buffer);
	t1[strlen(buffer)-strlen(ptr)] = 0x00;

	strcpy (t2, ptr+1);

	FileHandle::delSpace (t1);
	FileHandle::delSpace (t2);

	strcpy (name, t1);
	strcpy (comment, t2);

#ifdef SYS_DEBUG
	printf ("[FileHandle::getToken] name(%s) comment(%s)\n", name, comment);
#endif

	return (OK);
}

char *FileHandle::getToken (const char *buffer, const char *delimiters, char *token)
{
	static	char *tmp = NULL;
	static	char *ptr = NULL;

	if (NULL != buffer) {
		if (NULL != tmp) {
			free (ptr);
		}

		tmp = (char *)malloc(strlen(buffer)+1);
		ptr = tmp;

		strcpy (tmp, buffer);

		delSpaceAll (tmp);
	}

	if (strlen(tmp) == 0) {
		return (NULL);
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::getToken] %s : %s\n", tmp, delimiters);
#endif

	char	ch;
	int		len = strlen(tmp);
	int		dellen = strlen(delimiters);
	int		i, offset;

	for (i = 0, offset = 0; i < len; i++) {
		ch = *tmp;

		for (int j = 0; j < dellen; j++) {
			if (ch == delimiters[j]) {
				if (offset > 0) {
					token[offset] = 0x00;
					return (tmp);
				}

				if (!isspace(ch)) {
					token[offset] = ch;
					token[offset+1] = 0x00;

					return (++tmp);
				}
			}
		}

		token[offset] = ch;
		offset++;
		tmp++;
	}

	token[offset] = 0x00;

	return (tmp);
}

void FileHandle::delSpace (char *str)
{
	if (NULL == str) {
		//setErrNum (RT_EARGINVALID);
		return;
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::delSpace] input str %d (%s)\n", strlen(str), str);
#endif

	int32		i;
	char		tmp[MAX_LINE_SIZE];

	strncpy (tmp, str, MAX_LINE_SIZE-1);

	// delete rear space
	for (i = strlen(tmp)-1; i >= 0; i--) {
		if (!isspace ((int)tmp[i]))	break;
	}

	tmp[i+1] = 0x00;

	// delete front space
	for (i = 0; i < (int)strlen(tmp); i++) {
		if (!isspace ((int)tmp[i]))	break;
	}

	str[0] = 0x00;

	if (NULL != tmp + i) {
		strcpy (str, tmp+i);
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::delSpace] output str %d (%s)\n", strlen(str), str);
#endif
}

void FileHandle::delSpaceRear (const char *src, char *dest)
{
	if (NULL == src || NULL == dest) {
		//setErrNum (RT_EARGINVALID);
		return;
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::delSpaceRear] input src %d (%s)\n", strlen(src), src);
#endif

	int32		i;

	// delete rear space
	for (i = strlen(src)-1; i >= 0; i--) {
		if (!isspace ((int)src[i]))	break;
	}

	strncpy (dest, src, i+1);
	dest[i+1] = 0x00;

#ifdef SYS_DEBUG
	printf ("[FileHandle::delSpace] output dest %d (%s)\n", strlen(dest), dest);
#endif
}

void FileHandle::delSpaceAll (char *str)
{
	if (NULL == str) {
		//setErrNum (RT_EARGINVALID);
		return;
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::delSpaceAll] input str (%s)\n", str);
#endif

	uint32		i, j;
	char		tmp[MAX_LINE_SIZE];

	// delete all space
	for (i = 0, j = 0; i < strlen(str); i++) {
		if (!isspace ((int)str[i]))	{
			tmp[j++] = str[i];
		}
	}

	tmp[j] = 0x00;

	strcpy (str, tmp);

#ifdef SYS_DEBUG
	printf ("[FileHandle::delSpaceAll] output str (%s)\n", str);
#endif
}

void FileHandle::close ()
{
	isOK ();

	if (TRUE != m_fImportFp) {
		fclose (m_fp);
	}

	m_fp = NULL;
}

const boolean FileHandle::isExist (const char *path)
{
	if (NULL == path) {
		//setErrNum (RT_EARGINVALID);
		return (FALSE);
	}

	if (access (path, F_OK) < 0) {
		return (FALSE);
	}

	return (TRUE);
}

void FileHandle::isOK ()
{
	if (NULL == m_fp) {
		//setErrNum (RT_EINIT);
		return;
	}
}

const int FileHandle::remove (const char *path)
{
	if (NULL == path) {
		//setErrNum (RT_EARGINVALID);
		return (NOK);
	}

	const int	fileType = type (path);

	if (DIRECTORY == fileType) {
		if (rmdir (path) < 0) {
			//setErrNum (errno);
			return (NOK);
		}
	}
	else if (REGULAR == fileType) {
		if (unlink (path) < 0) {
			//setErrNum (errno);
			return (NOK);
		}
	}
	else {
		return (NOK);
	}

	return (OK);
}

const int FileHandle::move (const char *src, const char *dest)
{
	return (rename (src, dest));
}

const int FileHandle::rename (const char *src, const char *dest)
{
	if ( (NULL == src) || (NULL == dest) ) {
		//setErrNum (RT_EARGINVALID);
		return (NOK);
	}

	if (::rename (src, dest) < 0) {
		//setErrNum (errno);
		return (NOK);
	}

	return (OK);
}

const int FileHandle::size (const char *path)
{
	int		type, size;

	if (OK != getStat (path, type, size)) {
		return (NOK);
	}

	return (size);
}

const int FileHandle::type (const char *path)
{
	int		type, size;

	if (OK != getStat (path, type, size)) {
		return (NOK);
	}

	return (type);
}

const int FileHandle::getStat (const char *path, int &type, int &size)
{
	struct stat	statbuf;

	if (stat (path, &statbuf) < 0) {
		//setErrNum (errno);

#ifdef SYS_DEBUG
		perror ("stat failed");
#endif
		return (NOK);
	}

	size = statbuf.st_size;

	if (S_ISREG(statbuf.st_mode)) {
		type = REGULAR;
	}
	else if (S_ISDIR(statbuf.st_mode)) {
		type = DIRECTORY;
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::getStat] file(%s) size(%d) type(%d)\n", path, size, type);
#endif

	return (OK);
}

const int FileHandle::copy (const char *src, const char *dest)
{
	return ( write (src, dest, "w+") );
}

const int FileHandle::append (const char *src, const char *dest)
{
	return ( write (src, dest, "a+") );
}

const int FileHandle::write (const char *src, const char *dest, const char *mode)
{
	FILE		*fd_in, *fd_out;
	mode_t		oldMask;
	char		msg_buf[MAX_RECORD_SIZE];
	size_t		len;
    int			result;

	if (NULL == src || NULL == dest || NULL == mode) {
		//setErrNum (RT_EARGINVALID);
		return (NOK);
	}

	oldMask	= umask (0);

	if ( NULL == (fd_in = fopen (src, "r")) ) {
		//setErrNum (errno);

		umask (oldMask);
		return (NOK);
	}

	if ( NULL == (fd_out = fopen (dest, mode)) ) {
		fclose (fd_in);
		//setErrNum (errno);

		umask (oldMask);
		return (NOK);
	}

	result = OK;

	while (1) {
		if ( 0 >= (len = fread (msg_buf, 1, MAX_RECORD_SIZE, fd_in )) ) {
			if (FALSE == feof(fd_in)) {
				result = NOK;
			}
			break;
		}

		if (fwrite (msg_buf, 1, len, fd_out) < len) {
			result = NOK;
			break;
		}
	}

	fclose (fd_in);
	fclose (fd_out);

	umask (oldMask);

	if (OK != result) {
		unlink (dest);
	}

	return (result);
}

// seperate directory and file name from path
const int FileHandle::getDirFileName (const char *path, char *pDirName, char *pFileName)
{
	if ((NULL == path) || (NULL == pDirName) || (NULL == pFileName)) {
		//setErrNum (RT_EARGINVALID);
		return (NOK);
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::getDirFileName] %s\n", path);
#endif

	char	*ptr = NULL;

	ptr = strrchr (path, '/');

	if (NULL == ptr) {
		pDirName[0] = 0x00;
		strcpy (pFileName, path);
	}
	else {
		int		nDirNameLen = strlen(path) - strlen(ptr);

		strncpy (pDirName, path, nDirNameLen);
		pDirName[nDirNameLen] = 0x00;

		strcpy (pFileName, ptr+1);
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::getDirFileName] dir(%s) file(%s)\n", pDirName, pFileName);
#endif

	return (OK);
}

const int FileHandle::makeRecurDir (const char *path)
{
	if (NULL == path) {
		//setErrNum (RT_EARGINVALID);
		return (NOK);
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::makeRecurDir] %s\n", path);
#endif

	// change umask and save old umask
	mode_t		oldMask	= umask (0);

	int			nDirNameLen;
	char		dirName[128], tmp[64];
	char		*ptr;
	const char	*pDir;

	ptr			= NULL;
	pDir		= path;
	dirName[0]	= 0x00;

	do {
		ptr = strchr (pDir, '/');

		if (NULL != ptr) {
			nDirNameLen = strlen(pDir) - strlen(ptr);

			strncpy (tmp, pDir, nDirNameLen);
			tmp[nDirNameLen] = 0x00;

			pDir = ptr + 1;
		}
		else {
			strcpy (tmp, pDir);
		}

		strcat (dirName, tmp);
		strcat (dirName, "/");

		if ( (0 != strcmp (dirName, ".")) && (0 != strcmp (dirName, "..")) ) {
			if (OK != makeDir (dirName)) {
				umask (oldMask);
				return (NOK);
			}
		}
	} while (NULL != ptr);

	// restore old umask
	umask (oldMask);

	return (OK);
}

const int FileHandle::makeDir (const char *pDirName)
{
	if (NULL == pDirName) {
		//setErrNum (RT_EARGINVALID);
		return (NOK);
	}

#ifdef SYS_DEBUG
	printf ("[FileHandle::makeDir] %s\n", pDirName);
#endif

	// not exist
	if (0 > access (pDirName, F_OK)) {
		mode_t	oldMode	= umask (0);

		// make directory
		if (0 > mkdir (pDirName, 0777)) {
			umask (oldMode);

			//setErrNum (errno);

			printf ("[FileHandle::makeDir] %s create failed : %s\n", pDirName, strerror(errno));
			return (NOK);
		}

		umask (oldMode);
	}
	else if (DIRECTORY != type (pDirName)) {
		return (NOK);
	}

	return (OK);
}

const int FileHandle::setValidUser (const char *path)
{
	if ( (0 != getuid()) && (0 != geteuid()) ) {
		return (OK);
	}

	if (NULL == path) {
		printf ("[FileHandle::setValidUser] path is invalid");
		return (NOK);
	}

	int		uid = 0;
	int		gid = 0;

#if 0
	if (OK != RtObject::getValidUser (uid, gid)) {
		return (NOK);
	}
#endif

	if (0 > chown (path, uid, gid)) {
		printf ("[FileHandle::setValidUser] %s chown failed : %s\n", path, strerror(errno));
		return (NOK);
	}

	return (OK);
}

const int FileHandle::getMaxFileSize ()
{
	struct rlimit rp;

	if (getrlimit (RLIMIT_FSIZE, &rp) < 0) {
		perror ("[FileHandle::getMaxFileSize] getrlimit failed");
		return (NOK);
	}

	return (rp.rlim_max);
}

const int FileHandle::getLimits (const int resource, struct rlimit &rp)
{
	if (getrlimit (resource, &rp) < 0) {
		perror ("[FileHandle::getLimits] getrlimit failed");
		return (NOK);
	}

	return (OK);
}

const int FileHandle::getMaxNumOpenFiles ()
{
	struct rlimit rp;

	if (OK != getLimits (RLIMIT_NOFILE, rp)) {
		return (NOK);
	}

	return (rp.rlim_max);
}

const int FileHandle::getCurrMaxNumOpenFiles ()
{
	struct rlimit rp;

	if (OK != getLimits (RLIMIT_NOFILE, rp)) {
		return (NOK);
	}

	return (rp.rlim_cur);
}

const int FileHandle::setMaxNumOpenFiles (const int num)
{
	struct rlimit rp;

	if (OK != getLimits (RLIMIT_NOFILE, rp)) {
		return (NOK);
	}

	if (0 > num || num > (int)rp.rlim_max) {
		printf ("[FileHandle::setMaxNumOpenFiles] num(%d) is invalid", num);
		return (NOK);
	}

	rp.rlim_cur	= num;

	if (setrlimit (RLIMIT_NOFILE, &rp) < 0) {
		perror ("[FileHandle::setMaxNumOpenFiles] setrlimit failed");
		return (NOK);
	}

	return (OK);
}

const int FileHandle::getUsedUserTime (int &sec, int &usec)
{
	struct rusage rp;

	if (getrusage (RUSAGE_SELF, &rp) < 0) {
		perror ("[FileHandle::getUsedUserTime] getrusage failed");
		return (NOK);
	}

	sec		= rp.ru_utime.tv_sec;
	usec	= rp.ru_utime.tv_usec;

	return (OK);
}

const int FileHandle::getUsedSysTime (int &sec, int &usec)
{
	struct rusage rp;

	if (getrusage (RUSAGE_SELF, &rp) < 0) {
		perror ("[FileHandle::getUsedSysTime] getrusage failed");
		return (NOK);
	}

	sec		= rp.ru_stime.tv_sec;
	usec	= rp.ru_stime.tv_usec;

	return (OK);
}

const int FileHandle::getLockStat (const int fd, struct flock &lock, const int offset, const int size)
{
	memset (&lock, 0x00, sizeof(struct flock));

    lock.l_type		= F_UNLCK; 
    lock.l_start	= offset;
    lock.l_whence	= SEEK_SET;
    lock.l_len		= size;

    if (0 > fcntl(fd, F_GETLK, &lock)) {
		printf ("[FileHandle::getLockStat] fd(%d) offset(%d) size(%d) failed : %s", fd, offset, size, strerror(errno));
		return (NOK);
	}

	// Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK : 0, 1, 2
	printf ("[FileHandle::getLockStat] fd(%d) lock(%d)", fd, lock.l_type);

	return (OK);
}

const int FileHandle::isLock (const int fd, const int offset, const int size)
{
	struct flock	lock;

	if (OK != getLockStat (fd, lock, offset, size)) {
		return (NOK);
	}

	if ( (F_RDLCK == lock.l_type) || (F_WRLCK == lock.l_type) ) {
		return (TRUE);
	}

	return (FALSE);
}

const int FileHandle::lock (const int fd, const boolean fBlocking, const int offset, const int size)
{
	struct flock	lock;

    lock.l_type		= F_WRLCK; 
    lock.l_start	= offset;
    lock.l_whence	= SEEK_SET;
    lock.l_len		= size;

	int		cmd	= (TRUE == fBlocking) ? F_SETLKW : F_SETLK;

	if (0 > fcntl (fd, cmd, &lock)) {
		if (TRUE == fBlocking) {
			printf ("[FileHandle::lock] fd(%d) block(%d) offset(%d) size(%d) failed : %s", fd, fBlocking, offset, size, strerror(errno));
		}

		return (NOK);
	}

	return (OK);
}

const int FileHandle::unlock (const int fd, const int offset, const int size)
{
	struct flock	lock;

    lock.l_type		= F_UNLCK; 
    lock.l_start	= offset;
    lock.l_whence	= SEEK_SET;
    lock.l_len		= size;

	if (0 > fcntl (fd, F_SETLK, &lock)) {
		printf ("[FileHandle::unlock] fd(%d) offset(%d) size(%d) failed : %s", fd, offset, size, strerror(errno));
		return (NOK);
	}

	return (OK);
}
const int FileHandle::getValidUser (int &uid, int &gid)
{
	// get environment for run user
	char	*pRunUserName	= NULL;

	if (NULL == (pRunUserName = getenv ("kstar"))) {
		printf ("\n");
		printf ("    -----------------------------------------------------------------------------\n");
		printf ("     The environment variable RIDESRUNUSER was not set in your account. \n");
		printf ("     Please check whether the current account is correct to run RIDES system. \n");
		printf ("     If account is correct, first define environment variable RIDESRUNUSER. \n");
		printf ("     Then retry ... \n");
		printf ("    -----------------------------------------------------------------------------\n");
		printf ("\n");

		return (NOK);
	}

	// get uid of run account
	struct passwd	*pwd = NULL;

	if (NULL == (pwd = getpwnam (pRunUserName))) {
		printf ("\n");
		printf ("    -----------------------------------------------------------------------------\n");
		printf ("     The environment variable RIDESRUNUSER is invalid \n");
		printf ("     Cannot found user %s in /etc/passwd \n", pRunUserName);
		printf ("     Please check and fix, then retry ... \n");
		printf ("    -----------------------------------------------------------------------------\n");
		printf ("\n");

		return (NOK);
	}

	if ( (0 != getuid()) && (0 != geteuid()) && (pwd->pw_uid != getuid()) ) {
		printf ("\n");
		printf ("    -----------------------------------------------------------------------------\n");
		printf ("     The running user is not both root or %s \n", pRunUserName);
		printf ("     Please check and fix, then retry ... \n");
		printf ("    -----------------------------------------------------------------------------\n");
		printf ("\n");

		return (NOK);
	}

	uid	= pwd->pw_uid;
	gid	= pwd->pw_gid;

	return (OK);
}

#if 0
bool FileHandle::strToken(const char *str, const char *token, tokened **tokened_head)
{
	tokened *cur = NULL, *prev = NULL;
	char chr; int i = 0, cnt = 0; int token_len = 0;
	char *last_token_occur = NULL;
	if( str == NULL ||token == NULL )
    {
		return false;
	};
	token_len = strlen ( token );
	last_token_occur = (char *) str;
	while ( ( chr = *( str + cnt ) ) != '\0' )
    {
		for( i = 0; token_len > i; ++i )
        {
			if ( chr == token[i] )
            {
				if ( (str + cnt) > last_token_occur )
                {
					//cur=(tokened *)malloc(sizeof(tokened));
					cur = new tokened;
					memset (cur, 0x0, sizeof(tokened) );
					cur -> type = TOK_TOKEN_X;
					cur -> sta  = last_token_occur;
					cur -> end  = (char *)(str+cnt);
					if ( (*tokened_head) == NULL )
                    {
						(*tokened_head) = cur;
					}
                    else
                    {
						prev -> next = cur;
					};
					prev = cur;
				};

				cur = new tokened;
				memset( cur, 0x0, sizeof(tokened) );
				cur->type = TOK_TOKEN_O;
				cur->sta = (char *)(str+cnt);
				cur->end = (char *)(str+cnt+1);

				if ( (*tokened_head) == NULL ) (*tokened_head) = cur;
                else	prev->next=cur;
				prev = cur;

				last_token_occur = (char *)(str+cnt+1);
				break;
			};
		};
		++cnt;
	};
	if ( ( str + cnt ) > last_token_occur )
    {
		cur = new tokened;
		memset(cur, 0x0, sizeof(tokened));
		cur->type = TOK_TOKEN_X;
		cur->sta = last_token_occur;
		cur->end = (char *)(str+cnt);
		if ( (*tokened_head) == NULL) (*tokened_head) = cur;
        else		 prev -> next = cur;
		prev = cur;
	};
	return true;
};

bool FileHandle::getTokenStr ( const char *str, const char *token, int &tokentype)
{
	char *buf = NULL;
	int size = 0;
	tokened *tokened_head = NULL, *cur = NULL;
	strToken ( str, token, &tokened_head );
	cur = tokened_head;
	while ( cur != NULL )
	{
		size = (cur->end - cur->sta);
		buf = new char[size+1];
		buf[size+1] = '\0';
		memcpy( buf, cur->sta, size);
		tokentype = cur->type;
		printf("%s : '%s'\n",((cur->type==TOK_TOKEN_O) ? "TOKEN_O" : "TOKEN_X"),buf);
		cur = cur->next;
	};
	strTokenfree(tokened_head);
	delete [] buf;
	return true;
}

bool FileHandle::strTokenfree ( tokened *tokened_head )
{
    tokened *cur = NULL, *next = NULL;
    cur = tokened_head;
    while( cur!=NULL)
    {
        next=cur->next;
        //free(cur);
        delete cur;
        cur=next;
    };
    return 0;
};
#endif
// End of File
