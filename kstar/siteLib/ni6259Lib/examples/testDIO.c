/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/testDIO.c $
 * $Id: testDIO.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Test of digital inputs and outputs.
 *
 * Author        : Janez Golob (Cosylab d.d)
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
 * @example testDIO.c
 *
 * This example writes and reads from card's digital input/output ports.
 * All ports which are used are added to configuration. This enables the
 * ports and allows the generation of file descriptors. Each port has
 * its own IO mask, which marks the DIO pins as inputs or outputs (0
 * is input, 1 is output; all masks are set to 0 by default - all pins
 * are inputs). Values are written to DIO ports with system write call.
 * Value written to port, set as output, will be transfered to read
 * register. This means that the read register of DIO port will contain
 * masked value. Values are also read back and written to the console.
 */

#include <pxi-6259-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#define DEVICE_FILE "/dev/pxi6259"

void usage(char *name) {
        printf("This example writes and reads from card's digital input/output ports.\n"
                "All ports which are used are added to configuration. This enables the\n"
                "ports and allows the generation of file descriptors. Each port has\n"
                "its own IO mask, which marks the DIO pins as inputs or outputs (0\n"
                "is input, 1 is output; all masks are set to 0 by default - all pins\n"
                "are inputs). Values are written to DIO ports with system write call.\n"
                "Value written to port, set as output, will be transfered to read\n"
                "register. This means that the read register of DIO port will contain\n"
                "masked value. Values are also read back and written to the console.\n"
                "\n"
                "Usage: %s <device number>\n"
                "Example: %s 0\n", name, name);
}

int testDIO(uint32_t deviceNum) {
        pxi6259_dio_conf_t dioConfig;
        char filename[256];
        int devFD;

        /* Mask for port 0 is set to not interfere with terminals which are used with test stand. If
         * these bits are used, an error will most likely appear. To avoid errors, disconnect test stand
         * wires or use different values! */
        uint32_t portMask[PXI6259_MAX_DIO_PORT + 1] = { 0x12345070, 0x1, 0x0 };
        int portFDs[PXI6259_MAX_DIO_PORT + 1];

        uint32_t writeValue0 = 0xF38B7A39;
        uint8_t writeValue1 = 0x01;
        uint8_t writeValue2 = 0x00;

        uint32_t readValue0 = 0x1234;
        uint8_t readValue1 = 0x1234;
        uint8_t readValue2 = 0x1234;

        int i;

        // open DIO file descriptor
        sprintf(filename, "%s.%u.dio", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize DIO configuration
        dioConfig = pxi6259_create_dio_conf();

        // configure DIO ports
        if (pxi6259_add_dio_channel(&dioConfig, DIO_PORT0, portMask[0])) {
                fprintf(stderr, "Failed to configure DIO port 0!\n");
                return -1;
        }
        if (pxi6259_add_dio_channel(&dioConfig, DIO_PORT1, portMask[1])) {
                fprintf(stderr, "Failed to configure DIO port 1!\n");
                return -1;
        }
        if (pxi6259_add_dio_channel(&dioConfig, DIO_PORT2, portMask[2])) {
                fprintf(stderr, "Failed to configure DIO port 2!\n");
                return -1;
        }

        // load DIO configuration and let it apply
        if (pxi6259_load_dio_conf(devFD, &dioConfig)) {
                fprintf(stderr, "Loading DIO configuration failed!\n");
                return -1;
        }

        // open file descriptor for each DIO port
        for (i = 0; i <= PXI6259_MAX_DIO_PORT; i++) {
                sprintf(filename, "%s.%u.dio.%u", DEVICE_FILE, deviceNum, i);
                portFDs[i] = open(filename, O_RDWR | O_NONBLOCK);
                if (portFDs[i] < 0) {
                        fprintf(stderr, "Failed to open port: %s\n", strerror(errno));
                        return -1;
                }
        }

        // write to ports
        if (write(portFDs[0], &writeValue0, 4) != 4) {
                fprintf(stderr, "Error writing to port 0! Error: %s\n", strerror(errno));
        }
        if (write(portFDs[1], &writeValue1, 4) != 4) {
                fprintf(stderr, "Error writing to port 1! Error: %s\n", strerror(errno));
        }
        if (write(portFDs[2], &writeValue2, 4) != 4) {
                fprintf(stderr, "Error writing to port 2! Error: %s\n", strerror(errno));
        }

        // read from ports and output values
        if (read(portFDs[0], &readValue0, 4) != 4) {
                fprintf(stderr, "Error reading from port 0! Error: %s\n", strerror(errno));
        }
        printf("Value read on port 0: 0x%X\n", readValue0);

        if (read(portFDs[1], &readValue1, 4) != 4) {
                fprintf(stderr, "Error reading from port 1! Error: %s\n", strerror(errno));
        }
        printf("Value read on port 1: 0x%X\n", readValue1);

        if (read(portFDs[2], &readValue2, 4) != 4) {
                fprintf(stderr, "Error reading from port 2! Error: %s\n", strerror(errno));
        }
        printf("Value read on port 2: 0x%X\n", readValue2);

        // close file descriptors
        for (i = 0; i <= PXI6259_MAX_DIO_PORT; i++) {
                close(portFDs[i]);
        }
        close(devFD);

        return 0;
}

int main(int argc, char** argv) {
        uint32_t deviceNum;

        if (argc != 2) {
                usage(argv[0]);
                return 1;
        }

        deviceNum = strtoul(argv[1], NULL, 10);

        if (testDIO(deviceNum)) {
                fprintf(stderr, "Setup of DIO ports on card %d failed!\n", deviceNum);
        }

        return 0;
}
