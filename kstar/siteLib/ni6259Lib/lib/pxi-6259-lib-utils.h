/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/lib/pxi-6259-lib-utils.h $
 * $Id: pxi-6259-lib-utils.h 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Utility header file.
 *
 * Author        : Janez Golob (Cosylab)
 *
 * Copyright (c) : 2010-2013 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ************************************************************************/

#ifndef PXI6259UTILS_H_
#define PXI6259UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>

#ifndef BAR0
# define BAR0    0
#endif

#ifndef BAR1
# define BAR1    1
#endif


/*Circular buffer definition */
typedef struct circular_buffer
{
    void *buffer;               /* data buffer */
    void *buffer_end;           /* end of data buffer */
    size_t capacity;            /* maximum number of items in the buffer */
    size_t count;               /* number of items in the buffer */
    size_t sz;                  /* size of each item in the buffer */
    void *head;                 /* pointer to head */
    void *tail;                 /* pointer to tail */
    unsigned long missed;       /* missed reads */
    pthread_mutex_t mutex;      /* mutex for locking */
} circular_buffer_t;


/**
 * Initialize circular buffer.
 *
 * This function initializes the circular buffer. The buffer of a capacity * sz
 * size is allocated and the internal variables are reset to a known initial state.
 *
 * @param cb            Pointer to the circular buffer structure.
 * @param capacity      Number of the elements in the circular buffer.
 * @param sz            The size of the elements in the circular buffer.
 *
 * @return              0 if function succeeds otherwise -1.
 */
int cb_init(circular_buffer_t *cb, size_t capacity, size_t sz);

/**
 * Free circular buffer.
 *
 * This function deallocate resources needed by the circular buffer.
 *
 * @param cb            Pointer to the circular buffer structure.
 *
 * @return              0 if function succeeds otherwise -1.
 */
int cb_free(circular_buffer_t *cb);

/**
 * Copy element into circular buffer.
 *
 * This function push back an element into the circular buffer. The element is
 * copied so the item argument can be reused after invoking this function.
 *
 * @param cb            Pointer to the circular buffer structure.
 * @param item          Pointer to the element.
 *
 * @return              0 if function succeeds otherwise -1.
 */
int cb_push_back(circular_buffer_t *cb, const void *item);

/**
 * Copy array of the elements into circular buffer.
 *
 * This function push back an array of the elements into the circular buffer.
 * The elements are copied so the item argument can be reused after invoking
 * this function.
 *
 * @param cb            Pointer to the circular buffer structure.
 * @param item          Pointer to the array of the elements.
 * @param nr            Number of the elements in the array.
 *
 * @return              0 if function succeeds otherwise -1.
 */
int cb_push_back_rp(circular_buffer_t *cb, const void *item, unsigned int nr);

/**
 * Copy element from the circular buffer.
 *
 * This function pop front an element from the circular buffer.
 *
 * @param cb            Pointer to the circular buffer structure.
 * @param item          Pointer to the element.
 *
 * @return              -1 if there is no element in the buffer or something goes
 *                      wrong otherwise number of missed reads.
 */
int cb_pop_front(circular_buffer_t *cb, void *item);


/**
 * Get and reset overflow.
 *
 * This function gets overflow and resets the overflow variable to 0;
 *
 * @param cb            Circular buffer to be used.
 * @param missed        Returns number of the missed packages.
 *
 * @return              0 if function succeeds otherwise -1.
 */
int cb_get_overflow(circular_buffer_t *cb, unsigned long *missed);



/******************************************************************/
/* THE FOLLOWING FUNCTIONS CAN BE USED FOR TESTING PURPOSES ONLY! */
/******************************************************************/

/**
 * Write 8 bit value to an offsettable iomem register.
 *
 * @param fd            File descriptor to be used to do the ioctl call.
 * @param bar           PCI bar selection if any.
 * @param offset        Register offset.
 * @param val           Value.
 *
 * @return              0 if function succeeds otherwise error code.
 */
int write_register_8(
                int fd,
                uint8_t bar,
                uint32_t offset,
                uint8_t val);

/**
 * Write 16 bit value to an offsettable iomem register.
 *
 * @param fd            File descriptor to be used to do the ioctl call.
 * @param bar           PCI bar selection if any.
 * @param offset        Register offset.
 * @param val           Value.
 *
 * @return              0 if function succeeds otherwise error code.
 */
int write_register_16(
                int fd,
                uint8_t bar,
                uint32_t offset,
                uint16_t val);

/**
 * Write 32 bit value to an offsettable iomem register.
 *
 * @param fd            File descriptor to be used to do the ioctl call.
 * @param bar           PCI bar selection if any.
 * @param offset        Register offset.
 * @param val           Value.
 *
 * @return              0 if function succeeds otherwise error code.
 */
int write_register_32(
                int fd,
                uint8_t bar,
                uint32_t offset,
                uint32_t val);

int write_register_32_rp(
                int fd,
                uint8_t bar,
                uint32_t *offset,
                uint32_t *val,
                uint32_t buffer_size);

/**
 * Read 8 bit value from an offsettable iomem register.
 *
 * @param fd            File descriptor to be used to do the ioctl call.
 * @param bar           PCI bar selection if any.
 * @param offset        Register offset.
 * @param val           Value.
 *
 * @return              0 if function succeeds otherwise error code.
 */
int read_register_8(
                int fd,
                uint8_t bar,
                uint32_t offset,
                uint8_t *val);

/**
 * Read 16 bit value from an offsettable iomem register.
 *
 * @param fd            File descriptor to be used to do the ioctl call.
 * @param bar           PCI bar selection if any.
 * @param offset        Register offset.
 * @param val           Value.
 *
 * @return              0 if function succeeds otherwise error code.
 */
int read_register_16(
                int fd,
                uint8_t bar,
                uint32_t offset,
                uint16_t *val);

/**
 * Read 32 bit value from an offsettable iomem register.
 *
 * @param fd            File descriptor to be used to do the ioctl call.
 * @param bar           PCI bar selection if any.
 * @param offset        Register offset.
 * @param val           Value.
 *
 * @return              0 if function succeeds otherwise error code.
 */
int read_register_32(
                int fd,
                uint8_t bar,
                uint32_t offset,
                uint32_t *val);

int read_register_32_rp(
                int fd,
                uint8_t bar,
                uint32_t *offset,
                uint32_t *val,
                uint32_t buffer_size);

#ifdef __cplusplus
} /* closing brace for "C" */
#endif

#endif /* PXI6259UTILS_H_ */
