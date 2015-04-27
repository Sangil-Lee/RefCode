// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

#if !defined(WIN32)
#include <dirent.h>
#include <sys/time.h>
#endif

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "kuStd.h"

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

void *kuStd::memcpy (void *dest, const void *src, const size_t &n)
{
	if (!dest || !src) { return (NULL); }

	size_t	nleft, nwrite, noffset;

	nleft	= n;
	noffset	= 0;

	while (nleft > 0) {
		nwrite = (nleft < KU_STD_MAX_COPY_SIZE) ? nleft : KU_STD_MAX_COPY_SIZE;

		::memcpy (((char *)dest) + noffset, ((char *)src) + noffset, nwrite);

		nleft   -= nwrite;
		noffset	+= nwrite;
	}

	return (dest);
}

void *kuStd::memset (void *s, const kuInt32 &c, const size_t &n)
{
	if (!s) { return (NULL); }

	size_t	nleft, nwrite, noffset;

	nleft	= n;
	noffset	= 0;

	while (nleft > 0) {
		nwrite = (nleft < KU_STD_MAX_COPY_SIZE) ? nleft : KU_STD_MAX_COPY_SIZE;

		::memset (((char *)s) + noffset, c, nwrite);

		nleft   -= nwrite;
		noffset	+= nwrite;
	}

	return (s);
}

kuInt32 kuStd::equal (const char *s1, const char *s2)
{
	if (!s1 || !s2) {
		return (kuNOK);
	}

	return ( (s1[0] == s2[0]) && (!strcmp(s1, s2)) );
}

kuInt32 kuStd::equal (const char *s1, const char *s2, const size_t &n)
{
	if (!s1 || !s2) {
		return (kuNOK);
	}

	return ( (s1[0] == s2[0]) && (!strncmp(s1, s2, n)) );
}

kuInt32 kuDoubleEqual (double v1, double v2, double gap)
{
	return (kuStd::equal (v1, v2, gap));
}

template<typename T>
kuInt32 kuStd::equal (T v1, T v2, T gap)
{
	return ( (v1 - gap <= v2) && (v2 <= v1 + gap) );
}

// 0 (equal), > 0 (left > right), < 0 (left < right)
kuInt32 kuStd::timecmp (const kuInt32 &lsec, const kuInt32 &lmsec, const kuInt32 &rsec, const kuInt32 &rmsec)
{
	kuInt32		sec, msec;

	sec		= lsec;
	msec	= lmsec;

	if (msec < rmsec) {
		sec		-= 1;
		msec	+= 1000;
	}

	if (sec > rsec) {
		return (1);
	}
	else if (sec < rsec) {
		return (-1);
	}
	else {
		return (msec - rmsec);
	}
}

// return byte alignment size
const kuInt32 kuStd::getAlignSize (const kuInt32 &size)
{
	return ( (kuInt32)( (size + (KU_STD_ALIGN_UNIT - 1)) / KU_STD_ALIGN_UNIT * KU_STD_ALIGN_UNIT) );
}

const char *kuStd::int2str (const kuInt32 ival)
{
	static char	buf[40];

	sprintf (buf, "%d", ival);

	return (buf);
}

const kuInt32 kuStd::getValueFromStr (const char *valStr)
{
	if (NULL == valStr) {
		return (0);
	}

	kuInt32   value;

	if (!strncasecmp (valStr, KU_HEXA_DECIMAL_PREIFX, strlen(KU_HEXA_DECIMAL_PREIFX))) {
		sscanf (valStr, "%x", &value);
	}
	else {
		value = atoi (valStr);
	}

	return (value);
}

const kuUInt32 kuStd::binaryToUInt32 (const char *s) 
{
	kuUInt32	i = 0;
	kuInt32		count = 0;

	while (s[count]) {
		i = (i << 1) | (s[count++] - '0');
	}

	return i;
}

const float kuStd::getFloatFromStr (const char *str)
{
	if ( (NULL == str) || (strlen(str) <= 0) ) {
		return (0);
	}

	float		fval;
	kuInt32		ival;

	if (str[0] == 'b' || str[0] == 'B') {
		fval	= (float)binaryToUInt32 (str+1);
	}
	else if (0 == strncasecmp("0x", str, 2)) {
		sscanf (str, "%x", &ival);
		fval	= (float)ival;
	}
	else {
		sscanf (str, "%f", &fval);
	}

	return (fval);
}

