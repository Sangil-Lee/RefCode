/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/testSampleRate.c $
 * $Id: testSampleRate.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Test of analog input sample rate.
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
 * @example testSampleRate.c
 *
 * This example tests sampling rate of analog input segment. It does this by
 * scanning one or more channels several times and measuring time spent for
 * reading the samples. Channels are scanned in separate threads. Normal
 * results should be 1.25 M samples/s for one channel and 1.00 M samples/s
 * for 2 or more channels. This example does not report at which sample rate
 * samples are taken but rather calculates sample rate achieved including
 * overhead needed to read and or scale samples. It should be noted that
 * overhead for 1 sample is the same as for 100000. If number of samples
 * is not provided, default value of 1000000 is used. Scaling is on by default.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <pxi-6259-lib.h>

#define DEVICE_FILE "/dev/pxi6259"
#define MAX_READ_ARRAY 2048

struct attributes {
        int channelFD;
        uint64_t numberOfSamples;
        uint64_t scansRead;
        int scale;
};

void usage(char *name) {
        printf("This example tests sampling rate of analog input segment. It does this by\n"
                "scanning one or more channels several times and measuring time spent for\n"
                "reading the samples. Channels are scanned in separate threads. Normal\n"
                "results should be 1.25 M samples/s for one channel and 1.00 M samples/s\n"
                "for 2 or more channels. This example does not report at which sample rate\n"
                "samples are taken but rather calculates sample rate achieved including\n"
                "overhead needed to read and or scale samples. It should be noted that\n"
                "overhead for 1 sample is the same as for 100000. If number of samples\n"
                "is not provided, default value of 1000000 is used. Scaling is on by default.\n"
                "\n"
                "Usage: %s <device number> <number of channels> [number of samples] [scale (1:ON 0:OFF)]\n"
                "Example: %s 0 2 1000000 1\n", name, name);
}

/*
 * Thread execution function.
 */
void* readSamples(void* ptr) {
        int16_t readRawArray[MAX_READ_ARRAY];
        float readScaledArray[MAX_READ_ARRAY];
        int retval;

        struct attributes* attributes = (struct attributes *) ptr;

        if (attributes == NULL) {
                fprintf(stderr, "Error in thread: invalid argument!\n");
                return NULL;
        }

        // read scaled or unscaled samples until limit is reached
        while (attributes->scansRead < attributes->numberOfSamples) {
                usleep(100);
                retval = 0;
                if (attributes->scale) {
                        retval = pxi6259_read_ai(attributes->channelFD, readScaledArray, MAX_READ_ARRAY);
                } else {
                        retval = read(attributes->channelFD, readRawArray, MAX_READ_ARRAY * 2);
                }

                if (retval > 0) {
                        if (attributes->scale) {
                                attributes->scansRead += retval;
                        } else {
                                attributes->scansRead += (retval / 2);
                        }
                        continue;
                } else if (retval == 0) {
                        fprintf(stderr, "No data available! Samples read: %ld\n", attributes->scansRead);
                        continue;
                } else if (retval < 0 && errno == EAGAIN) {
                        continue;
                } else if (retval < 0) {
                        fprintf(stderr, "Error while reading scans! Error: %s\n", strerror(errno));
                        return NULL;
                }
        }

        return 0;
}

