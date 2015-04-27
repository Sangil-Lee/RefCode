/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/generalPurposeCounterTest.c $
 * $Id: generalPurposeCounterTest.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Test of general purpose counters.
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
 * @example generalPurposeCounterTest.c
 *
 * This example tests general purpose counters. It tests their initial
 * values setting as well as their counting. These counters use different
 * timebases and count up for some time, which is measured. From this
 * measured time and the values of the counters, the frequency of the board
 * is then calculated and written to the console.
 */

#include <pxi-6259-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_FILE "/dev/pxi6259"

void usage(char *name) {
        printf("This example tests general purpose counters. It tests their initial\n"
                "values setting as well as their counting. These counters use different\n"
                "timebases and count up for some time, which is measured. From this\n"
                "measured time and the values of the counters, the frequency of the board\n"
                "is then calculated and written to the console.\n"
                "\n"
                "Usage: %s <device number>\n"
                "Example: %s 0\n", name, name);
}

int gpcTest(uint32_t deviceNum, uint8_t timebase) {
        int counter0FD = 0, counter1FD = 0;
        char filename[256];
        uint32_t initialValue = 0;
        uint32_t gpcValue0 = 0;
        uint32_t gpcValue1 = 0;

        struct timeval startTime0, endTime0;
        struct timeval startTime1, endTime1;
        long usecDiff0, usecDiff1;
        float frequency;

        pxi6259_gpc_conf_t gpcConfig;
        int devFD;

        // open GPC file descriptor
        sprintf(filename, "%s.%u.gpc", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR | O_NONBLOCK);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize GPC configuration
        gpcConfig = pxi6259_create_gpc_conf();

        // configure GPC's initial value
        if (pxi6259_set_gpc_attribute(&gpcConfig, GPC_INITIAL_VALUE, initialValue)) {
                fprintf(stderr, "Failed to set GPC config attribute!\n");
                return -1;
        }

        // configure GPC timebase
        enum pxi6259_segment_attribute segAttrTimebase = timebase ? GPC_SOURCE_SELECT_TIMEBASE1
                        : GPC_SOURCE_SELECT_TIMEBASE2;
        if (pxi6259_set_gpc_attribute(&gpcConfig, GPC_SOURCE_SELECT, segAttrTimebase)) {
                fprintf(stderr, "Failed to set GPC config attribute!\n");
                return -1;
        }

        // export GPC signal to another terminal
        if (pxi6259_export_gpc_signal(&gpcConfig, PXI6259_GPC0_OUTPUT, PXI6259_RTSI6)) {
                fprintf(stderr, "Failed to export GPC signal!\n");
                return -1;
        }

        // configure some other GPC stuff
        if (pxi6259_set_gpc_attribute(&gpcConfig, GPC_COUNT_DIRECTION, GPC_COUNT_DIRECTION_SOFTWARE_UP)) {
                fprintf(stderr, "Failed to set GPC config attribute!\n");
                return -1;
        }
        if (pxi6259_set_gpc_attribute(&gpcConfig, GPC_SELECT_TYPE, GPC_TASK_TYPE_SIMPLE_COUNTER)) {
                fprintf(stderr, "Failed to set GPC config attribute!\n");
                return -1;
        }

        // load GPC configurations and let them apply
        if (pxi6259_load_gpc0_conf(devFD, &gpcConfig)) {
                fprintf(stderr, "Failed to load GPC0 config!\n");
                return -1;
        }
        if (pxi6259_load_gpc1_conf(devFD, &gpcConfig)) {
                fprintf(stderr, "Failed to load GPC1 config!\n");
                return -1;
        }

        // open file descriptor for each GPC
        sprintf(filename, "%s.%d.gpc.0", DEVICE_FILE, deviceNum);
        counter0FD = open(filename, O_RDWR | O_NONBLOCK);
        if (counter0FD < 0) {
                fprintf(stderr, "Failed to open counter 0: %s\n", strerror(errno));
                return -1;
        }
        sprintf(filename, "%s.%d.gpc.1", DEVICE_FILE, deviceNum);
        counter1FD = open(filename, O_RDWR | O_NONBLOCK);
        if (counter1FD < 0) {
                fprintf(stderr, "Failed to open counter 1: %s\n", strerror(errno));
                return -1;
        }

        // read and check counters values
        if (read(counter0FD, &gpcValue0, 4) != 4) {
                fprintf(stderr, "Failed to read GPC0!\n");
                return -1;
        }
        if (gpcValue0 != initialValue) {
                fprintf(stderr, "GPC0 value is invalid!\n");
                return -1;
        }

        if (read(counter1FD, &gpcValue1, 4) != 4) {
                fprintf(stderr, "Failed to read GPC1!\n");
                return -1;
        }
        if (gpcValue1 != initialValue) {
                fprintf(stderr, "GPC1 value is invalid!\n");
                return -1;
        }

        // start both counters
        gettimeofday(&startTime0, NULL);
        if (pxi6259_start_gpc0(devFD)) {
                fprintf(stderr, "Failed to start GPC0!\n");
                return -1;
        }
        gettimeofday(&startTime1, NULL);
        if (pxi6259_start_gpc1(devFD)) {
                fprintf(stderr, "Failed to start GPC1!\n");
                return -1;
        }

        usleep(1000000);

        // get value of counter 0
        gettimeofday(&endTime0, NULL);
        if (read(counter0FD, &gpcValue0, 4) != 4) {
                fprintf(stderr, "Failed to read GPC0!\n");
                return -1;
        }

        // get value of counter 1
        gettimeofday(&endTime1, NULL);
        if (read(counter1FD, &gpcValue1, 4) != 4) {
                fprintf(stderr, "Failed to read GPC1!\n");
                return -1;
        }

        // stop both counters
        if (pxi6259_stop_gpc0(devFD)) {
                fprintf(stderr, "Failed to stop GPC0!\n");
                return -1;
        }
        if (pxi6259_stop_gpc1(devFD)) {
                fprintf(stderr, "Failed to stop GPC1!\n");
                return -1;
        }

        // calculate frequencies and output them
        usecDiff0 = (endTime0.tv_sec * 1000000 + endTime0.tv_usec) - (startTime0.tv_sec * 1000000 + startTime0.tv_usec);
        frequency = (float) gpcValue0 / (float) usecDiff0 * 1000000.0;

        printf("%s timebase used. Measured frequency: %f%s\n", timebase ? "Fast" : "Slow", timebase ? (frequency
                        / 1000000) : (frequency / 1000), timebase ? "MHz" : "kHz");

        usecDiff1 = (endTime1.tv_sec * 1000000 + endTime1.tv_usec) - (startTime1.tv_sec * 1000000 + startTime1.tv_usec);
        frequency = (float) gpcValue1 / (float) usecDiff1 * 1000000.0;

        printf("%s timebase used. Measured frequency: %f%s\n", timebase ? "Fast" : "Slow", timebase ? (frequency
                        / 1000000) : (frequency / 1000), timebase ? "MHz" : "kHz");

        // close file descriptors
        close(counter0FD);
        close(counter1FD);
        close(devFD);

        return 0;
}

int main(int argc, char** argv) {
        int retval = 0;
        uint32_t deviceNum;

        if (argc != 2) {
                usage(argv[0]);
                return 1;
        }

        deviceNum = strtoul(argv[1], NULL, 10);

        retval = gpcTest(deviceNum, 1);
        retval |= gpcTest(deviceNum, 0);
        if (retval) {
                fprintf(stderr, "GPC test/s failed!\n");
                return -1;
        }

        return 0;
}