void kuStd::upper (const char *src, char *dest)
{
	if (!dest || !src) { return; }

	kuUInt32	i;

	for (i = 0; i < strlen(src); i++) {
		dest[i] = toupper(src[i]);
	}
	dest[i]	= 0x00;
}

void kuStd::lower (const char *src, char *dest)
{
	if (!dest || !src) { return; }

	kuUInt32	i;

	for (i = 0; i < strlen(src); i++) {
		dest[i] = tolower(src[i]);
	}
	dest[i]	= 0x00;
}

void kuStd::dump (const char *buf, const kuInt32 nMaxLenPerLine)
{
	if (!buf) { return; }

	dump ((kuInt32)strlen(buf), buf, nMaxLenPerLine);
}

void kuStd::dump (const kuInt32 len, const char *buf, const kuInt32 nMaxLenPerLine)
{
	if (!buf) { return; }

	for (kuInt32 i = 0; i < len; i++) {
		if ((0 < nMaxLenPerLine) && (0 != i) && !(i % nMaxLenPerLine)) {
			printf ("\n");
		}

		printf ("%02x ", buf[i]);
	}
	printf ("\n");
}

void kuStd::todigit (const char *src, char *dest)
{
	if (!dest || !src) { return; }

	kuUInt32	i, j;

	for (i = 0, j = 0; i < strlen(src); i++) {
		if (isdigit(src[i])) {
			dest[j++] = src[i];
		}
	}
	dest[j]	= 0x00;
}

// return printable string. if input is null, return "null" string
const char *kuStd::nullprint (const char *str)
{
	return ( (NULL == str) ? "null" : str );
}

const kuInt32 kuStd::readn (const kuInt32 fd, void *buf, const epicsUInt32 nbytes)
{
	char		*ptr = (char *)buf;
	kuInt32		nleft, nread;

	nleft = nbytes;

#if defined(WIN32) 
	memset(buf,'\0',nleft);
#endif 

	while (nleft > 0) {
		if ((nread = ::read (fd, ptr, nleft)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			else {					// Error
				return (-1);
			}
		}
		else if (0 == nread) {		// EOF

#if defined(WIN32)
			ptr[nread] = '\0';
#endif
			break;
		}

		nleft -= nread;
		ptr   += nread;
	}

	return (nbytes - nleft);
}

const kuInt32 kuStd::writen (const kuInt32 fd, const void *buf, const epicsUInt32 nbytes)
{
	char		*ptr = (char *)buf;
	kuInt32		nleft, nwritten;

	nleft = nbytes;

	while (nleft > 0) {
		if ((nwritten = ::write (fd, ptr, nleft)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			else {
				return (-1);
			}
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}

	return (nbytes - nleft);
}

#if defined(WIN32)
char	*optarg;		// global argument pointer
kuInt32	optind = 0; 	// global argv index
kuInt32	opterr = 0;		// for comportability with linux
kuInt32	optopt = 0;		// for comportability with linux

kuInt32 getopt (kuInt32 argc, char *argv[], char *optstring)
{
	static char *next = NULL;
	if (optind == 0) {
		next = NULL;
	}

	optarg = NULL;

	if (next == NULL || *next == '\0') {
		if (optind == 0)
			optind++;

		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0') {
			optarg = NULL;
			if (optind < argc) {
				optarg = argv[optind];
			}
			return EOF;
		}

		if (strcmp(argv[optind], "--") == 0) {
			optind++;
			optarg = NULL;
			if (optind < argc) {
				optarg = argv[optind];
			}
			return EOF;
		}

		next = argv[optind];
		next++;		// skip past -
		optind++;
	}

	char c = *next++;
	char *cp = strchr(optstring, c);

	if (cp == NULL || c == ':') {
		return '?';
	}

	cp++;
	if (*cp == ':') {
		if (*next != '\0') {
			optarg = next;
			next = NULL;
		}
		else if (optind < argc) {
			optarg = argv[optind];
			optind++;
		}
		else {
			return ('?');
		}
	}

	return c;
}
#endif

const kuInt32 kuStd::cpdir (const char *src, const char *dest)
{
	if ( (NULL == src) || (NULL == dest) ) {
		return (kuNOK);
	}

#if !defined(WIN32)
	char	cmd[256];

	sprintf (cmd, "\\cp -rf %s/* %s", src, dest);
	system (cmd);
#else
	HANDLE				hSrch;
	WIN32_FIND_DATAA	wfd;
	BOOL				bResult	= TRUE;

	char			srcFile [128];
	char			destFile[128];
	char			wildCard[128];

	sprintf (wildCard, "%s\\*.*", src);
	CreateDirectoryA (dest, NULL);

	if (INVALID_HANDLE_VALUE == (hSrch = FindFirstFileA (wildCard, &wfd))) {
		return (kuNOK);
	}

	while (bResult) {
		sprintf (srcFile , "%s\\%s", src , wfd.cFileName);
		sprintf (destFile, "%s\\%s", dest, wfd.cFileName);

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(wfd.cFileName, ".") && strcmp(wfd.cFileName, "..")) {
				kuStd::cpdir (srcFile, destFile);
			}
		}
		else {
			CopyFileA (srcFile, destFile, FALSE);
		}
	
		bResult = FindNextFileA (hSrch, &wfd);
	}
	
	FindClose (hSrch);
#endif

	return (kuOK);
}

