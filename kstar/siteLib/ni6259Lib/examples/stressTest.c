/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/stressTest.c $
 * $Id: stressTest.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Stress test of analog input and output.
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
 * @example stressTest.c
 *
 * Intention of this example is to test long-time stability of the
 * driver. This test generates constant voltage on analog output channels using
 * waveform. In the same time, it samples generated signals. It also counts
 * these read samples and checks their values. It continuously outputs to the
 * console number of failed scans and number of samples that are not within some
 * tolerance as well as number of all read samples. Sampling should be terminated
 * by the user after some time, by pressing CTRL+C.
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

#define DEVICE_FILE "/dev/pxi6259"

void usage(char *name) {
        printf("Intention of this example is to test long-time stability of the\n"
                "driver. This test generates constant voltage on analog output channels using\n"
                "waveform. In the same time, it samples generated signals. It also counts\n"
                "these read samples and checks their values. It continuously outputs to the\n"
                "console number of failed scans and number of samples that are not within some\n"
                "tolerance as well as number of all read samples. Sampling should be terminated\n"
                "by the user after some time, by pressing CTRL+C.\n"
                "\n"
                "Usage: %s <device number>\n"
                "Example: %s 0\n", name, name);
}

int stressTest(uint32_t deviceNum) {
        uint32_t retval = 0;
        pxi6259_ao_conf_t aoConfig;
        pxi6259_ai_conf_t aiConfig;

        uint8_t numberOfChannels = 1;
        uint32_t numberOfSamples = 1000;
        uint64_t numberOfValueErrors = 0;
        uint64_t numberOfScansSum = 0;
        uint64_t numberOfScansSum_tmp = 0;
        uint64_t numberOfScanErrors = 0;
        int32_t scansRead = 0;

        int devAiFD = 0;
        int aiChannelFDs[numberOfChannels];

        int devAoFD = 0;
        int aoChannelFDs[numberOfChannels];

        uint32_t scans = 0;
        int i, j;
        char filename[256];

        uint16_t value = 0x1234;

        uint16_t rawWriteArray[numberOfChannels][numberOfSamples];
        uint16_t rawReadArray[numberOfSamples];

        // fill write array with values
        for (i = 0; i < numberOfChannels; i++) {
                for (j = 0; j < numberOfSamples; j++) {
                        rawWriteArray[i][j] = (value * j) % 0xFFFF;
                }
        }

        /*
         * Configure AI
         */
        // open AI file descriptor
        sprintf(filename, "%s.%u.ai", DEVICE_FILE, deviceNum);
        devAiFD = open(filename, O_RDWR | O_NONBLOCK);
        if ((devAiFD < 0) || (devAiFD > 1024)) {
                fprintf(stderr, "Failed to open AI device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AI configuration
        aiConfig = pxi6259_create_ai_conf();

        // configure AI channels
        for (i = 0; i < numberOfChannels; i++) {
                if (pxi6259_add_ai_channel(&aiConfig, i, AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
                        fprintf(stderr, "Failed to configure AI channel %u!\n", i);
                        return -1;
                }
        }

        // configure number of samples
        if (pxi6259_set_ai_number_of_samples(&aiConfig, 0, numberOfSamples, 1)) {
                fprintf(stderr, "Failed to configure number of samples on AI!\n");
                return -1;
        }

        // configure AI convert clock
        if (pxi6259_set_ai_convert_clk(&aiConfig, 16, 3, AI_CONVERT_SELECT_SI2TC, AI_CONVERT_POLARITY_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI convert clock!\n");
                return -1;
        }

        // configure AI sampling clock
        if (pxi6259_set_ai_sample_clk(&aiConfig, 16, 3, AI_SAMPLE_SELECT_SI_TC, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI sampling clock!\n");
                return -1;
        }

        // load AI configuration and let it apply
        if (pxi6259_load_ai_conf(devAiFD, &aiConfig)) {
                fprintf(stderr, "Failed to load AI configuration!\n");
                return -1;
        }

        // open file descriptor for each AI channel
        for (i = 0; i < numberOfChannels; i++) {
                sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum, i);
                aiChannelFDs[i] = open(filename, O_RDWR);
                if ((aiChannelFDs[i] < 0) || (aiChannelFDs[i] > 1024)) {
                        fprintf(stderr, "Failed to open AI channel %u: %s\n", aiChannelFDs[i], strerror(errno));
                        return -1;
                }
        }

        /*
         * Configure AO
         */
        // open AO file descriptor
        sprintf(filename, "%s.%u.ao", DEVICE_FILE, deviceNum);
        devAoFD = open(filename, O_RDWR | O_NONBLOCK);
        if ((devAoFD < 0) || (devAoFD > 1024)) {
                fprintf(stderr, "Failed to open AO device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AO configuration
        aoConfig = pxi6259_create_ao_conf();

        // enable waveform generation
        if (pxi6259_set_ao_waveform_generation(&aoConfig, 1)) {
                fprintf(stderr, "Failed to setup waveform generation on AO!\n");
                return -1;
        }

        // configure AO channels
        for (i = 0; i < numberOfChannels; i++) {
                if (pxi6259_add_ao_channel(&aoConfig, i, AO_DAC_POLARITY_BIPOLAR)) {
                        fprintf(stderr, "Failed to configure AO channel %u!\n", i);
                        return -1;
                }
        }

        // configure AO count
        if (pxi6259_set_ao_count(&aoConfig, numberOfSamples, numberOfChannels, 1)) {
                fprintf(stderr, "Failed to setup AO count!\n");
                return -1;
        }

        // configure AO update clock
        if (pxi6259_set_ao_update_clk(&aoConfig, AO_UPDATE_SOURCE_SELECT_UI_TC, AO_UPDATE_SOURCE_POLARITY_RISING_EDGE,
                        5)) {
                fprintf(stderr, "Failed to configure AO update clock!\n");
                return -1;
        }

        // load AO configuration and let it apply
        if (pxi6259_load_ao_conf(devAoFD, &aoConfig)) {
                fprintf(stderr, "Failed to load AO configuration!\n");
                return -1;
        }

        // open file descriptor for each AO channel
        for (i = 0; i < numberOfChannels; i++) {
                sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, i);
                aoChannelFDs[i] = open(filename, O_RDWR);
                if ((aoChannelFDs[i] < 0) || (aoChannelFDs[i] > 1024)) {
                        fprintf(stderr, "Failed to open AO channel %u: %s\n", aoChannelFDs[i], strerror(errno));
                        return -1;
                }
        }

        /*
         * Test core
         */
        // write values to AO channels
        for (i = 0; i < numberOfChannels; i++) {
                retval = write(aoChannelFDs[i], rawWriteArray[i], numberOfSamples * 2);
                if ((retval / 2) < numberOfSamples) {
                        fprintf(stderr, "Failed while writing values to AO! Values written: %x\n", retval);
                        return -1;
                }
        }

        // start AI segment (data acquisition)
        if (pxi6259_start_ai(devAiFD)) {
                fprintf(stderr, "Failed to start AI segment!\n");
                return -1;
        }

        // start AO segment (signal generation)
        if (pxi6259_start_ao(devAoFD)) {
                fprintf(stderr, "Failed to start AO segment!\n");
                return -1;
        }

        printf("Number of scan errors: %ld, number of value errors %ld, number of samples scanned %ld",
                        numberOfScanErrors, numberOfValueErrors, numberOfScansSum);

        // continuously scan channels and check values
        while (1) {
                for (i = 0; i < numberOfChannels; i++) {
                        scansRead = read(aiChannelFDs[i], rawReadArray, numberOfSamples * 2);
                        if (scansRead > 0) {
                                numberOfScansSum += (scansRead / 2);
                                for (scans = 0; scans < (scansRead / 2); scans++) {
                                        if (abs(rawReadArray[j] - rawWriteArray[i][0]) > 6553) {
                                                numberOfValueErrors++;
                                        }
                                }
                        }
                        if (scansRead < 0) {
                                numberOfScanErrors++;
                        }
                }
                // output current scan status
                if (numberOfScansSum >= (numberOfScansSum_tmp + 1000000)) {
                        numberOfScansSum_tmp = numberOfScansSum;
                        printf("\rNumber of scan errors: %ld, number of value errors %ld, number of samples scanned %ld",
                                        numberOfScanErrors, numberOfValueErrors, numberOfScansSum);
                }
        }

        // stop AI segment
        if (pxi6259_stop_ai(devAiFD)) {
                fprintf(stderr, "Failed to stop AI segment!\n");
                return -1;
        }

        // stop AO segment
        if (pxi6259_stop_ao(devAoFD)) {
                fprintf(stderr, "Failed to stop AO segment!\n");
                return -1;
        }

        // close file descriptors
        close(devAiFD);
        close(devAoFD);

        return 0;
}

int main(int argc, char** argv) {
        uint32_t deviceNum = 0;

        if (argc != 2) {
                usage(argv[0]);
                return 1;
        }

        deviceNum = strtoul(argv[1], NULL, 10);

        if (stressTest(deviceNum)) {
                fprintf(stderr, "Error running stress test!\n");
                return -1;
        }

        return 0;
}
