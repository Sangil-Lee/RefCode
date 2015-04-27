// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#if !defined(WIN32)
#ifndef USE_DF// for don't use df command
#include <sys/vfs.h>
#endif
#endif
// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "kuFile.h"

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

kuFile::kuFile ()
{
}

kuFile::~kuFile ()
{
}

char *kuFile::getToken (const char *buffer, const char *delimiters, char *token)
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
	printf ("[kuFile::getToken] %s : %s\n", tmp, delimiters);
#endif

	char	ch;
	kuInt32	len = strlen(tmp);
	kuInt32	dellen = strlen(delimiters);
	kuInt32	i, offset;

	for (i = 0, offset = 0; i < len; i++) {
		ch = *tmp;

		for (kuInt32 j = 0; j < dellen; j++) {
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

void kuFile::delSpace (char *str)
{
	if (NULL == str) {
		return;
	}

#ifdef SYS_DEBUG
	printf ("[kuFile::delSpace] input str %d (%s)\n", strlen(str), str);
#endif

	kuUInt32	i;
	char		tmp[KU_MAX_LINE_SIZE];

	strncpy (tmp, str, KU_MAX_LINE_SIZE-1);

	// delete rear space
	for (i = strlen(tmp)-1; i >= 0; i--) {
		if (!isspace ((kuInt32)tmp[i]))	break;
	}

	tmp[i+1] = 0x00;

	// delete front space
	for (i = 0; i < strlen(tmp); i++) {
		if (!isspace ((kuInt32)tmp[i]))	break;
	}

	str[0] = 0x00;

	if (NULL != tmp + i) {
		strcpy (str, tmp+i);
	}

#ifdef SYS_DEBUG
	printf ("[kuFile::delSpace] output str %d (%s)\n", strlen(str), str);
#endif
}

void kuFile::delSpaceRear (const char *src, char *dest)
{
	if (NULL == src || NULL == dest) {
		return;
	}

#ifdef SYS_DEBUG
	printf ("[kuFile::delSpaceRear] input src %d (%s)\n", strlen(src), src);
#endif

	kuInt32		i;

	// delete rear space
	for (i = strlen(src)-1; i >= 0; i--) {
		if (!isspace ((kuInt32)src[i]))	break;
	}

	strncpy (dest, src, i+1);
	dest[i+1] = 0x00;

#ifdef SYS_DEBUG
	printf ("[kuFile::delSpace] output dest %d (%s)\n", strlen(dest), dest);
#endif
}

void kuFile::delSpaceAll (char *str)
{
	if (NULL == str) {
		return;
	}

#ifdef SYS_DEBUG
	printf ("[kuFile::delSpaceAll] input str (%s)\n", str);
#endif

	kuUInt32		i, j;
	char		tmp[KU_MAX_LINE_SIZE];

	// delete all space
	for (i = 0, j = 0; i < strlen(str); i++) {
		if (!isspace ((kuInt32)str[i]))	{
			tmp[j++] = str[i];
		}
	}

	tmp[j] = 0x00;

	strcpy (str, tmp);

#ifdef SYS_DEBUG
	printf ("[kuFile::delSpaceAll] output str (%s)\n", str);
#endif
}

const kuBoolean kuFile::isExist (const char *path)
{
	if (NULL == path) {
		return (kuFALSE);
	}

	if (access (path, F_OK) < 0) {
		return (kuFALSE);
	}

	return (kuTRUE);
}

const kuInt32 kuFile::remove (const char *path)
{
	if (NULL == path) {
		return (kuNOK);
	}

	const kuInt32	fileType = type (path);

	if (DIRECTORY == fileType) {
		if (rmdir (path) < 0) {
			return (kuNOK);
		}
	}
	else if (REGULAR == fileType) {
		if (unlink (path) < 0) {
			return (kuNOK);
		}
	}
	else {
		return (kuNOK);
	}

	return (kuOK);
}

const kuInt32 kuFile::move (const char *src, const char *dest)
{
	return (rename (src, dest));
}

const kuInt32 kuFile::rename (const char *src, const char *dest)
{
	if ( (NULL == src) || (NULL == dest) ) {
		return (kuNOK);
	}

#if defined(WIN32)
	remove(dest);
#endif

	if (::rename (src, dest) < 0) {
		return (kuNOK);
	}

	return (kuOK);
}

const kuInt64 kuFile::size (const char *path)
{
	kuInt32	type;
	kuInt64	size;

	if (kuOK != getStat (path, type, size)) {
		return (kuNOK);
	}

	return (size);
}

const kuInt32 kuFile::type (const char *path)
{
	kuInt32	type;
	kuInt64	size;

	if (kuOK != getStat (path, type, size)) {
		return (kuNOK);
	}

	return (type);
}

const kuInt32 kuFile::getStat (const char *path, kuInt32 &type, kuInt64 &size)
{
	struct stat	statbuf;

	if (stat (path, &statbuf) < 0) {
#ifdef SYS_DEBUG
		perror ("stat failed");
#endif
		return (kuNOK);
	}

	size = statbuf.st_size;

	if (S_ISREG(statbuf.st_mode)) {
		type = REGULAR;
	}
	else if (S_ISDIR(statbuf.st_mode)) {
		type = DIRECTORY;
	}

#ifdef SYS_DEBUG
	printf ("[kuFile::getStat] file(%s) size(%ld) type(%d)\n", path, size, type);
#endif

	return (kuOK);
}

const kuInt32 kuFile::copy (const char *src, const char *dest)
{
	return ( write (src, dest, "w+") );
}

const kuInt32 kuFile::append (const char *src, const char *dest)
{
	return ( write (src, dest, "a+") );
}

const kuInt32 kuFile::write (const char *src, const char *dest, const char *mode)
{
	FILE		*fd_in, *fd_out;
	mode_t		oldMask;
	char		msg_buf[KU_MAX_RECORD_SIZE];
	size_t		len;
    kuInt32		result;

	if (NULL == src || NULL == dest || NULL == mode) {
		return (kuNOK);
	}

	oldMask	= umask (0);

	if ( NULL == (fd_in = fopen (src, "r")) ) {
		umask (oldMask);
		return (kuNOK);
	}

	if ( NULL == (fd_out = fopen (dest, mode)) ) {
		fclose (fd_in);

		umask (oldMask);
		return (kuNOK);
	}

	result = kuOK;

	while (1) {
		if ( 0 >= (len = fread (msg_buf, 1, KU_MAX_RECORD_SIZE, fd_in )) ) {
			if (kuFALSE == feof(fd_in)) {
				result = kuNOK;
			}
			break;
		}

		if (fwrite (msg_buf, 1, len, fd_out) < len) {
			result = kuNOK;
			break;
		}
	}

	fclose (fd_in);
	fclose (fd_out);

	umask (oldMask);

	if (kuOK != result) {
		unlink (dest);
	}

	return (result);
}

// convert path delimiter
const char *kuFile::convertDelimiter (const char *src, char *dest, const epicsUInt8 origin, const epicsUInt8 newer)
{
	if ((NULL == src) || (NULL == dest)) {
		return (NULL);
	}

	kuUInt32	i;
	for (i = 0; i < strlen(src); i++) {
		dest[i]	= (origin == src[i]) ? newer : src[i];
	}
	dest[i]	= 0x00;

#ifdef SYS_DEBUG
	printf ("[kuFile::convertDelimiter] %s -> %s\n", src, dest);
#endif

	return (dest);
}

// convert path delimiter to kuInt32ernal style ('/')
const char *kuFile::convertPath (char *path)
{
	return ( convertPath (path, path) );
}

const char *kuFile::convertPath (const char *src, char *dest)
{
	return ( convertDelimiter (src, dest, '\\', '/') );
}

// convert path delimiter to host's style (windows : '\', linux : '/')
const char *kuFile::convertPathToHost (char *path)
{
	return ( convertPathToHost (path, path) );
}

const char *kuFile::convertPathToHost (const char *src, char *dest)
{
#if defined(WIN32)
	return ( convertDelimiter (src, dest, '/', '\\') );
#else
	return ( convertDelimiter (src, dest, '\\', '/') );
#endif
}

// seperate directory and file name from path
const kuInt32 kuFile::getDirFileName (const char *pPath, char *pDirName, char *pFileName)
{
	if ((NULL == pPath) || (NULL == pDirName) || (NULL == pFileName)) {
		return (kuNOK);
	}

	char	path[256];
	convertPath (pPath, path);

#ifdef SYS_DEBUG
	printf ("[kuFile::getDirFileName] %s\n", path);
#endif

	char	*ptr = NULL;

	ptr = (char *)strrchr (path, '/');

	if (NULL == ptr) {
		pDirName[0] = 0x00;
		strcpy (pFileName, path);
	}
	else {
		kuInt32		nDirNameLen = strlen(path) - strlen(ptr);

		strncpy (pDirName, path, nDirNameLen);
		pDirName[nDirNameLen] = 0x00;

		strcpy (pFileName, ptr+1);
	}

#ifdef SYS_DEBUG
	printf ("[kuFile::getDirFileName] dir(%s) file(%s)\n", pDirName, pFileName);
#endif

	return (kuOK);
}

const kuInt32 kuFile::makeRecurDir (const char *pPath)
{
	if (NULL == pPath) {
		return (kuNOK);
	}

	char	path[256];
	convertPath (pPath, path);

#ifdef SYS_DEBUG
	printf ("[kuFile::makeRecurDir] %s\n", path);
#endif

	// change umask and save old umask
	mode_t		oldMask	= umask (0);

	kuInt32		nDirNameLen;
	char		dirName[128], tmp[64];
	char		*ptr;
	const char	*pDir;

	ptr			= NULL;
	pDir		= path;
	dirName[0]	= 0x00;

	do {
		ptr = (char *)strchr (pDir, '/');

		if (NULL != ptr) {
			nDirNameLen = strlen(pDir) - strlen(ptr);

			strncpy (tmp, pDir, nDirNameLen);
			tmp[nDirNameLen] = 0x00;

			pDir = ptr + 1;
		}
		else {
			strcpy (tmp, pDir);
		}

#if !defined(WIN32)
		strcat (dirName, tmp);
		strcat (dirName, "/");
#else
		if (dirName[0] == 0x00) {	// Ìºí¿?'\'  Ç¾  (C:\)
			strcat (dirName, tmp);
			strcat (dirName, "/");
		}
		else {
			strcat (dirName, "/");
			strcat (dirName, tmp);
		}
#endif

		if ( (0 != strcmp (dirName, ".")) && (0 != strcmp (dirName, "..")) ) {
			if (kuOK != makeDir (dirName)) {
				umask (oldMask);
				return (kuNOK);
			}
		}
	} while (NULL != ptr);

	// restore old umask
	umask (oldMask);

	return (kuOK);
}

const kuInt32 kuFile::makeDir (const char *pDirName, const kuBoolean fForced)
{
	if (NULL == pDirName) {
		return (kuNOK);
	}

#ifdef SYS_DEBUG
	printf ("[kuFile::makeDir] dir(%s) fForced(%d)\n", pDirName, fForced);
#endif

	// exist
	if (0 == access (pDirName, F_OK)) {
		if (DIRECTORY == type (pDirName)) {
			return (kuOK);
		}
		else if (kuTRUE != fForced) {
			return (kuNOK);
		}
		else if (kuOK != remove (pDirName)) {
			return (kuNOK);
		}
	}

	mode_t	oldMode	= umask (0);

	// make directory
	if (0 > mkdir (pDirName, KU_DIR_CREAT_MODE)) {
		umask (oldMode);

		printf ("[kuFile::makeDir] %s create failed : %s\n", pDirName, strerror(errno));
		return (kuNOK);
	}

	umask (oldMode);

	return (kuOK);
}

const kuInt64 kuFile::getMaxFileSize ()
{
#if !defined(WIN32)
	struct rlimit rp;

	if (getrlimit (RLIMIT_FSIZE, &rp) < 0) {
		perror ("[kuFile::getMaxFileSize] getrlimit failed");
		return (kuNOK);
	}

	return (rp.rlim_max);
#else
	return (INT_MAX);
#endif
}

const kuInt32 kuFile::getLimits (const kuInt32 resource, struct rlimit &rp)
{
#if !defined(WIN32)
	if (getrlimit (resource, &rp) < 0) {
		perror ("[kuFile::getLimits] getrlimit failed");
		return (kuNOK);
	}
#else
	memset (&rp, 0x00, sizeof(rp));
#endif

	return (kuOK);
}

const kuInt32 kuFile::getMaxNumOpenFiles ()
{
#if !defined(WIN32)
	struct rlimit rp;

	if (kuOK != getLimits (RLIMIT_NOFILE, rp)) {
		return (kuNOK);
	}

	return (rp.rlim_max);
#else
	return (1024);
#endif
}

const kuInt32 kuFile::getCurrMaxNumOpenFiles ()
{
#if !defined(WIN32)
	struct rlimit rp;

	if (kuOK != getLimits (RLIMIT_NOFILE, rp)) {
		return (kuNOK);
	}

	return (rp.rlim_cur);
#else
	return (0);
#endif
}

const kuInt32 kuFile::setMaxNumOpenFiles (const kuInt32 num)
{
#if !defined(WIN32)
	struct rlimit rp;

	if (kuOK != getLimits (RLIMIT_NOFILE, rp)) {
		return (kuNOK);
	}

	if (0 > num || num > (kuInt32)rp.rlim_max) {
		printf ("[kuFile::setMaxNumOpenFiles] num(%d) is invalid", num);
		return (kuNOK);
	}

	rp.rlim_cur	= num;

	if (setrlimit (RLIMIT_NOFILE, &rp) < 0) {
		perror ("[kuFile::setMaxNumOpenFiles] setrlimit failed");
		return (kuNOK);
	}
#endif

	return (kuOK);
}

const kuInt32 kuFile::getUsedUserTime (kuInt32 &sec, kuInt32 &usec)
{
#if !defined(WIN32)
	struct rusage rp;

	if (getrusage (RUSAGE_SELF, &rp) < 0) {
		perror ("[kuFile::getUsedUserTime] getrusage failed");
		return (kuNOK);
	}

	sec		= rp.ru_utime.tv_sec;
	usec	= rp.ru_utime.tv_usec;
#else
	sec	= usec	= 0;
#endif

	return (kuOK);
}

const kuInt32 kuFile::getUsedSysTime (kuInt32 &sec, kuInt32 &usec)
{
#if !defined(WIN32)
	struct rusage rp;

	if (getrusage (RUSAGE_SELF, &rp) < 0) {
		perror ("[kuFile::getUsedSysTime] getrusage failed");
		return (kuNOK);
	}

	sec		= rp.ru_stime.tv_sec;
	usec	= rp.ru_stime.tv_usec;
#else
	sec	= usec	= 0;
#endif

	return (kuOK);
}

const kuInt32 kuFile::getLockStat (const kuInt32 fd, struct flock &lock, const kuInt64 offset, const kuInt64 size)
{
#if !defined(WIN32)
	memset (&lock, 0x00, sizeof(struct flock));

    lock.l_type		= F_UNLCK; 
    lock.l_start	= offset;
    lock.l_whence	= SEEK_SET;
    lock.l_len		= size;

    if (0 > fcntl(fd, F_GETLK, &lock)) {
		printf ("[kuFile::getLockStat] fd(%d) offset(%ld) size(%ld) failed : %s", fd, offset, size, strerror(errno));
		return (kuNOK);
	}

	// Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK : 0, 1, 2
	printf ("[kuFile::getLockStat] fd(%d) lock(%d)", fd, lock.l_type);
#endif

	return (kuOK);
}

const kuInt32 kuFile::isLock (const kuInt32 fd, const kuInt64 offset, const kuInt64 size)
{
#if !defined(WIN32)
	struct flock	lock;

	if (kuOK != getLockStat (fd, lock, offset, size)) {
		return (kuNOK);
	}

	if ( (F_RDLCK == lock.l_type) || (F_WRLCK == lock.l_type) ) {
		return (kuTRUE);
	}
#endif

	return (kuFALSE);
}

const kuInt32 kuFile::lock (const kuInt32 fd, const kuBoolean fBlocking, const kuInt64 offset, const kuInt64 size)
{
#if defined(WIN32)
	if (0 > _locking (fd, _LK_LOCK, size)) {
		if (kuTRUE == fBlocking) {
			printf ("[kuFile::lock] fd(%d) block(%d) offset(%ld) size(%ld) failed : %s\n", fd, fBlocking, offset, size, strerror(errno));
		}

		return (kuNOK);
	}
#else
	struct flock	lock;

    lock.l_type		= F_WRLCK; 
    lock.l_start	= offset;
    lock.l_whence	= SEEK_SET;
    lock.l_len		= size;

	kuInt32		cmd	= (kuTRUE == fBlocking) ? F_SETLKW : F_SETLK;

	if (0 > fcntl (fd, cmd, &lock)) {
		if (kuTRUE == fBlocking) {
			printf ("[kuFile::lock] fd(%d) block(%d) offset(%ld) size(%ld) failed : %s\n", fd, fBlocking, offset, size, strerror(errno));
		}

		return (kuNOK);
	}
#endif

	return (kuOK);
}

const kuInt32 kuFile::unlock (const kuInt32 fd, const kuInt64 offset, const kuInt64 size)
{
#if defined(WIN32)
	if (0 > _locking (fd, _LK_UNLCK, size)) {
		printf ("[kuFile::unlock] fd(%d) offset(%ld) size(%ld) failed : %s\n", fd, offset, size, strerror(errno));
		return (kuNOK);
	}
#else
	struct flock	lock;

    lock.l_type		= F_UNLCK; 
    lock.l_start	= offset;
    lock.l_whence	= SEEK_SET;
    lock.l_len		= size;

	if (0 > fcntl (fd, F_SETLK, &lock)) {
		printf ("[kuFile::unlock] fd(%d) offset(%ld) size(%ld) failed : %s\n", fd, offset, size, strerror(errno));
		return (kuNOK);
	}
#endif

	return (kuOK);
}

// return the used rate of disk for target file system
// kuNOK (Error), 0 <= (%)
const kuInt32 kuFile::getUsedRate (const char *pTargetDir)
{
	kuInt64		nTotSize, nUsedSize, nAvailableSize;
	kuUInt32	nUsedRate;

	if (kuOK != getFileSystemInfo (pTargetDir, nTotSize, nUsedSize, nAvailableSize, nUsedRate)) {
		return (kuNOK);
	}

	return (nUsedRate);
}

const kuInt32 kuFile::getFileSystemInfo (const char *pTargetDir,
		kuInt64 &nTotSize, kuInt64 &nUsedSize, kuInt64 &nAvailableSize, kuUInt32 &nUsedRate)
{
    kuInt64		totalSpace	= 0;
    kuInt64		spaceFree	= 0;
    kuInt64		spaceUsed	= 0;
    kuInt32		percentUsed	= 0;

#if !defined(WIN32)
	if (NULL == pTargetDir) {
		printf ("[kuFile::getFileSystemInfo] input directory is invalid\n");
		return (kuNOK);
	}
	
	if (kuTRUE != kuFile::isExist (pTargetDir)) {
		printf ("[kuFile::getFileSystemInfo] directory(%s) not exist\n", pTargetDir);
		return (kuNOK);
	}
	
	if (kuFile::DIRECTORY != kuFile::type (pTargetDir)) {
		printf ("[kuFile::getFileSystemInfo] %s is not directory\n", pTargetDir);
		return (kuNOK);
	}

#ifndef USE_DF// for don't use df command
    struct  statfs  lstatfs;
    if(statfs(pTargetDir,&lstatfs)<0) {
        printf ("[kuFile::getFilesystemInfo] %s open failed \n", pTargetDir);
        return (kuNOK);
    }

    totalSpace  = lstatfs.f_blocks * (lstatfs.f_bsize/1024);
    spaceFree   = lstatfs.f_bavail * (lstatfs.f_bsize/1024);
    spaceUsed   = totalSpace - spaceFree;
    percentUsed = (kuInt32)(((double)spaceUsed/(double)totalSpace)*100);

#else	// USE_DF

	char	command[128];
	FILE 	*fp	= NULL;

	sprintf (command, "df -k -P %s", pTargetDir);

	if ( NULL == (fp = popen (command, "r")) ) {
		printf ("[kuFile::getFileSystemInfo] %s open failed\n", command);
		return (kuNOK);
	}

	char	buf[1024];
	char	fileSystemName[128];
	char	fileSystemRoot[128];
	kuBoolean	fMatched;

	fMatched	= kuFALSE;
	
	while (1) {
		if (feof(fp)) {
			break;
		}

		if (NULL == fgets (buf, sizeof(buf), fp)) {
			break;
		}

		fileSystemName[0]	= 0x00;
		fileSystemRoot[0]	= 0x00;

		sscanf (buf, "%s %ld %ld %ld %d%% %s", fileSystemName, &totalSpace, &spaceUsed, &spaceFree, &percentUsed, fileSystemRoot);

		if (0 != strcmp ("Filesystem", fileSystemName)) {
			fMatched	= kuTRUE;
			break;
		}
	}
	
	pclose (fp);

	if (kuTRUE != fMatched) {
		return (kuNOK);
	}
#endif	// USE_DF

#else	// WIN32

	struct _diskfree_t df = {0};
	kuInt32		uDrive	= 0;
	if(pTargetDir[0]=='\\' || pTargetDir[0]=='/') uDrive = _getdrive();
	else uDrive	= ::toupper(pTargetDir[0]) - 'A' + 1;
 
	if (0 != _getdiskfree(uDrive, &df)) {
		return (kuNOK);
	}

	totalSpace	= df.total_clusters;
	spaceFree	= df.avail_clusters;
	spaceUsed	= df.total_clusters - df.avail_clusters;
	percentUsed	= nUsedSize / nTotSize * 100;

#endif	// WIN32

	nTotSize        = totalSpace;
	nAvailableSize  = spaceFree;
	nUsedSize       = spaceUsed;
	nUsedRate       = percentUsed;

#ifdef SYS_DEBUG
	printf ("[kuFile::getFileSystemInfo] %s : tot(%ld) used(%ld) avail(%ld) use(%d)\n",
			pTargetDir, nTotSize, nUsedSize, nAvailableSize, nUsedRate);
#endif

	return (kuOK);
}

// End of File
