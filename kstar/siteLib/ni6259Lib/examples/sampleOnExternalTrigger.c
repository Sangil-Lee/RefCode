/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/sampleOnExternalTrigger.c $
 * $Id: sampleOnExternalTrigger.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Analog input continuous scan.
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
 * @example sampleOnExternalTrigger.c
 *
 * This example configures PFI1 to be used as sampling trigger, so that
 * signal on that pin determines if sampling is enabled or not. Then
 * analog input channel 0 is being continuously read and number of scanned
 * samples written to the console if sampling is enabled. This test should
 * be terminated by the user after some time, by pressing any key.
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
        printf("This example configures PFI1 to be used as sampling trigger, so that\n"
               "signal on that pin determines if sampling is enabled or not. Then\n"
               "analog input channel 0 is being continuously read and number of scanned\n"
               "samples written to the console if sampling is enabled. This test should\n"
               "be terminated by the user after some time, by pressing any key.\n"
               "\n"
               "Usage: %s <device number>\n"
               "Example: %s 0\n", name, name);
}

int main(int argc, char** argv) {
        uint32_t deviceNum;
        int channelFD = -1;
        char filename[256];
        pxi6259_ai_conf_t aiConfig;
        int devFD = -1;
        uint32_t num_samples = 10000;
        float read_buf[num_samples];
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

                // set PFI1 for sampling trigger
                if (pxi6259_set_ai_attribute(&aiConfig, AI_SAMPLE_SELECT, AI_SAMPLE_SELECT_PFI1)) {
                        fprintf(stderr, "Failed to set PFI1 for sampling trigger!\n");
                        break;
                }
                if (pxi6259_set_ai_attribute(&aiConfig, AI_SAMPLE_POLARITY, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                        fprintf(stderr, "Failed to set PFI1 sampling polarity!\n");
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

                // 5x on second read from channel
                printf("Waiting for samples, press any key to cancel!\n");
                do {
                        int scansRead = pxi6259_read_ai(channelFD, read_buf, num_samples);
                        // output to console only if something was read on a channel
                        if (scansRead > 0) {
                                printf("Scanned %i samples.\n", scansRead);
                        }
                } while(!checkForKeyPress(100000));

                // stop AI segment
                if (pxi6259_stop_ai(devFD)) {
                        fprintf(stderr, "Failed to stop data acquisition!\n");
                        break;
                }

                ret = 0;
        } while(0);

        // close file descriptors
        if (channelFD != -1) {
                close(channelFD);
        }
        if (devFD != -1) {
                close(devFD);
        }

        return ret;
}