const kuInt32 kuStd::deldir (const char *path)
{
	if (NULL == path) {
		return (kuNOK);
	}

#if !defined(WIN32)
	char	cmd[256];

	sprintf (cmd, "\\rm -rf %s", path);
	system (cmd);
#else
	HANDLE				hSrch;
	WIN32_FIND_DATAA	wfd;
	BOOL				bResult	= TRUE;

	char			delPath [128];
	char			fullPath[128];
	char			drive   [_MAX_DRIVE];
	char			dir     [128];

	strcpy (delPath, path);
	
	if (strcmp (delPath + strlen(delPath)-4, "\\*.*") != 0) {
		strcat (delPath, "\\*.*");
	}

	if (INVALID_HANDLE_VALUE == (hSrch = FindFirstFileA (delPath, &wfd))) {
		return (kuNOK);
	}

	_splitpath (delPath, drive, dir, NULL, NULL);

	while (bResult) {
		sprintf (fullPath, "%s%s%s", drive, dir, wfd.cFileName);

		// Ð±  Ó¼  ,  Ó¼  Ñ´.
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
			SetFileAttributesA (fullPath, FILE_ATTRIBUTE_NORMAL);
		}

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(wfd.cFileName, ".") && strcmp(wfd.cFileName, "..")) {
				kuStd::deldir (fullPath);
			}
		}
		else {
			DeleteFileA (fullPath);
		}
	
		bResult = FindNextFileA (hSrch, &wfd);
	}
	
	FindClose (hSrch);

	//  Ú± Ú½ Ñ´.
	sprintf (fullPath, "%s%s", drive, dir);
	fullPath[strlen(fullPath)-1] = 0x00;

	if (RemoveDirectoryA (fullPath) == FALSE) {
	}
#endif

	return (kuOK);
}

kuInt32 kuStd::scandir(const char *dir, struct dirent ***namelist, const char *compar)
{	
	kuInt32	nListSize =0;

#if defined(WIN32)
	char dir2[258];
	// À©µµ¿ìÀÏ °æ¿ì '/*'À» ºÙ¿©¾ß µÈ´Ù.
	sprintf(dir2,"%s/*",dir);	
	
	WIN32_FIND_DATA find_data;
	HANDLE hFind;
	wcout << "Read files from: "<< dir <<endl;

	//count files
	hFind = FindFirstFile(dir2, &find_data); // Find the first file and return a handle
	if(hFind == INVALID_HANDLE_VALUE) {	    
		return kuNOK;
	}
	
	do {
		nListSize++;
	} while (FindNextFile(hFind, &find_data)); // Get the next file

	FindClose(hFind); // Close the file handle

	//read file names
	hFind = FindFirstFile(dir2, &find_data); // Find the first file and return a handle
	
	if(hFind == INVALID_HANDLE_VALUE) {
	    // Error - no files
	    return kuNOK;
	}
	
	char buf[255];
	kuInt32 i=0;

	dirent **names ;
	names = (dirent **)malloc(sizeof(dirent*)*nListSize);//dirent*[nListSize];

	do {		
		names[i] =(dirent *)malloc(sizeof(dirent));//new dirent;
		sprintf(names[i]->d_name, find_data.cFileName);
		//cout << "Found: " << names[i]->d_name << "\n";
		i++;
	} while (FindNextFile(hFind, &find_data)); // Get the next file

	*namelist = names;
	FindClose(hFind); // Close the file handle

#else
	if( (NULL == compar) || (strcmp("alphasort",compar)==0) )
		nListSize = ::scandir (dir, namelist, NULL, alphasort);
	else 
		printf ("Unsupported sort function\n");

#endif

	return nListSize;
}

// End of File
