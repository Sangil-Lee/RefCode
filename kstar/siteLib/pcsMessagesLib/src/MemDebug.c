/* @(#)MemDebug.c	1.25 03/07/07
******************************************************************************
File: MemDebug.c

Wrapper routines to malloc, calloc, realloc, and free.
These routines can check for memory corruption when memory is freed,
write diagnostic messages about malloc's of 0 bytes, and
give diagnostic messages to help track down memory problems.

By default, these routines fall through to the system functions.
To activate use of the memory checking set the environmental
variable MEMDEBUG to the value 1.  To print set memory checking
and print diagnostics set the value to 2.  Any other value will
deactivate both memory checking and diagnostic printing.
******************************************************************************
*/

#if defined(SOLARIS_1) || defined(SOLARIS_2) || defined(LINUXOS)
#include <time.h>
#include <sys/param.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#ifdef SUN
#include    <memory.h>      /* for memset proto */
#endif

/*
Define whether to check for memory corruption by putting
sizes and line numbers into each memory allocation.
This works for Solaris, but not for linux.
*/
#if defined(SOLARIS_1) || defined(SOLARIS_2)
#define CHECK_MEMORY 1
#else
#define CHECK_MEMORY 0
#endif

#include "MemDebug.h"

/**************************************************************************/
/*************************** Preprocessor stuff ***************************/
/**************************************************************************/

#undef malloc
#undef calloc
#undef realloc
#undef free

#define header_size 32
#define trailer_size 16
static FILE *p_TargetFile;
static int PRINTIT = -1;
static int CALLED = 0;
static unsigned int total_malloced;
static int file_open = 0;
static int keep_open = 0;
/*
fopen and fprintf malloc memory so use pthread_mutex_lock
before the malloc, calloc, etc. are done to avoid problems.
Variables used in pthread_mutex routines.
*/
static pthread_mutex_t mp;
static int ierr;

static void memdebug_init ()
{
char *temp;

    PRINTIT = 0; /* memdebug turned off */
    temp = (char *)getenv("MEMDEBUG");
    if (temp != NULL) PRINTIT = (int) atoi(temp);
    total_malloced = 0;
#ifdef HP_OLD
    ierr = pthread_mutex_init(&mp, pthread_mutexattr_default);
#else
    ierr = pthread_mutex_init(&mp, NULL);
#endif
}

static void open_file()
{
    if(file_open) return;
    p_TargetFile = fopen("memdebug.out","a");
    if (p_TargetFile == NULL)
    {
        perror("open_file");
        p_TargetFile = stderr;
    }
    file_open = 1;
}

static void close_file()
{
    if(!keep_open) fclose(p_TargetFile);
    if(!keep_open) file_open = 0;
}

static void print_Location (char *p_FileName, long p_LineNumber)
{
time_t now;
struct tm *current;
char time_str[20];

    now     = time(NULL);
    current = localtime(&now);
    sprintf(time_str, "%04d%02d%02d %02d:%02d:%02d", 
            current->tm_year+1900,current->tm_mon+1,current->tm_mday,
            current->tm_hour,current->tm_min,current->tm_sec);
    time_str[17] = '\0';
    open_file();
    fprintf(p_TargetFile,"%s %-15s Line %5lu # ",
        time_str,p_FileName,p_LineNumber);
    fflush(p_TargetFile);
}

/**************************************************************************/
/**************************** Global functions ****************************/
/**************************************************************************/

void set_memdebug (int flag, int *old_flag)
{
    if (PRINTIT == -1) memdebug_init();
    if (old_flag != NULL) *old_flag = PRINTIT;
    PRINTIT = flag;
}

void *__check_malloc(size_t p_Size, char *p_FileName, long p_LineNumber)
/*
    Replaces standard malloc()
*/

{
void *l_NewBlock;
int size;
char *ptr;

    if (PRINTIT == -1) memdebug_init();
    if (PRINTIT == 0) return(malloc(p_Size));
    if (p_Size == 0)
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Malloc used with block size 0.\n");
        close_file();
        return (NULL);
    }

    if (PRINTIT == 2)
    {
        ierr = pthread_mutex_lock(&mp);
        if (ierr) fprintf(stderr,"ERROR %d in pthread_mutex_lock\n", errno);
        if (ierr) abort();
    }

#if CHECK_MEMORY
    if (CALLED != 0) 
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Malloc called while in malloc\n");
        close_file();
    }
#endif

    CALLED++;

    errno = 0;
#if CHECK_MEMORY
    size = p_Size + header_size + trailer_size;
#else
    size = p_Size;
