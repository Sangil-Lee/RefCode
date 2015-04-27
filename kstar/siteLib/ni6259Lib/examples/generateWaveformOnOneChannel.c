/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/generateWaveformOnOneChannel.c $
 * $Id: generateWaveformOnOneChannel.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Generate waveform signal on one analog output channel.
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
 * @example generateWaveformOnOneChannel.c
 *
 * This example generates sine signal on selected analog output channel. The
 * amplitude of the signal is set to 7V. The frequency is calculated from provided
 * sample rate. If sample rate is not provided, it is automatically set to 2000000.
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
        printf("This example generates sine signal on selected analog output channel. The\n"
                "amplitude of the signal is set to 7V. The frequency is calculated from provided\n"
                "sample rate. If sample rate is not provided, it is automatically set to 2000000.\n"
                "\n"
                "Usage: %s <device number> <channel number> [sample rate]\n"
                "Example: %s 0 0 2000000\n", name, name);
}

uint32_t generateWaveformOnOneChannel(uint32_t deviceNum, uint32_t channelNum, uint32_t sampleRate) {
        uint32_t retval = 0;
        pxi6259_ao_conf_t aoConfig;
        int devFD;
        int channelFD;

        uint32_t periodDivisor = 20000000 / sampleRate;
        uint32_t number_of_samples = 1024;
        float scaledWriteArray[number_of_samples];

        uint32_t i;
        char filename[256];

        double pi;
        double radianPart;

        // open AO file descriptor
        sprintf(filename, "%s.%u.ao", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR | O_NONBLOCK);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AO configuration
        aoConfig = pxi6259_create_ao_conf();

        // enable waveform generation
        if (pxi6259_set_ao_waveform_generation(&aoConfig, 1)) {
                fprintf(stderr, "Failed to set AO waveform generation!\n");
                return -1;
        }

        // configure AO channel
        if (pxi6259_add_ao_channel(&aoConfig, channelNum, AO_DAC_POLARITY_BIPOLAR)) {
                fprintf(stderr, "Failed to configure channel!\n");
                return -1;
        }

        // set AO count
        if (pxi6259_set_ao_count(&aoConfig, number_of_samples, 1, 1)) {
                fprintf(stderr, "Failed to configure AO count!\n");
                return -1;
        }

        // configure AO update clock
        if (pxi6259_set_ao_update_clk(&aoConfig, AO_UPDATE_SOURCE_SELECT_UI_TC, AO_UPDATE_SOURCE_POLARITY_RISING_EDGE,
                        periodDivisor)) {
                fprintf(stderr, "Failed to configure AO update clock!\n");
                return -1;
        }

        // load AO configuration and let it apply
        if (pxi6259_load_ao_conf(devFD, &aoConfig)) {
                fprintf(stderr, "Failed to load configuration!\n");
                return -1;
        }

        // open file descriptor for each AO channel
        sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, channelNum);
        channelFD = open(filename, O_RDWR | O_NONBLOCK);
        if (channelFD < 0) {
                fprintf(stderr, "Failed to open channel: %s\n", strerror(errno));
                return -1;
        }

        // create one complete sine period in volts
        pi = 4.0 * atan(1.0);
        radianPart = 2 * pi / number_of_samples;
        for (i = 0; i < number_of_samples; i++) {
                scaledWriteArray[i] = (float) (7 * sin((double) radianPart * i));
        }

        // write sine to AO channel
        retval = pxi6259_write_ao(channelFD, scaledWriteArray, number_of_samples);
        if (retval != number_of_samples) {
                fprintf(stderr, "Failed while writing! Written %d samples.\n", retval);
                return -1;
        }

        // start AO segment (signal generation)
        if (pxi6259_start_ao(devFD)) {
                fprintf(stderr, "Failed to start AO!\n");
                return -1;
        }

        // wait for user interaction
        printf("Press ENTER to continue!\n");
        getchar();

        // stop AO segment
        if (pxi6259_stop_ao(devFD)) {
                fprintf(stderr, "Failed to stop AO!\n");
        }

        // close file descriptors
        close(channelFD);
        close(devFD);

        return 0;
}

int main(int argc, char** argv) {
        uint32_t deviceNum;
        uint32_t channelNum;
        uint32_t sampleRate = 2000000;

        if (argc < 3 || argc > 4) {
                usage(argv[0]);
                return 1;
        }

        deviceNum = strtoul(argv[1], NULL, 10);
        channelNum = strtoul(argv[2], NULL, 10);

        if (argc == 4) {
                sampleRate = strtoul(argv[3], NULL, 10);
        }

        if (generateWaveformOnOneChannel(deviceNum, channelNum, sampleRate)) {
                fprintf(stderr, "Error generating waveform signal\n");
                return -1;
        }

        return 0;
}
