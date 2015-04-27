/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/performance_ai_ao_loopback.c $
 * $Id: performance_ai_ao_loopback.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Generate static signal with analog output.
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


#include <pxi-6259-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define DEVICE_FILE "/dev/pxi6259"

void usage(const char *name) {
        printf("This will perform automatic measurement of voltage errors from AO to AI segment.\n"
                "Note that for this test to work a physical connection must be made from AOx to AIx\n"
                "channel [where x is the number of channel, e.g. 0 means that AO0 is connected to AI0]\n"
                "Test will then generate and sample static voltages across desired range.\n\n"
                "Gain values:  1: +/- 10V, 2: +/- 5V, 3: +/- 2V, 4: +/- 1V, 5: +/- 500mV, 6: +/- 200mV, 7: +/- 100mV\n\n"
                "Usage: \n"
                "%s <deviceNumber> <channelNumber> [noOfSamples:10] [start voltage: 0] [end voltage: 10] [voltage step: 0.5] [gain : 1] [interactive : no]\n"
                "Example:\n%s 0 0 1000 0 10 1 1\n",name,name);
}

int generateStaticSignal(uint32_t deviceNum, uint8_t channel, float voltage) {
        char filename[256];
        int channelFD;
        int i;
        pxi6259_ao_conf_t aoConfig;
        int devFD;

        // open AO file descriptor
        sprintf(filename, "%s.%u.ao", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AO configuration
        aoConfig = pxi6259_create_ao_conf();

        // configure AO channel
        if (pxi6259_add_ao_channel(&aoConfig, channel, AO_DAC_POLARITY_BIPOLAR)) {
                fprintf(stderr, "Failed to configure channel %d!\n", i);
                return -1;
        }

        // enable signal generation
        if (pxi6259_set_ao_attribute(&aoConfig, AO_SIGNAL_GENERATION, AO_SIGNAL_GENERATION_STATIC)) {
                fprintf(stderr, "Failed to enable generating static signal!\n");
                return -1;
        }

        // set continuous mode
        if (pxi6259_set_ao_attribute(&aoConfig, AO_CONTINUOUS, 0)) {
                fprintf(stderr, "Failed to set continuous mode!\n");
                return -1;
        }

        // load AO configuration and let it apply
        if (pxi6259_load_ao_conf(devFD, &aoConfig)) {
                fprintf(stderr, "Failed to load configuration!\n");
                return -1;
        }

        // open file descriptor for each AO channel
        sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, channel);
        channelFD = open(filename, O_RDWR | O_NONBLOCK);
        if (channelFD < 0) {
                fprintf(stderr, "Failed to open channel %u: %s\n", i, strerror(errno));
                return -1;
        }

        // start AO segment (signal generation)
        if (pxi6259_start_ao(devFD)) {
                fprintf(stderr, "Failed to start segment!n");
                return -1;
        }

        // put static voltages on AO channels

        int rc = pxi6259_write_ao(channelFD, &voltage, 1);
        if (rc < 0) {
                fprintf(stderr, "Failed to write to AO channel: %u\n", i);
                return -1;
        }


        // stop AO segment
        if (pxi6259_stop_ao(devFD)) {
                fprintf(stderr, "Failed to stop generating signal!\n");
                return -1;
        }

        // close file descriptors
        close(channelFD);
        close(devFD);

        return 0;
}

