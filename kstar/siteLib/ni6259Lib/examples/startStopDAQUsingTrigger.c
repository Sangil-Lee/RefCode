/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/startStopDAQUsingTrigger.c $
 * $Id: startStopDAQUsingTrigger.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Exercise start and stop external triggers for DAQ.
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
 * @example startStopDAQUsingTrigger.c
 *
 * Example exercises starting and stopping of DAQ using external triggers.
 * It first configures sampling clock to sample 2 samples/second.
 * Then it configures PFI1 to act as start trigger and PFI2 to act as stop
 * trigger. It continuously reads scaled samples from analog input channel 0
 * and outputs number of scanned samples to the console if sampling is enabled.
 * Test should be terminated by the user after some time, by pressing any key.
 */

#include <pxi-6259-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "common.h"

#define DEVICE_FILE "/dev/pxi6259"

void usage(char *name) {
        printf("Example exercises starting and stopping of DAQ using external triggers.\n"
               "It first configures sampling clock to sample 2 samples/second.\n"
               "Then it configures PFI1 to act as start trigger and PFI2 to act as stop\n"
               "trigger. It continuously reads scaled samples from analog input channel 0\n"
               "and outputs number of scanned samples to the console if sampling is enabled.\n"
               "Test should be terminated by the user after some time, by pressing any key.\n"
               "\n"
               "Usage: %s <device number>\n"
               "Example: %s 0\n", name, name);
}

int main(int argc, char** argv) {
        uint32_t deviceNum;
        char filename[256];
        pxi6259_ai_conf_t aiConfig;
        int devFD = -1;
        int channelFD = -1;
        uint32_t nSamples = 10000;
        float buf[nSamples];
        int ret = 1;

        if (argc != 2) {
                usage(argv[0]);
                return 1;
        }
        deviceNum = strtoul(argv[1], NULL, 10);

        do {
                // open AI file descriptor
                sprintf(filename, "%s.%u.ai", DEVICE_FILE, deviceNum);
                devFD = open(filename, O_RDWR);
                if (devFD < 0) {
                        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                        break;
                }

                // initialize AI configuration
                aiConfig = pxi6259_create_ai_conf();

                // configure AI channel 0
                if (pxi6259_add_ai_channel(&aiConfig, 0, AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
                        fprintf(stderr, "Failed to configure channel %u\n", 0);
                        break;
                }

                // configure AI sampling clock to sample with 2 samples/second
                if (pxi6259_set_ai_sample_clk(&aiConfig, 10000000, 3, AI_SAMPLE_SELECT_SI_TC, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                        fprintf(stderr, "Failed to configure AI sampling clock!\n");
                        break;
                }

                // set PFI1 for start trigger
                if (pxi6259_set_ai_attribute(&aiConfig, AI_START_SELECT, AI_START_SELECT_PFI1)) {
                        fprintf(stderr, "Failed to set PFI1 for start trigger: %s!\n", strerror(errno));
                        break;
                }
                if (pxi6259_set_ai_attribute(&aiConfig, AI_START_POLARITY, AI_START_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                        fprintf(stderr, "Failed to set PFI1 start polarity!\n");
                        break;
                }

                // set PFI2 for stop trigger
                if (pxi6259_set_ai_attribute(&aiConfig, AI_REFERENCE_SELECT, AI_REFERENCE_SELECT_PFI2)) {
                        fprintf(stderr, "Failed to set PFI2 for stop trigger!\n");
                        break;
                }
                if (pxi6259_set_ai_attribute(&aiConfig, AI_REFERENCE_POLARITY, AI_REFERENCE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                        fprintf(stderr, "Failed to set PFI2 stop polarity!\n");
                        break;
                }

                // set number of AI samples at a time
                if (pxi6259_set_ai_number_of_samples(&aiConfig, 2, 0, AI_CONTINUOUS_DISABLE)) {
                        fprintf(stderr, "Failed to set number of samples!\n");
                        break;
                }

                // load AI configuration and let it apply
                if (pxi6259_load_ai_conf(devFD, &aiConfig)) {
                        fprintf(stderr, "Failed to load configuration!\n");
                        break;
                }

                // open file descriptor for AI channel 0
                sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum, 0);
                channelFD = open(filename, O_RDWR | O_NONBLOCK);
                if (channelFD < 0) {
                        fprintf(stderr, "Failed to open channel %u: %s\n", 0, strerror(errno));
                        break;
                }

                // start AI segment (data acquisition)
                if (pxi6259_start_ai(devFD)) {
                        fprintf(stderr, "Failed to start data acquisition!\n");
                        break;
                }

                // check for new samples every second
                printf("Waiting for samples, press any key to cancel!\n");
                do {
                        int nScans = pxi6259_read_ai(channelFD, buf, nSamples);
                        if (nScans > 0) {
                                printf("%i samples received\n", nScans);
                        }
                } while (!checkForKeyPress(1000000));

                // stop AI segment
                if (pxi6259_stop_ai(devFD)) {
                        fprintf(stderr, "Failed to stop data acquisition!\n");
                        break;
                }

                ret = 0;
        } while(0);

        // close file descriptors
        if (channelFD != -1)
                close(channelFD);
        if (devFD != -1)
                close(devFD);

        return ret;
}