#endif
    ptr = (char *)malloc (size);
    if (ptr)
    {
        memset(&ptr[0], 0, size);
#if CHECK_MEMORY
        memcpy(&ptr[0], (char*)&size, (sizeof(int)));
        memcpy(&ptr[sizeof(int)], (char*)&p_LineNumber, (sizeof(int)));
        strncpy(&ptr[2*sizeof(int)], p_FileName, 
                header_size - 2*sizeof(int) - 1);
        memset(&ptr[header_size+p_Size], 64, trailer_size);
        l_NewBlock = &ptr[header_size];
        total_malloced += size;
#else
        l_NewBlock = ptr;
#endif
        if (PRINTIT == 2)
        {
            print_Location(p_FileName,p_LineNumber);
            fprintf(p_TargetFile,"Malloc size %d, ptr=%p",
                    p_Size, l_NewBlock);
#if CHECK_MEMORY
            fprintf(p_TargetFile,", real size %d, total=%u\n", 
                    size, total_malloced);
#else
            fprintf(p_TargetFile,"\n");
#endif
        }
    }
    else
    {
        if(errno != 0)
        {
         char temp[256];
           sprintf(temp,"MemDebug: errno = %d from malloc",errno);
           perror(temp);
        }
        else
           perror("MemDebug: ptr = NULL from malloc");
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: ptr = NULL from malloc of size %d\n",
                p_Size);
        close_file();
        l_NewBlock = NULL;
        abort();
    }
    CALLED--;
    if (PRINTIT == 2)
    {
        ierr = pthread_mutex_unlock(&mp);
        if (ierr) fprintf(stderr,"ERROR %d in pthread_mutex_unlock\n", errno);
        if (ierr) abort();
    }
    return (l_NewBlock);
}

void *__check_calloc(size_t p_Number, size_t p_Size,
     char *p_FileName, long p_LineNumber)
/*
    Replaces standard calloc()
*/

{
void *l_NewBlock;
int size;
char *ptr;

    if (PRINTIT == -1) memdebug_init();
    if (PRINTIT == 0) return(calloc(p_Number,p_Size));
    if ((p_Number < 1) || (p_Size < 1))
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Calloc of %d * %d; returning\n",
                p_Number, p_Size);
        close_file();
        return (NULL);
    }

    if (PRINTIT == 2)
    {
        ierr = pthread_mutex_lock(&mp);
        if (ierr) fprintf(stderr,"ERROR %d in pthread_mutex_lock\n", errno);
        if (ierr) abort();
    }

#if CHECK_MEMORY
    if (CALLED != 0) 
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Calloc called while in malloc\n");
        close_file();
    }
#endif

    CALLED++;

    errno = 0;
#if CHECK_MEMORY
    size = p_Number * p_Size + header_size + trailer_size;
    ptr = (char *)malloc(size);
#else
    size = p_Size;
    ptr = (char *)calloc(p_Number,size);
#endif
    if (ptr)
    {
#if CHECK_MEMORY
        memset(&ptr[0], 0, size);
        memcpy(&ptr[0], (char*)&size, (sizeof(int)));
        memcpy(&ptr[sizeof(int)], (char*)&p_LineNumber, (sizeof(int)));
        strncpy(&ptr[2*sizeof(int)], p_FileName, 
                header_size - 2*sizeof(int) - 1);
        memset(&ptr[header_size + p_Number * p_Size], 64, trailer_size);
        l_NewBlock = &ptr[header_size];
        total_malloced += size;
#else
        l_NewBlock = ptr;
#endif
        if (PRINTIT == 2)
        {
            print_Location(p_FileName,p_LineNumber);
            fprintf(p_TargetFile,"Calloc size %d * %d, ptr=%p, total=%u\n",
                    p_Number, p_Size, l_NewBlock, total_malloced);
            close_file();
        }
    }
    else
    {
        if(errno != 0)
        {
         char temp[256];
           sprintf(temp,"MemDebug: errno = %d from calloc",errno);
           perror(temp);
        }
        else
           perror("MemDebug: ptr = NULL from calloc");
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: ptr = NULL from calloc of %d * %d\n",
                p_Number, p_Size);
        close_file();
        l_NewBlock = NULL;
        abort();
    }
    CALLED--;
    if (PRINTIT == 2)
    {
        ierr = pthread_mutex_unlock(&mp);
        if (ierr) fprintf(stderr,"ERROR %d in pthread_mutex_unlock\n", errno);
        if (ierr) abort();
    }
    return (l_NewBlock);
}

void *__check_realloc(void *p_Pointer, size_t p_Size,
            char *p_FileName, long p_LineNumber)
/*
    Replaces standard realloc()
*/

{
void *l_NewPointer;
int size;
char *ptr;
    
    if (PRINTIT == -1) memdebug_init();
    if (PRINTIT == 0) return(realloc(p_Pointer,p_Size));
    if (p_Pointer == NULL)
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Realloc used with NULL pointer\n");
        close_file();
        return (NULL);
    }
    
    if (p_Size == 0)
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Realloc used with new block size 0\n");
        close_file();
        return (NULL);
    }
    
    if (PRINTIT == 2)
    {
        ierr = pthread_mutex_lock(&mp);
        if (ierr) fprintf(stderr,"ERROR %d in pthread_mutex_lock\n", errno);
        if (ierr) abort();
    }

