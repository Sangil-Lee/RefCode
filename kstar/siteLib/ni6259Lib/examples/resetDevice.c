/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/resetDevice.c $
 * $Id: resetDevice.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Reset PXI 6259 device.
 *
 * Author        : Klemen Vodopivec (Cosylab d.d)
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
 * @example resetDevice.c
 *
 * Reset PXI 6259 device to initial state.
 */

#include <pxi-6259-lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_FILE "/dev/pxi6259"

void usage(char *name) {
        printf("Reset PXI 6259 device to initial state.\n"
               "\n"
               "Usage: %s <device number>\n"
               "Example: %s 0\n", name, name);
}

int main(int argc, char** argv) {
        uint32_t devnum;
        int dev;
        char *endptr;
        char filename[256];

        if (argc < 2) {
                usage(argv[0]);
                return 1;
        }

        // parse command line parameters
        devnum = strtoul(argv[1], &endptr, 10);
        if (devnum == 0 && endptr == argv[1]) {
                fprintf(stderr, "Wrong device id, numerical values only!\n");
                return 1;
        }

        // open device file descriptor
        sprintf(filename, "%s.%u", DEVICE_FILE, devnum);
        dev = open(filename, O_RDWR);
        if (dev < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return 2;
        }

        // reset device
        if (pxi6259_reset(dev)) {
                fprintf(stderr, "Failed to reset device: %s\n", strerror(errno));
                return 3;
        }

        // close device
        if (close(dev)) {
                fprintf(stderr, "Failed to close device: %s\n", strerror(errno));
                return 2;
        }

        return 0;
}
