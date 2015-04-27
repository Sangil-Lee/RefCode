/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/softwareTimedAcquisition.c $
 * $Id: softwareTimedAcquisition.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Sample two preselected analog channels.
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
 * @example softwareTimedAcquisition.c
 *
 * This example generates pulses and reads preset number of samples (10) from
 * first two analog input channels and outputs them to the console.
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
        printf("This example generates pulses and reads preset number of samples (10) from\n"
                "first two analog input channels and outputs them to the console.\n"
                "\n"
                "Usage: %s <device number>\n"
                "Example: %s 0\n", name, name);
}

int softwareTimedAcquisition(uint32_t deviceNum) {
        uint32_t num_samples = 10;
        float read_buf[num_samples];
        uint8_t channel0 = 0, channel1 = 1;
        int scans_read, i;
        int channel0FD, channel1FD;
        char filename[256];
        pxi6259_ai_conf_t aiConfig;
        int devFD;

        printf("Scan Channel %d and Channel %d On Card %d!\n", channel0, channel1, deviceNum);

        // open AI file descriptor
        sprintf(filename, "%s.%u.ai", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AI configuration
        aiConfig = pxi6259_create_ai_conf();

        // configure 2 AI channels
        if (pxi6259_add_ai_channel(&aiConfig, channel0, AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
                fprintf(stderr, "Failed to configure channel %u!\n", channel0);
                return -1;
        }
        if (pxi6259_add_ai_channel(&aiConfig, channel1, AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
                fprintf(stderr, "Failed to configure channel %u!\n", channel1);
                return -1;
        }

        // configure AI sampling clock
        if (pxi6259_set_ai_sample_clk(&aiConfig, 20, 3, AI_SAMPLE_SELECT_PULSE, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI Convert Clock!\n");
                return -1;
        }

        // export AI signal to another terminal
        if (pxi6259_export_ai_signal(&aiConfig, PXI6259_AI_SAMPLE_CLOCK, PXI6259_RTSI2)) {
                fprintf(stderr, "Failed to export AI signal!\n");
                return -1;
        }

        // load AI configuration and let it apply
        if (pxi6259_load_ai_conf(devFD, &aiConfig)) {
                fprintf(stderr, "Failed to load configuration!\n");
                return -1;
        }

        // open file descriptor for each of 2 used AI channels
        sprintf(filename, "%s.%d.ai.%d", DEVICE_FILE, deviceNum, channel0);
        channel0FD = open(filename, O_RDWR | O_NONBLOCK);
        if (channel0FD < 0) {
                fprintf(stderr, "Failed to open channel %u: %s\n", channel0, strerror(errno));
                return -1;
        }
        sprintf(filename, "%s.%d.ai.%d", DEVICE_FILE, deviceNum, channel1);
        channel1FD = open(filename, O_RDWR | O_NONBLOCK);
        if (channel1FD < 0) {
                fprintf(stderr, "Failed to open channel %u: %s\n", channel1, strerror(errno));
                return -1;
        }

        // start AI segment (data acquisition)
        if (pxi6259_start_ai(devFD)) {
                fprintf(stderr, "Failed to start data acquisition!\n");
                return -1;
        }

        // generate pulses
        for (i = 0; i < num_samples; i++) {
                usleep(1000);
                if (pxi6259_generate_ai_pulse(devFD, 1)) {
                        fprintf(stderr, "Failed to generate AI pulse!\n");
                        return -1;
                }
        }

        // read scaled samples and output them for each channel
        scans_read = pxi6259_read_ai(channel0FD, read_buf, num_samples);
        if (scans_read > 0) {
                printf("Scaled values for channel %d, scans %d:\n", channel0, scans_read);
                for (i = 0; i < scans_read; i++) {
                        printf("        %f \n", read_buf[i]);
                }
        }
        scans_read = pxi6259_read_ai(channel1FD, read_buf, num_samples);
        if (scans_read > 0) {
                printf("Scaled values for channel %d, scans %d:\n", channel1, scans_read);
                for (i = 0; i < scans_read; i++) {
                        printf("        %f \n", read_buf[i]);
                }
        }

        // stop AI segment
        if (pxi6259_stop_ai(devFD)) {
                fprintf(stderr, "Failed to stop data acquisition!\n");
        }

        // close file descriptors
        close(channel0FD);
        close(channel1FD);
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
        if (softwareTimedAcquisition(deviceNum)) {
                fprintf(stderr, "Error reading samples!\n");
                return -1;
        }

        return 0;
}