uint32_t sampleAIChannels(uint32_t deviceNum, uint32_t numberOfChannels, uint32_t numberOfSamples, uint8_t scale) {
        int retval;
        uint32_t scansReadSum = 0;
        int i;
        char filename[256];

        int devFD;
        pxi6259_ai_conf_t aiConfig;

        struct timeval startTime, currentTime;
        long usecDiff;
        float rate;

        struct attributes channelAtt[numberOfChannels];

        uint32_t convertPeriodDivisor = numberOfChannels > 1 ? 20 : 16;
        uint32_t samplePeriodDivisor = numberOfChannels > 1 ? 20 : 16;

        pthread_t thread[numberOfChannels];

        void *status;

        memset(channelAtt, 0, sizeof(struct attributes) * numberOfChannels);

        // open AI file descriptor
        sprintf(filename, "%s.%u.ai", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR | O_NONBLOCK);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AI configuration
        aiConfig = pxi6259_create_ai_conf();

        // configure AI channels
        for (i = 0; i < numberOfChannels; i++) {
                if (pxi6259_add_ai_channel(&aiConfig, i, AI_POLARITY_BIPOLAR, 1, AI_CHANNEL_TYPE_RSE, 0)) {
                        fprintf(stderr, "Failed to configure channel %u!\n", i);
                        return -1;
                }
        }

        // set number of samples
        if (pxi6259_set_ai_number_of_samples(&aiConfig, numberOfSamples, 0, 0)) {
                fprintf(stderr, "Failed to configure number of samples!\n");
                return -1;
        }

        // configure AI convert clock
        if (pxi6259_set_ai_convert_clk(&aiConfig, convertPeriodDivisor, 3, AI_CONVERT_SELECT_SI2TC,
                        AI_CONVERT_POLARITY_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI Convert Clock!\n");
                return -1;
        }

        // configure AI sampling clock
        if (pxi6259_set_ai_sample_clk(&aiConfig, samplePeriodDivisor, 3, AI_SAMPLE_SELECT_SI_TC,
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
        for (i = 0; i < numberOfChannels; i++) {
                sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum, i);
                channelAtt[i].channelFD = open(filename, O_RDWR | O_NONBLOCK);
                if (channelAtt[i].channelFD < 0) {
                        fprintf(stderr, "Failed to open channel %u: %s\n", channelAtt[i].channelFD, strerror(errno));
                        return -1;
                }
        }

        // setup channel attributes
        for (i = 0; i < numberOfChannels; i++) {
                channelAtt[i].numberOfSamples = numberOfSamples;
                channelAtt[i].scale = scale;
        }

        gettimeofday(&startTime, NULL);

        // start AI segment (data acquisition)
        if (pxi6259_start_ai(devFD)) {
                fprintf(stderr, "Failed to start data acquisition!\n");
                return -1;
        }

        // create child threads
        for (i = 0; i < numberOfChannels; i++) {
                retval = pthread_create(&thread[i], NULL, readSamples, (void *) &channelAtt[i]);
                if (retval) {
                        fprintf(stderr, "Error from thread creation: %d\n", retval);
                        exit(-1);
                }

        }

        // wait for child threads to finish
        for (i = 0; i < numberOfChannels; i++) {
                pthread_join(thread[i], &status);
        }

        gettimeofday(&currentTime, NULL);

        // stop AI segment
        if (pxi6259_stop_ai(devFD)) {
                if (errno == EBUSY) {
                        usleep(1000);
                        if (pxi6259_stop_ai(devFD)) {
                                fprintf(stderr, "Failed to stop data acquisition!\n");
                                return -1;
                        }
                } else {
                        fprintf(stderr, "Failed to stop data acquisition!\n");
                        return -1;
                }
        }

        // add all number of samples
        for (i = 0; i < numberOfChannels; i++) {
                scansReadSum += channelAtt[i].scansRead;
        }

        // calculate and output values
        usecDiff = (currentTime.tv_sec * 1000000 + currentTime.tv_usec) - (startTime.tv_sec * 1000000
                        + startTime.tv_usec);

        rate = (float) scansReadSum / (float) usecDiff * 1000000.0;
        printf("Difference %ld ms\n", usecDiff / 1000);
        printf("Samples scaned: %d\n", scansReadSum);
        printf("Scan rate: %f samples/s\n", rate);

        // close file descriptors
        for (i = 0; i < numberOfChannels; i++) {
                close(channelAtt[i].channelFD);
        }
        close(devFD);

        return 0;
}

int main(int argc, char** argv) {
        uint32_t deviceNum;
        uint32_t numberOfChannels;
        uint32_t numberOfSamples = 1000000;
        uint8_t scale = 1;

        if (argc < 3 || argc > 5) {
                usage(argv[0]);
                return 1;
        }

        deviceNum = strtoul(argv[1], NULL, 10);
        numberOfChannels = strtoul(argv[2], NULL, 10);

        if (argc >= 4) {
                numberOfSamples = strtoul(argv[3], NULL, 10);
        }
        if (argc == 5) {
                scale = atoi(argv[4]);
        }

        printf("Scan %d channels %d times!\n", numberOfChannels, numberOfSamples);

        if (sampleAIChannels(deviceNum, numberOfChannels, numberOfSamples, scale)) {
                fprintf(stderr, "Error reading samples!\n");
                return -1;
        }

        return 0;
}