int sampleChannel(uint32_t deviceNum, uint32_t channel, uint32_t nSamples,  uint8_t gain, float* buffer) {
        char filename[256];
        int i, j;
        int channelFD;
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

        if (pxi6259_add_ai_channel(&aiConfig, channel, AI_POLARITY_BIPOLAR, gain, AI_CHANNEL_TYPE_RSE, 0)) {
                fprintf(stderr, "Failed to configure channel %u!\n", channel);
                return -1;
        }

        // configure number of samples
        if (pxi6259_set_ai_number_of_samples(&aiConfig, nSamples, 0, 0)) {
                fprintf(stderr, "Failed to configure number of samples!\n");
                return -1;
        }

        // configure AI convert clock
        if (pxi6259_set_ai_convert_clk(&aiConfig, 160, 3, AI_CONVERT_SELECT_SI2TC,
                        AI_CONVERT_POLARITY_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI convert clock!\n");
                return -1;
        }

        // configure AI sampling clock
        if (pxi6259_set_ai_sample_clk(&aiConfig, 160, 3, AI_SAMPLE_SELECT_SI_TC,
                        AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI sampling clock!\n");
                return -1;
        }

        // load AI configuration and let it apply
        if (pxi6259_load_ai_conf(devFD, &aiConfig)) {
                fprintf(stderr, "Failed to load configuration!\n");
                return -1;
        }


        sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum, channel);
        channelFD = open(filename, O_RDWR | O_NONBLOCK);
        if (channelFD < 0) {
                fprintf(stderr, "Failed to open channel %u: %s\n", channel, strerror(errno));
                return -1;
        }


//        // wipe any previous samples
//        float buffer_n[nSamples];
//        pxi6259_read_ai(channel, buffer_n, nSamples);

        // start AI segment (data acquisition)
        if (pxi6259_start_ai(devFD)) {
                fprintf(stderr, "Failed to start data acquisition!\n");
                return -1;
        }
        usleep(100);

        // there should be nSamples
        int n = 0;
        while (n < nSamples) {
                // read scaled samples
                int rc = pxi6259_read_ai(channelFD, &buffer[n], nSamples - n);
                if (rc < 0) {
                        if (errno == EAGAIN) {
                                continue;
                        } else {
                                printf("Error while reading from AI!: %s\n",strerror(errno));
                                goto out;
                        }
                }
                n += rc;
        }

        // output samples values
        for (j = 0; j < nSamples; j++) {
                //printf("\t%.03f\n", buffer[j]);
        }

        out:
        // stop AI segment
        if (pxi6259_stop_ai(devFD)) {
                fprintf(stderr, "Failed to stop data acquisition!\n");
        }

        // close file descriptors
        close(channelFD);

        close(devFD);

        return 0;
}

int main(int argc, char** argv) {
        int i;
        float setpointVoltage;
        uint32_t deviceNum;
        uint8_t channelNo;
        uint32_t noOfAiSamples = 10;
        uint8_t gain=1;
        float startVoltage=0;
        float endVoltage=10.0;
        float stepVoltage = 0.5;
        uint8_t interactive = 0;



        if (argc <= 2) {
                usage(argv[0]);
                return 1;
        }

        deviceNum = strtoul(argv[1], NULL, 10);

        channelNo = strtoul(argv[2],NULL,10);

        if(argc > 3)
            noOfAiSamples = strtoul(argv[3],NULL, 10);

        if(argc > 4)
            startVoltage = strtof(argv[4],NULL);

        if(argc > 5)
            endVoltage = strtof(argv[5],NULL);

        if(argc > 6)
            stepVoltage = strtof(argv[6],NULL);

        if(argc > 7)
            gain = strtoul(argv[7],NULL,10);

        if(argc > 8)
            interactive = strtoul(argv[8],NULL,10);

//        float voltages[nVoltages];
//        for (i = 0; i < nVoltages; i++) {
//                voltages[i] = strtof(argv[i + 2], NULL);
//        }
//
//        if (generateStaticSignal(deviceNum, voltages, nVoltages)) {
//                fprintf(stderr, "Error generating static signal!\n");
//                return -1;
//        }

        float aiSampleBuffer[noOfAiSamples];

        for(setpointVoltage=startVoltage;setpointVoltage<=endVoltage;setpointVoltage+=stepVoltage){

            /* Generate signal */
            generateStaticSignal(deviceNum,channelNo,setpointVoltage);
            usleep(1000); //give some time for signal to settle
            /* Sample generated signal */
            sampleChannel(deviceNum,channelNo,noOfAiSamples,gain,aiSampleBuffer);


            float difference = 0;
            float maxDifference=0;
            float averageSample=0;
            float averageDifference=0;
            for(i=0;i<noOfAiSamples;i++){
                difference = setpointVoltage - aiSampleBuffer[i];
                difference = fabs(difference);
                difference*=1000;

                maxDifference = difference > maxDifference ? difference : maxDifference;

                averageSample+=aiSampleBuffer[i];
            }

            averageSample/=noOfAiSamples;
            averageDifference=fabs((setpointVoltage-averageSample)*1000);



            printf("Average sample voltage for set point: %fV is %fV, max difference: %fmV average difference: %fmV\n", setpointVoltage, averageSample, maxDifference,averageDifference);
            if(interactive){
                printf("Press enter to continue with measurements\n");
                getchar();
            }

        }

        return 0;
}
