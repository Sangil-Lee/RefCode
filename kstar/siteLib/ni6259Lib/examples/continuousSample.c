/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/continuousSample.c $
 * $Id: continuousSample.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Analog input continuous scan.
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
 * @example continuousSample.c
 *
 * This example configures sampling clock to sample 2 samples/second.
 * It continuously reads scaled samples from all analog input channels
 * and outputs number of scanned samples to the console. Sampling
 * should be terminated by the user after some time, by pressing CTRL+C.
 */

#include <pxi-6259-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_FILE "/dev/pxi6259"

void usage(char *name) {
        printf("This example configures sampling clock to sample 2 samples/second.\n"
                "It continuously reads scaled samples from all analog input channels\n"
                "and outputs number of scanned samples to the console. Sampling\n"
                "should be terminated by the user after some time, by pressing CTRL+C.\n"
                "\n"
                "Usage: %s <device number>\n"
                "Example: %s 0\n", name, name);
}

int continuousSample(uint32_t deviceNum) {
        int channelFDs[PXI6259_MAX_AI_CHANNEL + 1];
        char filename[256];
        int i;
        pxi6259_ai_conf_t aiConfig;
        int devFD;

        // open AI file descriptor
        sprintf(filename, "%s.%u.ai", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AI configuration
        aiConfig = pxi6259_create_ai_conf();

        // configure AI channels
        for (i = 0; i <= PXI6259_MAX_AI_CHANNEL; i++) {
                if (pxi6259_add_ai_channel(&aiConfig, i, AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
                        fprintf(stderr, "Failed to configure channel %u\n", i);
                        return -1;
                }
        }

        // configure AI sampling clock to sample with 2 samples/second
        if (pxi6259_set_ai_sample_clk(&aiConfig, 10000000, 3, AI_SAMPLE_SELECT_SI_TC, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI sampling clock!\n");
                return -1;
        }

        // load AI configuration and let it apply
        if (pxi6259_load_ai_conf(devFD, &aiConfig)) {
                fprintf(stderr, "Failed to load configuration!\n");
                return -1;
        }

        // open file descriptor for each AI channel
        for (i = 0; i <= PXI6259_MAX_AI_CHANNEL; i++) {
                sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum, i);
                channelFDs[i] = open(filename, O_RDWR | O_NONBLOCK);
                if (channelFDs[i] < 0) {
                        fprintf(stderr, "Failed to open channel %u: %s\n", i, strerror(errno));
                        return -1;
                }
        }

        // start AI segment (data acquisition)
        if (pxi6259_start_ai(devFD)) {
                fprintf(stderr, "Failed to start data acquisition!\n");
                return -1;
        }
        usleep(100);

        // read samples continuously
        while (1) {
                int n = 0;
                for (i = 0; i <= PXI6259_MAX_AI_CHANNEL; i++) {
                        // make sure channel's samples are read at least once
                        int scans_read = 1000;
                        while (scans_read > 500) {
                                uint32_t num_samples = 20000;
                                float read_buf[num_samples];

                                // read scaled samples
                                scans_read = pxi6259_read_ai(channelFDs[i], read_buf, num_samples);
                                if (scans_read < 0) {
                                        fprintf(stderr, "Failed while reading channel: %u: %s\n", i, strerror(errno));
                                        break;
                                }
                                n += scans_read;
                        }
                }
                // output number of scans
                printf("%d scans per second\n", n);
                sleep(1);
        }

        // stop AI segment
        if (pxi6259_stop_ai(devFD)) {
                fprintf(stderr, "Failed to stop data acquisition!\n");
        }

        // close file descriptors
        for (i = 0; i <= PXI6259_MAX_AI_CHANNEL; i++) {
                close(channelFDs[i]);
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

        if (continuousSample(deviceNum)) {
                fprintf(stderr, "Error reading samples!\n");
                return -1;
        }

        return 0;
}
