/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/lib/pxi-6259-lib-utils.c $
 * $Id: pxi-6259-lib-utils.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Utility functions definitions.
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include <pxi6259.h>
#include <pxi-6259-lib-utils.h>


int cb_init(circular_buffer_t *cb, size_t capacity, size_t sz)
{
        if (cb == NULL || capacity <= 0 || sz <= 0)
                return -1;

        cb->buffer = malloc(capacity * sz);
        if(cb->buffer == NULL)
                return -1;

        cb->buffer_end = (char *)cb->buffer + capacity * sz;
        cb->capacity = capacity;
        cb->count = 0;
        cb->sz = sz;
        cb->head = cb->buffer;
        cb->tail = cb->buffer;

        pthread_mutex_init(&cb->mutex, NULL);

        cb->missed = 0;

        return 0;
}

int cb_free(circular_buffer_t *cb)
{
        if (cb == NULL)
                return -1;

        if (cb->buffer != NULL) {
                free(cb->buffer);
        }

        pthread_mutex_destroy(&cb->mutex);

        return 0;
}

/**
 * TODO too much error checking
 */
int cb_push_back(circular_buffer_t *cb, const void *item)
{
        if (cb == NULL || item == NULL)
                return -1;

        pthread_mutex_lock(&cb->mutex);

        if (cb->buffer == NULL || cb->buffer_end == NULL) {
                pthread_mutex_unlock(&cb->mutex);
                return -1;
        }

        /* Copy element into circular buffer */
        memcpy(cb->head, item, cb->sz);
        cb->head = (char*)cb->head + cb->sz;
        if (cb->head == cb->buffer_end)
                cb->head = cb->buffer;

        if (cb->count < cb->capacity) {
                cb->count++;
        } else {
                cb->missed++;
                cb->tail = (char *) cb->tail + cb->sz;
                if (cb->tail == cb->buffer_end)
                        cb->tail = cb->buffer;
        }

        pthread_mutex_unlock(&cb->mutex);

        return 0;
}

int cb_push_back_rp(circular_buffer_t *cb, const void *item, unsigned int nr)
{
        int i, retval;
        char *ptr = (char *) item;

        for (i=0; i<nr; i++) {
                retval = cb_push_back(cb, ptr + i * cb->sz);
                if (retval) return -1;
        }

        return 0;
}

/**
 * TODO
 *      - too much error checking (can be faster)
 *      - read multiple items
 */
int cb_pop_front(circular_buffer_t *cb, void *item)
{
        unsigned long tmp;
        if (cb == NULL || item == NULL)
                return -1;

        pthread_mutex_lock(&cb->mutex);

        if (cb->buffer == NULL || cb->buffer_end == NULL || cb->count == 0) {
                pthread_mutex_unlock(&cb->mutex);
                return -1;
        }

        memcpy(item, cb->tail, cb->sz);
        cb->tail = (char*)cb->tail + cb->sz;
        if(cb->tail == cb->buffer_end)
                cb->tail = cb->buffer;
        cb->count--;

        tmp = cb->missed;

        pthread_mutex_unlock(&cb->mutex);

        return tmp;
}

int cb_get_overflow(circular_buffer_t *cb, unsigned long *missed)
{
        if (cb == NULL || missed == NULL)
                return -1;

        pthread_mutex_lock(&cb->mutex);

        *missed = cb->missed;
        cb->missed = 0;

        pthread_mutex_unlock(&cb->mutex);

        return 0;
}


/******************************************************************/
/* THE FOLLOWING FUNCTIONS CAN BE USED FOR TESTING PURPOSES ONLY! */
/******************************************************************/

int write_register_8(int fd, uint8_t bar, uint32_t offset, uint8_t val)
{
        struct pxi6259_raw_io_block raw_block;

        if(!fd)
                return -PXI6259_ERROR_DEVICE_NOT_INITIALIZED;

        raw_block.bar = bar;
        raw_block.offset = offset;
        raw_block.dw = PXI6259_DW08;
        raw_block.data = &val;

        return ioctl(fd, PXI6259_IOC_RAW_WRITE, &raw_block);
}

