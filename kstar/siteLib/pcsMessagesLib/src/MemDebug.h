/* @(#)MemDebug.h	1.10 03/07/07
Taken from original MemDebug.h which did much more than we need.
*/

#if defined(SOLARIS_1) || defined(SOLARIS_2) || defined(LINUXOS)
#ifndef __memdebug__
#define __memdebug__

#include <stdlib.h>

extern void set_memdebug (int flag, int *old_flag);

#define malloc(p_Size) \
    __check_malloc  (p_Size, __FILE__,__LINE__)
#define calloc(p_Number,p_Size) \
    __check_calloc (p_Number, p_Size, __FILE__, __LINE__)
#define realloc(p_Name,p_Size) \
    __check_realloc (p_Name, p_Size, __FILE__, __LINE__)
#define free(p_Name) \
   __check_free (p_Name, __FILE__, __LINE__)

/*************************************************************************/
/************************* Function Prototypes ***************************/
/*************************************************************************/

void  * __check_malloc    ( size_t p_Size,
                            char   * p_FileName,
                            long   p_LineNumber);
                        
void  * __check_calloc    ( size_t p_Number,
                            size_t p_Size,
                            char   * p_FileName,
                            long   p_LineNumber);
                        
void  * __check_realloc   ( void   * p_Pointer,
                            size_t p_Size,
                            char   * p_FileName,
                            long   p_LineNumber);
    
void    __check_free      ( void   * p_Pointer,
                            char   * p_FileName,
                            long   p_LineNumber);

#endif /* __memdebug__ */
#endif /* #if defined(SOLARIS_1) || defined(SOLARIS_2) || defined(LINUXOS) */