#if CHECK_MEMORY
    if (CALLED != 0) 
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Realloc called while in malloc\n");
        close_file();
    }
#endif

    CALLED++;

    errno = 0;
#if CHECK_MEMORY
    p_Pointer = ((char*)p_Pointer) - header_size;
    size = p_Size + header_size + trailer_size;
    total_malloced -= ((int*)p_Pointer)[0];
#else
    size = p_Size;
#endif
    ptr = (char *)realloc(p_Pointer,size);
    if (ptr)
    {
#if CHECK_MEMORY
        memset(&ptr[0], 0, header_size);
        memcpy(&ptr[0], (char*)&size, (sizeof(int)));
        memcpy(&ptr[sizeof(int)], (char*)&p_LineNumber, (sizeof(int)));
        strncpy(&ptr[2*sizeof(int)], p_FileName, 
                header_size - 2*sizeof(int) - 1);
        memset(&ptr[header_size+p_Size], 64, trailer_size);
        l_NewPointer = &ptr[header_size];
        total_malloced += size;
#else
        l_NewPointer = ptr;
#endif
        if (PRINTIT == 2)
        {
            print_Location(p_FileName,p_LineNumber);
            fprintf(p_TargetFile,"Realloc of %p, new ptr=%p, size=%d, total=%u\n",
                    p_Pointer, l_NewPointer, p_Size, total_malloced);
            close_file();
        }
    }
    else
    {
        if(errno != 0)
        {
         char temp[256];
           sprintf(temp,"MemDebug: errno = %d from realloc",errno);
           perror(temp);
        }
        else
           perror("MemDebug: ptr = NULL from realloc");
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: ptr = NULL from realloc of size %d\n",
                p_Size);
        close_file();
        l_NewPointer = NULL;
        abort();
    }
    CALLED--;
    if (PRINTIT == 2)
    {
        ierr = pthread_mutex_unlock(&mp);
        if (ierr) fprintf(stderr,"ERROR %d in pthread_mutex_unlock\n", errno);
        if (ierr) abort();
    }
    return (l_NewPointer);
}

void __check_free(void *p_Pointer, char *p_FileName, long p_LineNumber)
                        
/*
    Replaces standard free()
*/

{
#if CHECK_MEMORY
int size, linenum, i, *iptr;
#endif
char *ptr;

    if (PRINTIT == 0)
    {
        free(p_Pointer);
        return;
    }
    if (PRINTIT == 2)
    {
        ierr = pthread_mutex_lock(&mp);
        if (ierr) fprintf(stderr,"ERROR %d in pthread_mutex_lock\n", errno);
        if (ierr) abort();
    }

#if CHECK_MEMORY
    if (CALLED != 0) 
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Free called while in malloc\n");
        close_file();
    }
#endif

    CALLED++;

    if (PRINTIT == -1) memdebug_init();
    if (p_Pointer != NULL)
    {
#if CHECK_MEMORY
        ptr =  (char*)p_Pointer - header_size;	/* start of actual block */
        iptr = (int*)ptr;
        size = iptr[0];
        total_malloced -= size;
        for (i=trailer_size-1; i<0; i--)
        {
            if (ptr[size-trailer_size+i] != 64)
            {
                print_Location(p_FileName,p_LineNumber);
                fprintf(p_TargetFile,"freeing block at %p "
                        "of size %d gotten at\n", ptr, size);
                close_file();
                linenum = iptr[1];
                print_Location(&ptr[2*sizeof(int)],(long)linenum);
                fprintf(p_TargetFile,"MemDebug: Block overrun of %d bytes\n", i);
                close_file();
                break;
            }
        }
/*
        memset(ptr, 0, size);
*/
#else
        ptr =  (char *)p_Pointer;
#endif
        free(ptr);
        if (PRINTIT == 2)
        {
            print_Location(p_FileName,p_LineNumber);
#if CHECK_MEMORY
            fprintf(p_TargetFile,"Free size %d, ptr=%p, total=%u\n",
                    size, p_Pointer, total_malloced);
#else
            fprintf(p_TargetFile,"Free ptr=%p\n", p_Pointer);
#endif
            close_file();
        }
    }
    else
    {
        print_Location(p_FileName,p_LineNumber);
        fprintf(p_TargetFile,"MemDebug: Freeing NULL block\n");
        close_file();
    }
    CALLED--;
    if (PRINTIT == 2)
    {
        ierr = pthread_mutex_unlock(&mp);
        if (ierr) fprintf(stderr,"ERROR %d in pthread_mutex_unlock\n", errno);
        if (ierr) abort();
    }
    return;
}
#endif /* #if defined(SOLARIS_1) || defined(SOLARIS_2) || defined(LINUXOS) */