int write_register_16(int fd, uint8_t bar, uint32_t offset, uint16_t val)
{
        struct pxi6259_raw_io_block raw_block;

        if(!fd)
                return -PXI6259_ERROR_DEVICE_NOT_INITIALIZED;

        raw_block.bar = bar;
        raw_block.offset = offset;
        raw_block.dw = PXI6259_DW16;
        raw_block.data = &val;

        ioctl(fd, PXI6259_IOC_RAW_WRITE, &raw_block);
        return 0;
}

int write_register_32(int fd, uint8_t bar, uint32_t offset, uint32_t val)
{
        struct pxi6259_raw_io_block raw_block;

        if(!fd)
                return -PXI6259_ERROR_DEVICE_NOT_INITIALIZED;

        raw_block.bar = bar;
        raw_block.offset = offset;
        raw_block.dw = PXI6259_DW32;
        raw_block.data = &val;

        return ioctl(fd, PXI6259_IOC_RAW_WRITE, &raw_block);
}


int write_register_32_rp(int fd, uint8_t bar, uint32_t *offset, uint32_t *val,
                uint32_t buffer_size)
{
        struct pxi6259_raw_io_block raw_block;
        int retval, i;

        if(!fd)
                return -PXI6259_ERROR_DEVICE_NOT_INITIALIZED;

        raw_block.bar = bar;
        raw_block.dw = PXI6259_DW32;

        for (i = 0; i<buffer_size; i++) {
                raw_block.offset = offset[i];
                raw_block.data = &val[i];

                retval = ioctl(fd, PXI6259_IOC_RAW_WRITE, &raw_block);
                if(retval)
                        return retval;
        }

        return 0;
}

int read_register_8(int fd, uint8_t bar, uint32_t offset, uint8_t *val)
{
        struct pxi6259_raw_io_block raw_block;
        int retval;

        if(!fd)
                return -PXI6259_ERROR_DEVICE_NOT_INITIALIZED;

        raw_block.bar = bar;
        raw_block.offset = offset;
        raw_block.dw = PXI6259_DW08;
        raw_block.data = val;

        retval = ioctl(fd, PXI6259_IOC_RAW_READ, &raw_block);

        return retval;
}

int read_register_16(int fd, uint8_t bar, uint32_t offset, uint16_t *val)
{
        struct pxi6259_raw_io_block raw_block;
        int retval;

        if(!fd)
        	return -PXI6259_ERROR_DEVICE_NOT_INITIALIZED;

        raw_block.bar = bar;
        raw_block.offset = offset;
        raw_block.dw = PXI6259_DW16;
        raw_block.data = val;

        retval = ioctl(fd, PXI6259_IOC_RAW_READ, &raw_block);
        return retval;
}

int read_register_32(int fd, uint8_t bar, uint32_t offset, uint32_t *val)
{
        struct pxi6259_raw_io_block raw_block;
        int retval;

        if(!fd) {
                return -PXI6259_ERROR_DEVICE_NOT_INITIALIZED;
        }

        raw_block.bar = bar;
        raw_block.offset = offset;
        raw_block.dw = PXI6259_DW32;
        raw_block.data = val;

        retval = ioctl(fd, PXI6259_IOC_RAW_READ, &raw_block);

       	return retval;
}

int read_register_32_rp(int fd, uint8_t bar, uint32_t *offset, uint32_t *val,
                uint32_t buffer_size)
{
        struct pxi6259_raw_io_block raw_block;
        int retval, i;

        if(!fd)
                return -PXI6259_ERROR_DEVICE_NOT_INITIALIZED;

        raw_block.bar = bar;
        raw_block.dw = PXI6259_DW32;

        for (i = 0; i<buffer_size; i++) {
                raw_block.offset = offset[i];
                raw_block.data = &val[i];

                retval = ioctl(fd, PXI6259_IOC_RAW_READ, &raw_block);
                if(retval)
                        return retval;
        }

        return 0;
}

