/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/finiteSampling.c $
 * $Id: finiteSampling.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Configure n samples and read them all at once.
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
 * @example finiteSampling.c
 *
 * Acquire pre-configured number of samples at once.
 * The example configures 100 kHz sampling rate and then it instructs
 * driver to acquire only user-configurable number of samples.
 * It then waits the time needed for all samples and a little bit more.
 * Finally fetch all samples at once.
 */

#include <pxi-6259-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_FILE         "/dev/pxi6259"
#define SAMPLING_FREQ       100000 // in Hz

void usage(char *name) {
        printf("Acquire pre-configured number of samples at once.\n"
               "The example configures %d kHz sampling rate and then it instructs\n"
               "driver to acquire only user-configurable number of samples.\n"
               "It then waits the time needed for all samples and a little bit more.\n"
               "Finally fetch all samples at once.\n"
               "\n"
               "Usage: %s <device number> <channel number> <number samples>\n"
               "Example: %s 0 1 10\n", SAMPLING_FREQ/1000, name, name);
}

int main(int argc, char** argv) {
        uint32_t device_num, channel_num, n_samples;
        int device, channel;
        pxi6259_ai_conf_t aiConfig;
        char filename[256];
        float *buffer;
        char *endptr;
        int n_read_samples;

        if (argc != 4) {
                usage(argv[0]);
                return 1;
        }

        // parse command line parameters
        device_num = strtoul(argv[1], &endptr, 10);
        if (device_num == 0 && endptr == argv[1]) {
                fprintf(stderr, "Wrong device id, numerical values only!\n");
                return 1;
        }
        channel_num = strtoul(argv[2], &endptr, 10);
        if ((channel_num == 0 && endptr == argv[2]) || channel_num > PXI6259_MAX_AI_CHANNEL) {
                fprintf(stderr, "Wrong channel id, numerical values only in range %d - %d!\n", 0, PXI6259_MAX_AI_CHANNEL);
                return 1;
        }
        n_samples = strtoul(argv[3], &endptr, 10);
        if (n_samples == 0 && endptr == argv[3]) {
                fprintf(stderr, "Wrong number of samples, numerical values only!\n");
                return 1;
        }

        // open AI file descriptor
        sprintf(filename, "%s.%u.ai", DEVICE_FILE, device_num);
        device = open(filename, O_RDWR);
        if (device < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return 2;
        }

        // initialize AI configuration
        aiConfig = pxi6259_create_ai_conf();

        // configure AI channels
        if (pxi6259_add_ai_channel(&aiConfig, channel_num, AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
                fprintf(stderr, "Failed to configure channel %u: %s\n", channel_num, strerror(errno));
                return 3;
        }

        // configure AI sampling clock (clock divison = 20Mhz / SAMPLING_FREQ)
        if (pxi6259_set_ai_sample_clk(&aiConfig, 20000000 / SAMPLING_FREQ, 3, AI_SAMPLE_SELECT_SI_TC, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI sampling clock: %s!\n", strerror(errno));
                return 4;
        }

        // set number of AI samples at a time
        if (pxi6259_set_ai_number_of_samples(&aiConfig, n_samples, 0, AI_CONTINUOUS_DISABLE)) {
                fprintf(stderr, "Failed to set number of samples: %s!\n", strerror(errno));
                return 5;
        }

        // load AI configuration and let it apply
        if (pxi6259_load_ai_conf(device, &aiConfig)) {
                fprintf(stderr, "Failed to load configuration: %s!\n", strerror(errno));
                return 6;
        }

        // open file descriptor for each AI channel
        sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, device_num, channel_num);
        channel = open(filename, O_RDWR);
        if (channel < 0) {
                fprintf(stderr, "Failed to open channel %u: %s\n", channel_num, strerror(errno));
                return 7;
        }

        // allocate buffer
        buffer = malloc(sizeof(float) * (n_samples + 1));
        if (!buffer) {
                fprintf(stderr, "Failed to allocate buffer\n");
                return 8;
        }

        // start AI segment (data acquisition)
        if (pxi6259_start_ai(device)) {
                fprintf(stderr, "Failed to start data acquisition: %s!\n", strerror(errno));
                free(buffer);
                return 9;
        }


        /* In commented part of code bellow you can see how to perform AI acquisition in alternative way */

        // let acquisition enought time to acquire all samples - double the needed time
        usleep(2 * n_samples * 1000000ULL /* us */ / SAMPLING_FREQ);

        // read samples continuously - although +1 requested, n_samples should be returned
        n_read_samples = pxi6259_read_ai(channel, buffer, n_samples + 1);
        if (n_read_samples < 0) {
                fprintf(stderr, "Failed while reading channel: %s\n", strerror(errno));
                free(buffer);
                return 10;
        }

        printf("Acquired %d of %d samples, test %s!\n", n_read_samples, n_samples,
                                                        n_read_samples == n_samples ? "SUCCEEDED" : "FAILED");

        free(buffer);

        // stop AI segment
        if (pxi6259_stop_ai(device)) {
                fprintf(stderr, "Failed to stop data acquisition!\n");
                // Don't treat as real error since samples acquired
        }

        // close file descriptors
        close(channel);
        close(device);

        return 0;
}
