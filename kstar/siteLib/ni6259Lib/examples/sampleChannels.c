/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/sampleChannels.c $
 * $Id: sampleChannels.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Sample various analog channels.
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
 * @example sampleChannels.c
 *
 * This example acquires specified number of samples from analog input
 * channels, specified in command line. Samples values are printed to the
 * console for each provided analog input channel.
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

void usage(char *name) {
        printf("This example acquires specified number of samples from analog input\n"
                "channels, specified in command line. Samples values are printed to the\n"
                "console for each provided analog input channel.\n"
                "\n"
                "Usage: %s <device number> <number of samples> <channel 1> [channel n]*\n"
                "Example: %s 0 1 0 1 2 3\n", name, name);
}

int sampleChannels(uint32_t deviceNum, uint32_t nSamples, uint32_t nChannels, uint32_t *channels) {
        char filename[256];
        int i, j;
        int channelFDs[nChannels];
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
        for (i = 0; i < nChannels; i++) {
                if (pxi6259_add_ai_channel(&aiConfig, channels[i], AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
                        fprintf(stderr, "Failed to configure channel %u!\n", channels[i]);
                        return -1;
                }
        }

        // configure number of samples
        if (pxi6259_set_ai_number_of_samples(&aiConfig, nSamples, 0, 0)) {
                fprintf(stderr, "Failed to configure number of samples!\n");
                return -1;
        }

        // configure AI convert clock
        if (pxi6259_set_ai_convert_clk(&aiConfig, nChannels == 1 ? 16 : 20, 3, AI_CONVERT_SELECT_SI2TC,
                        AI_CONVERT_POLARITY_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI convert clock!\n");
                return -1;
        }

        // configure AI sampling clock
        if (pxi6259_set_ai_sample_clk(&aiConfig, nChannels == 1 ? 16 : 20, 3, AI_SAMPLE_SELECT_SI_TC,
                        AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI sampling clock!\n");
                return -1;
        }

        // load AI configuration and let it apply
        if (pxi6259_load_ai_conf(devFD, &aiConfig)) {
                fprintf(stderr, "Failed to load configuration!\n");
                return -1;
        }

        // open file descriptor for each AI channel
        for (i = 0; i < nChannels; i++) {
                sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum, channels[i]);
                channelFDs[i] = open(filename, O_RDWR | O_NONBLOCK);
                if (channelFDs[i] < 0) {
                        fprintf(stderr, "Failed to open channel %u: %s\n", channels[i], strerror(errno));
                        return -1;
                }
        }

        // wipe any previous samples
        for (i = 0; i < nChannels; i++) {
                float buffer[nSamples];
                pxi6259_read_ai(channelFDs[i], buffer, nSamples);
        }

        // start AI segment (data acquisition)
        if (pxi6259_start_ai(devFD)) {
                fprintf(stderr, "Failed to start data acquisition!\n");
                return -1;
        }
        usleep(100);

        // there should be nSamples * nChannels scans available
        for (i = 0; i < nChannels; i++) {
                float buffer[nSamples];
                int n = 0;

                while (n < nSamples) {
                        // read scaled samples
                        int rc = pxi6259_read_ai(channelFDs[i], &buffer[n], nSamples - n);
                        if (rc < 0) {
                                if (errno == EAGAIN) {
                                        continue;
                                } else {
                                        goto out;
                                }
                        }
                        n += rc;
                }

                // output samples values
                printf("Samples for channel %u:\n", channels[i]);
                for (j = 0; j < nSamples; j++) {
                        printf("\t%.03f\n", buffer[j]);
                }
        }

        out:
        // stop AI segment
        if (pxi6259_stop_ai(devFD)) {
                fprintf(stderr, "Failed to stop data acquisition!\n");
        }

        // close file descriptors
        for (i = 0; i < nChannels; i++) {
                close(channelFDs[i]);
        }
        close(devFD);

        return 0;
}

int main(int argc, char** argv) {
        int i;
        int maxChannels = PXI6259_MAX_AI_CHANNEL + 1;
        uint32_t nChannels = argc - 3;
        uint32_t deviceNum;
        uint32_t nSamples;

        if (argc <= 3) {
                usage(argv[0]);
                return 1;
        }

        if (nChannels > maxChannels) {
                fprintf(stderr, "Error: Up to %d channels supported!\n", maxChannels);
                return 1;
        }

        uint32_t channels[nChannels];
        deviceNum = strtoul(argv[1], NULL, 10);
        nSamples = strtoul(argv[2], NULL, 10);
        for (i = 0; i < nChannels; i++) {
                channels[i] = strtoul(argv[i + 3], NULL, 10);
        }

        if (sampleChannels(deviceNum, nSamples, nChannels, channels)) {
                fprintf(stderr, "Error reading samples!\n");
                return -1;
        }

        return 0;
}
