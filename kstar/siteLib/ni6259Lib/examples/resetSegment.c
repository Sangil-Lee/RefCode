/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/resetSegment.c $
 * $Id: resetSegment.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Reset PXI 6259 device.
 *
 * Author        : Tom Slejko (Cosylab d.d)
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

/**
 * @example resetSegment.c
 *
 * Reset PXI 6259 device segment to initial state.
 */

#include <pxi-6259-lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <test_common.h>

#define DEVICE_FILE "/dev/pxi6259"

void usage(char *name) {
    printf("Reset segment of PXI 6259 device to initial state.\n"
            "\n"
            "Usage: %s <device number> <segment>\n"
            "Example: %s 0 AI\n"
            "Available segments: AI AO DIO GPC\n", name, name);
}

int main(int argc, char** argv) {
    uint32_t devnum;
    int dev, reset=0;
    char *endptr;
    char filename[256];

    if (argc < 3) {
        usage(argv[0]);
        return TEST_INVALID_INPUT;
    }

    // parse command line parameters
    devnum = strtoul(argv[1], &endptr, 10);
    if (devnum == 0 && endptr == argv[1]) {
        fprintf(stderr, "Wrong device id, numerical values only!\n");
        return TEST_INVALID_INPUT;
    }

    /**** AI segment reset ****/
    if (!strcmp(argv[2], "AI")) {
        // open device file descriptor
        sprintf(filename, "%s.%u.ai", DEVICE_FILE, devnum);
        dev = open(filename, O_RDWR);
        if (dev < 0) {
            fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
            return TEST_DEVICE_FILES_PROBLEM;
        }

        // reset device
        if (pxi6259_reset_ai(dev)) {
            fprintf(stderr, "Failed to reset device: %s\n", strerror(errno));
            return TEST_RUNTIME_ERROR;
        }
        reset=1;
    }

    /**** AO segment reset ****/
    if (!strcmp(argv[2], "AO")) {
        // open device file descriptor
        sprintf(filename, "%s.%u.ao", DEVICE_FILE, devnum);
        dev = open(filename, O_RDWR);
        if (dev < 0) {
            fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
            return TEST_DEVICE_FILES_PROBLEM;
        }

        // reset device
        if (pxi6259_reset_ao(dev)) {
            fprintf(stderr, "Failed to reset device: %s\n", strerror(errno));
            return TEST_RUNTIME_ERROR;
        }
        reset=1;
    }

    /**** DIO segment reset ****/
    if (!strcmp(argv[2], "DIO")) {
        // open device file descriptor
        sprintf(filename, "%s.%u.dio", DEVICE_FILE, devnum);
        dev = open(filename, O_RDWR);
        if (dev < 0) {
            fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
            return TEST_DEVICE_FILES_PROBLEM;
        }

        // reset device
        if (pxi6259_reset_dio(dev)) {
            fprintf(stderr, "Failed to reset device: %s\n", strerror(errno));
            return TEST_RUNTIME_ERROR;
        }
        reset=1;
    }

    /**** GPC segment reset ****/
    if (!strcmp(argv[2], "GPC")) {
        // open device file descriptor
        sprintf(filename, "%s.%u.gpc", DEVICE_FILE, devnum);
        dev = open(filename, O_RDWR);
        if (dev < 0) {
            fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
            return TEST_DEVICE_FILES_PROBLEM;
        }

        // reset device
        if (pxi6259_reset_gpc(dev)) {
            fprintf(stderr, "Failed to reset device: %s\n", strerror(errno));
            return TEST_RUNTIME_ERROR;
        }
        reset=1;
    }

    if (reset==0){
            fprintf(stderr, "Invalid device segment %s specified.\n", argv[2]);
            return TEST_INVALID_INPUT;
    }

    // close device
    if (close(dev)) {
        fprintf(stderr, "Failed to close device: %s\n", strerror(errno));
        return TEST_DEVICE_FILES_PROBLEM;
    }

    printf("Segment reset SUCCEEDED\n");

    return TEST_SUCCESS;
}
