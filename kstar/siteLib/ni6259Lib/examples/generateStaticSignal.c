/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/generateStaticSignal.c $
 * $Id: generateStaticSignal.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Generate static signal with analog output.
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
 * @example generateStaticSignal.c
 *
 * This example generates user defined voltages on analog output channels.
 * Voltages should be in range from -10.0V to +10.0V in decimal precision.
 * Each voltage corresponds to one channel, starting from 0 on to number
 * of all analog output channels.
 */

#include <pxi-6259-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_FILE "/dev/pxi6259"

void usage(const char *name) {
        printf("This example generates user defined voltages on analog output channels.\n"
                "Voltages should be in range from -10.0V to +10.0V in decimal precision.\n"
                "Each voltage corresponds to one channel, starting from 0 on to number\n"
                "of all analog output channels.\n"
                "\n"
                "Usage: %s <device number> <AO0 voltage> [AOn voltage]*\n"
                "Example: %s 0 -2.4 1.3 3.7 -0.9\n", name, name);
}

int generateStaticSignal(uint32_t deviceNum, float *voltages, uint8_t nVoltages) {
        char filename[256];
        int channelFDs[nVoltages];
        int i;
        pxi6259_ao_conf_t aoConfig;
        int devFD;

        // open AO file descriptor
        sprintf(filename, "%s.%u.ao", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AO configuration
        aoConfig = pxi6259_create_ao_conf();

        // configure AO channels
        for (i = 0; i < nVoltages; i++) {
                if (pxi6259_add_ao_channel(&aoConfig, i, AO_DAC_POLARITY_BIPOLAR)) {
                        fprintf(stderr, "Failed to configure channel %d!\n", i);
                        return -1;
                }
        }

        // enable signal generation
        if (pxi6259_set_ao_attribute(&aoConfig, AO_SIGNAL_GENERATION, AO_SIGNAL_GENERATION_STATIC)) {
                fprintf(stderr, "Failed to enable generating static signal!\n");
                return -1;
        }

        // set continuous mode
        if (pxi6259_set_ao_attribute(&aoConfig, AO_CONTINUOUS, 0)) {
                fprintf(stderr, "Failed to set continuous mode!\n");
                return -1;
        }

        // load AO configuration and let it apply
        if (pxi6259_load_ao_conf(devFD, &aoConfig)) {
                fprintf(stderr, "Failed to load configuration!\n");
                return -1;
        }

        // open file descriptor for each AO channel
        for (i = 0; i < nVoltages; i++) {
                sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, i);
                channelFDs[i] = open(filename, O_RDWR | O_NONBLOCK);
                if (channelFDs[i] < 0) {
                        fprintf(stderr, "Failed to open channel %u: %s\n", i, strerror(errno));
                        return -1;
                }
        }

        // start AO segment (signal generation)
        if (pxi6259_start_ao(devFD)) {
                fprintf(stderr, "Failed to start segment!n");
                return -1;
        }

        // put static voltages on AO channels
        for (i = 0; i < nVoltages; i++) {
                int rc = pxi6259_write_ao(channelFDs[i], &voltages[i], 1);
                if (rc < 0) {
                        fprintf(stderr, "Failed to write to AO channel: %u\n", i);
                        return -1;
                }
        }

        // stop AO segment
        if (pxi6259_stop_ao(devFD)) {
                fprintf(stderr, "Failed to stop generating signal!\n");
                return -1;
        }

        // close file descriptors
        for (i = 0; i < nVoltages; i++) {
                close(channelFDs[i]);
        }
        close(devFD);

        return 0;
}

int main(int argc, char** argv) {
        int i;
        int maxChannels = PXI6259_MAX_AO_CHANNEL + 1;
        uint32_t nVoltages = argc - 2;
        uint32_t deviceNum;

        if (argc <= 2) {
                usage(argv[0]);
                return 1;
        }

        if (nVoltages > maxChannels) {
                fprintf(stderr, "Error: up to %d channels supported!\n", maxChannels);
                return 1;
        }

        deviceNum = strtoul(argv[1], NULL, 10);
        float voltages[nVoltages];
        for (i = 0; i < nVoltages; i++) {
                voltages[i] = strtof(argv[i + 2], NULL);
        }

        if (generateStaticSignal(deviceNum, voltages, nVoltages)) {
                fprintf(stderr, "Error generating static signal!\n");
                return -1;
        }

        printf("Applied requested voltages to analog outputs.\n");
        return 0;
}
