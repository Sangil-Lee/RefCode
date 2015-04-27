/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/startAOUsingTrigger.c $
 * $Id: startAOUsingTrigger.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Use external trigger to start sample AO.
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
 * @example startAOUsingTrigger.c
 *
 * Wait for external trigger to start generating sine signal on AO 0.
 * Amplitude of the signal is set to 7V. The frequency is calculated from provided
 * sample rate (default 2000000 ns). PFI1 is used as external trigger.
 */

#include <pxi-6259-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "common.h"

#define DEVICE_FILE "/dev/pxi6259"

#define DEFAULT_SAMPLE_RATE     2000000

void usage(char *name) {
        printf("Wait for external trigger to start generating sine signal on AO 0.\n"
               "Amplitude of the signal is set to 7V. The frequency is calculated from provided\n"
               "sample rate (default %d ns). PFI1 is used as external trigger.\n"
               "\n"
               "Usage: %s <device number> [sample rate]\n"
               "Example: %s 0 2000000\n", DEFAULT_SAMPLE_RATE, name, name);
}

int main(int argc, char** argv) {
        uint32_t deviceNum;
        uint32_t sampleRate = DEFAULT_SAMPLE_RATE;
        char filename[256];
        int devFD = -1;
        int channelFD = -1;
        pxi6259_ao_conf_t aoConfig;
        int nSamples = 1024;
        float scaledWriteArray[nSamples];
        int ret, i;
        const double pi = 4.0 * atan(1.0);
        const double radianPart = 2 * pi / nSamples;

        // parse command line parameters
        if (argc < 2 || argc > 3) {
                usage(argv[0]);
                return 1;
        }
        deviceNum = strtoul(argv[1], NULL, 10);
        if (argc == 3) {
                sampleRate = strtoul(argv[2], NULL, 10);
        }

        do {
                // open AO file descriptor
                sprintf(filename, "%s.%u.ao", DEVICE_FILE, deviceNum);
                devFD = open(filename, O_RDWR | O_NONBLOCK);
                if (devFD < 0) {
                        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                        break;
                }

                // initialize AO configuration
                aoConfig = pxi6259_create_ao_conf();

                // enable waveform generation
                if (pxi6259_set_ao_waveform_generation(&aoConfig, AO_FIFO_RETRANSMIT_ENABLE)) {
                        fprintf(stderr, "Failed to set AO waveform generation: %s!\n", strerror(errno));
                        break;
                }

                // configure AO channel 0
                if (pxi6259_add_ao_channel(&aoConfig, 0, AO_DAC_POLARITY_BIPOLAR)) {
                        fprintf(stderr, "Failed to configure channel: %s!\n", strerror(errno));
                        break;
                }

                // set AO count
                if (pxi6259_set_ao_count(&aoConfig, nSamples, 1, AO_CONTINUOUS_IGNORE_BC_TC)) {
                        fprintf(stderr, "Failed to configure AO count: %s!\n", strerror(errno));
                        break;
                }

                // configure AO update clock
                if (pxi6259_set_ao_update_clk(&aoConfig, AO_UPDATE_SOURCE_SELECT_UI_TC, AO_UPDATE_SOURCE_POLARITY_RISING_EDGE, 20000000 / sampleRate)) {
                        fprintf(stderr, "Failed to configure AO update clock: %s!\n", strerror(errno));
                        break;
                }

                // set PFI1 for start trigger
                if (pxi6259_set_ao_attribute(&aoConfig, AO_START1_SOURCE_SELECT, AO_START1_SELECT_PFI1)) {
                        fprintf(stderr, "Failed to set PFI1 for start trigger: %s!\n", strerror(errno));
                        break;
                }
                if (pxi6259_set_ao_attribute(&aoConfig, AO_START1_POLARITY_SELECT, AO_START1_POLARITY_FALLING_EDGE)) {
                        fprintf(stderr, "Failed to set PFI1 start polarity: %s!\n", strerror(errno));
                        break;
                }

                // load AO configuration and let it apply
                if (pxi6259_load_ao_conf(devFD, &aoConfig)) {
                        fprintf(stderr, "Failed to load configuration: %s!\n", strerror(errno));
                        break;
                }

                // open file descriptor for AO channel 0
                sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, 0);
                channelFD = open(filename, O_RDWR | O_NONBLOCK);
                if (channelFD < 0) {
                        fprintf(stderr, "Failed to open channel: %s\n", strerror(errno));
                        break;
                }

                // create one complete sine period in volts
                for (i = 0; i < nSamples; i++) {
                        scaledWriteArray[i] = (float) (7 * sin((double) radianPart * i));
                }

                // write sine to AO channel 0
                if (pxi6259_write_ao(channelFD, scaledWriteArray, nSamples) != nSamples) {
                        fprintf(stderr, "Failed while writing!\n");
                        break;
                }

                // start AO segment (signal generation)
                if (pxi6259_start_ao(devFD)) {
                        fprintf(stderr, "Failed to start AO: %s!\n", strerror(errno));
                        break;
                }

                // wait for user interaction
                printf("Wait for external trigger and then start generating sample waveform, press any key to cancel!\n");
                checkForKeyPress(-1);

                // stop AO segment
                if (pxi6259_stop_ao(devFD)) {
                        fprintf(stderr, "Failed to stop AO: %s!\n", strerror(errno));
                }

                ret = 0;
        } while (0);

        // close file descriptors
        if (channelFD != -1)
                close(channelFD);
        if (devFD != -1)
                close(devFD);

        return ret;
}
