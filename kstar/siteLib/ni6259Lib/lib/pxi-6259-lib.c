/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/lib/pxi-6259-lib.c $
 * $Id: pxi-6259-lib.c 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : NI PXI-6682 device driver support library.
 *
 * Author        : Janez Golob (Cosylab)
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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>
//#include <linux/errno.h>  /* error codes */
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <pxi-6259-lib.h>
#include <pxi-6259-lib-utils.h>

#include <pxi6259.h>
#include <pxi6259-enums.h>

#define CONFIG_KEY      0xfa0d0dab
#define UDEV_REFRESH_TIMEOUT    10000000  // in us

/* AI cache */
static ai_scaling_coefficients_t ai_coefs[MINOR_NUMBERS/DEVICE_MINOR_STEP];
static pxi6259_ai_conf_t ai_confs[MINOR_NUMBERS/DEVICE_MINOR_STEP];
/* Time of last coefficient transfer */
static unsigned long aichanmtime[MINOR_NUMBERS/DEVICE_MINOR_STEP];
/* If the AI cache is initialized */
static uint8_t ai_initialized = 0;
pthread_mutex_t ai_cache_mutex = PTHREAD_MUTEX_INITIALIZER;

/* AO cache */
static ao_scaling_coefficients_t ao_coefs[MINOR_NUMBERS/DEVICE_MINOR_STEP];
static pxi6259_ao_conf_t ao_confs[MINOR_NUMBERS/DEVICE_MINOR_STEP];
/* Time of last coefficient transfer */
static unsigned long aochanmtime[MINOR_NUMBERS/DEVICE_MINOR_STEP];
/* If the AI cache is initialized */
static uint8_t ao_initialized = 0;
pthread_mutex_t ao_cache_mutex = PTHREAD_MUTEX_INITIALIZER;



void pxi6259_exit_on_signal(int signum);

__must_check int get_ao_scaling_coefficient(int fd, ao_scaling_coefficients_t *coeff);
__must_check int get_ai_scaling_coefficient(int fd, ai_scaling_coefficients_t *coeff);

struct pxi6259_conf {
        union {
                struct pxi6259_ai_conf ai_task;
                struct pxi6259_ao_conf ao_task;
                struct pxi6259_gpc_conf gpc_task;
                struct pxi6259_dio_conf dio_task;
        } conf;
        enum pxi6259_segment segment;
};

__must_check int pxi6259_reset(int fd)
{
    return ioctl(fd, PXI6259_IOC_RESET);
}

__must_check int pxi6259_reset_ai(int fd)
{
    return ioctl(fd, PXI6259_IOC_RESET_AI);

}

__must_check int pxi6259_reset_ao(int fd)
{
    return ioctl(fd, PXI6259_IOC_RESET_AO);
}

__must_check int pxi6259_reset_dio(int fd)
{
    return ioctl(fd, PXI6259_IOC_RESET_DIO);
}

__must_check int pxi6259_reset_gpc(int fd)
{
    return ioctl(fd, PXI6259_IOC_RESET_GPC);
}

/********************************************/
/* CONFIGURATION AND CONTROL FUNCTIONS      */
/********************************************/

pxi6259_ai_conf_t pxi6259_create_ai_conf()
{
        pxi6259_ai_conf_t new_conf;
        int retval = 0;

        memset(&new_conf, 0, sizeof(new_conf));
        new_conf.key = CONFIG_KEY;

        /* Default segment configuration */
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_CONVERT_OUTPUT_SELECT,
                        AI_CONVERT_OUTPUT_SELECT_ACTIVE_LOW);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_SIP_OUTPUT_SELECT,
                        AI_SIP_OUTPUT_SELECT_ACTIVE_HIGH);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_CONVERT_PULSE_WIDTH,
                        AI_CONVERT_PULSE_WIDTH_1_ClOCK_PERIOD);
        /* Default environment configuration */
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_EXTERNAL_MUX_PRESENT,
                        AI_EXTERNAL_MUX_PRESENT_EVERY_CONVERT);
        retval |=  pxi6259_set_ai_attribute(&new_conf,
                        AI_EXTMUX_OUTPUT_SELECT,
                        AI_EXTMUX_OUTPUT_SELECT_GROUND);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_EXTERNAL_GATE_SELECT,
                        AI_EXTERNAL_GATE_SELECT_DISABLED);
        /* Default start trigger configuration */
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_START_SELECT,
                        AI_START_SELECT_PULSE);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_START_POLARITY,
                        AI_START_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE);
        /* Reference trigger configuration */
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_REFERENCE_SELECT,
                        AI_REFERENCE_SELECT_PULSE);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_REFERENCE_POLARITY,
                        AI_REFERENCE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE);
        /* Default stop trigger configuration */
        retval |=  pxi6259_set_ai_attribute(&new_conf,
                        AI_STOP_SELECT,
                        AI_STOP_SELECT_IN);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_STOP_POLARITY,
                        AI_STOP_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE);
        /* Configure number of scans and samples */
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_NO_OF_POST_TRIG_SAMPLES,
                        1);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_NO_OF_PRE_TRIG_SAMPLES,
                        0);
        /* Default sample clock configuration */
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_SAMPLE_SELECT,
                        AI_SAMPLE_SELECT_PULSE);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_SAMPLE_POLARITY,
                        AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_SAMPLE_DELAY_DEVISOR,
                        3);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_SAMPLE_PERIOD_DEVISOR,
                        20);
        /* Default convert clock configuration */
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_CONVERT_SELECT,
                        AI_CONVERT_SELECT_SI2TC);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_CONVERT_POLARITY,
                        AI_CONVERT_POLARITY_FALLING_EDGE);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                        AI_CONVERT_DELAY_DEVISOR,
                        3);
        retval |= pxi6259_set_ai_attribute(&new_conf,
                       AI_CONVERT_PERIOD_DEVISOR,
                       20);
        if (retval)
                printf("err: Create AI config!\n");

        return new_conf;
}

pxi6259_ao_conf_t pxi6259_create_ao_conf()
{
        pxi6259_ao_conf_t new_conf;
        int retval;

        memset(&new_conf, 0, sizeof(new_conf));
        new_conf.key = CONFIG_KEY;

        retval = pxi6259_set_ao_attribute(&new_conf,
                        AO_SIGNAL_GENERATION, AO_SIGNAL_GENERATION_STATIC);
        retval |= pxi6259_set_ao_attribute(&new_conf,
                        AO_NUMBER_OF_BUFFERS, 1);
        retval |= pxi6259_set_ao_attribute(&new_conf,
                        AO_NUMBER_OF_SAMPLES, 1);
        retval |= pxi6259_set_ao_attribute(&new_conf,
                        AO_CONTINUOUS, 0);
        return new_conf;
}

pxi6259_dio_conf_t pxi6259_create_dio_conf()
{
        pxi6259_dio_conf_t new_conf;
        memset(&new_conf, 0, sizeof(new_conf));
        new_conf.key = CONFIG_KEY;
        return new_conf;
}

pxi6259_gpc_conf_t pxi6259_create_gpc_conf()
{
        pxi6259_gpc_conf_t new_conf;
        int retval;

        memset(&new_conf, 0, sizeof(new_conf));
        new_conf.key = CONFIG_KEY;

        retval = pxi6259_set_gpc_attribute(&new_conf,
                        GPC_SOURCE_SELECT,
                        GPC_SOURCE_SELECT_TIMEBASE1);
        retval |= pxi6259_set_gpc_attribute(&new_conf,
                        GPC_COUNT_DIRECTION,
                        GPC_COUNT_DIRECTION_SOFTWARE_DOWN);
        retval |= pxi6259_set_gpc_attribute(&new_conf,
                        GPC_SELECT_TYPE,
                        GPC_TASK_TYPE_SIMPLE_COUNTER);
        return new_conf;
}

static inline void absolute_time(unsigned int delta_us, struct timeval *abstime) {
    gettimeofday(abstime, NULL);
    abstime->tv_sec += delta_us / 1000000;
    abstime->tv_usec += delta_us % 1000000;
}

static inline int timeval_compare(struct timeval *t1, struct timeval *t2) {
    if (t1->tv_sec < t2->tv_sec)
        return -1;
    if (t1->tv_sec > t2->tv_sec)
        return 1;
    if (t1->tv_usec < t2->tv_usec)
        return -1;
    if (t1->tv_usec > t2->tv_usec)
        return 1;

    return 0;
}


static int _wait_for_device_file(int fd, const char *chantype, int chanid, struct timeval *wait_until) {
        const useconds_t recheck_timeout = 1000; // in us
        uint32_t devid;
        int retval;
        char devfile[256];
        struct timeval now;

        // Compile device file name
        retval = ioctl(fd, PXI6259_IOC_GET_DEVICE_ID, &devid);
        if (retval) return -1;
        snprintf(devfile, sizeof(devfile), "/dev/pxi6259.%d.%s.%d", devid, chantype, chanid);

        do {
            struct stat buf;
            retval = stat(devfile, &buf);
            if (retval == 0) {
#if 0	//TODO : YSW commented out checking permissions because some system doesn't grant rw permission for others in default
                // This check comes from bug 3722 investigation. When the system load is high,
                // udev might be slow in applying 0666 mode to device files and
                // the files could not be used (by non-root at least).
                if (buf.st_mode & (S_IROTH | S_IWOTH))
                    return 0;
#else
				return 0;
#endif

                // retry otherwise
            }
            else if (errno != ENOENT)
                return -1;

            usleep(recheck_timeout);

            gettimeofday(&now, NULL);
        } while (timeval_compare(&now, wait_until) == -1);

        errno = ETIME;
        return -1;
}

// Generic wait_for_device supports ai,ao,dio conf types - emulate C++ templates
#define wait_for_device_file(fd, conf, type, timeout) \
        { \
                int i; \
                for (i=0; i<sizeof(conf->channel)/sizeof(struct pxi6259_##type##_channel); i++) { \
                        if (conf->channel[i].enabled && _wait_for_device_file(fd, #type, i, timeout) == -1) \
                                return -1; \
                } \
                return 0; \
        }

__must_check int pxi6259_load_ai_conf(int fd, pxi6259_ai_conf_t *conf)
{
        int retval = 0;
        struct timeval timeout;

        PDEBUG("Loading AI configuration...\n");

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_LOAD_AI_CONF, conf);
        if (retval) return -1;

        absolute_time(UDEV_REFRESH_TIMEOUT, &timeout);
        wait_for_device_file(fd, conf, ai, &timeout);
}

__must_check int pxi6259_load_ao_conf(int fd, pxi6259_ao_conf_t* conf)
{
        int retval;
        struct timeval timeout;

        PDEBUG("Loading AO configuration...\n");

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_LOAD_AO_CONF, conf);
        if (retval) return -1;

        absolute_time(UDEV_REFRESH_TIMEOUT, &timeout);
        wait_for_device_file(fd, conf, ao, &timeout);
}

__must_check int pxi6259_load_dio_conf(int fd, pxi6259_dio_conf_t *conf)
{
        int retval;
        struct timeval timeout;

        PDEBUG("Loading DIO configuration...\n");

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_LOAD_DIO_CONF, conf);
        if (retval) return -1;

        absolute_time(UDEV_REFRESH_TIMEOUT, &timeout);
        wait_for_device_file(fd, conf, dio, &timeout);
}

__must_check int pxi6259_load_gpc0_conf(int fd, pxi6259_gpc_conf_t *conf)
{
        int retval;
        struct timeval timeout;

        PDEBUG("Loading GPC0 configuration...\n");

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_LOAD_GPC0_CONF, conf);
        if (retval) return -1;

        absolute_time(UDEV_REFRESH_TIMEOUT, &timeout);
        return _wait_for_device_file(fd, "gpc", 0, &timeout);
}

__must_check int pxi6259_load_gpc1_conf(int fd, pxi6259_gpc_conf_t *conf)
{
        int retval;
        struct timeval timeout;

        PDEBUG("Loading GPC1 configuration...\n");

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_LOAD_GPC1_CONF, conf);
        if (retval) return -1;

        absolute_time(UDEV_REFRESH_TIMEOUT, &timeout);
        return _wait_for_device_file(fd, "gpc", 1, &timeout);
}

__must_check int pxi6259_read_ai_conf(int fd, pxi6259_ai_conf_t *conf)
{
        int retval;

        PDEBUG("Reading AI configuration...\n");

        if (conf == NULL) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_READ_AI_CONF, conf);
        return retval;
}

__must_check int pxi6259_read_ao_conf(int fd, pxi6259_ao_conf_t *conf)
{
        int retval;

        PDEBUG("Reading AO configuration...\n");

        if (conf == NULL) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_READ_AO_CONF, conf);
        return retval;
}

__must_check int pxi6259_read_dio_conf(int fd, pxi6259_dio_conf_t *conf)
{
        int retval;

        PDEBUG("Reading DIO configuration...\n");

        if (conf == NULL) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_READ_DIO_CONF, conf);
        return retval;
}

__must_check int pxi6259_read_gpc0_conf(int fd, pxi6259_gpc_conf_t *conf)
{
        int retval;

        PDEBUG("Reading GPC0 configuration...\n");

        if (conf == NULL) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_READ_GPC0_CONF, conf);
        return retval;
}

__must_check int pxi6259_read_gpc1_conf(int fd, pxi6259_gpc_conf_t *conf)
{
        int retval;

        PDEBUG("Reading AI configuration...\n");

        if (conf == NULL) {
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_READ_GPC1_CONF, conf);
        return retval;
}



__must_check int pxi6259_set_ai_attribute(pxi6259_ai_conf_t *ai_conf,
                enum pxi6259_segment_attribute attribute, ...)
{
        /* This variable handles arguments */
        va_list list_ptr;
        uint32_t arg = 0;

        va_start(list_ptr, attribute);

        if (ai_conf == NULL || ai_conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        PDEBUG("Setting AI segment attribute %d!\n", attribute);

        switch (attribute) {
          case AI_NO_OF_PRE_TRIG_SAMPLES :
                ai_conf->aiNoOfPreTrigSamples = va_arg(list_ptr, uint32_t);
                if (ai_conf->aiNoOfPostTrigSamples >= 1 || ai_conf->aiNoOfPreTrigSamples >= 1) {
                        //ai_conf->aiArmDIV = 1;
                        ai_conf->aiEndOnEndOfScan = 1;
                } else {
                     //   ai_conf->aiArmDIV = 1;
                        ai_conf->aiEndOnEndOfScan = 0;
                }
                break;
          case AI_NO_OF_POST_TRIG_SAMPLES :
                ai_conf->aiNoOfPostTrigSamples = va_arg(list_ptr, uint32_t);
                if (ai_conf->aiNoOfPostTrigSamples >= 1 || ai_conf->aiNoOfPreTrigSamples >= 1) {
                       // ai_conf->aiArmDIV = 1;
                        ai_conf->aiEndOnEndOfScan = 1;
                } else {
                       // ai_conf->aiArmDIV = 1;
                        ai_conf->aiEndOnEndOfScan = 0;
                }
                break;
          case AI_CONTINUOUS :
                  arg = va_arg(list_ptr, ai_continuous_t);
                  if(arg > AI_CONTINUOUS_ENABLE){
                          PDEBUG ("Error: Invalid AI Segment Argument: continuous\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiContinuous = arg;
                  break;
          case AI_EXTERNAL_CONVERT_CLOCK :
                  arg = (uint8_t)va_arg(list_ptr, uint32_t);
                  if(arg > 1){
                          PDEBUG ("Error: Invalid AI Segment Argument: external convert clock\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiExternalConvertClock = arg;
                  break;
          case AI_SAMPLE_PERIOD_DEVISOR :
                  arg = va_arg(list_ptr, uint32_t);
                  if(arg < 10){
                          PDEBUG ("Error: Invalid AI Segment Argument: sample period divisor too short\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiSamplePeriodDivisor = arg;
                  break;
          case AI_SAMPLE_DELAY_DEVISOR :
                  arg = va_arg(list_ptr, uint32_t);
                  if(arg < 3){
                          PDEBUG ("Error: Invalid AI Segment Argument: sample delay divisor too short\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiSampleDelayDivisor = arg;
                  break;
          case AI_CONVERT_PERIOD_DEVISOR :
                  arg = va_arg(list_ptr, uint32_t);
                  if(arg < 16){
                          PDEBUG ("Error: Invalid AI Segment Argument: convert period divisor too short\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiConvertPeriodDivisor = arg;
                  break;
          case AI_CONVERT_DELAY_DEVISOR :
                  arg = va_arg(list_ptr, uint32_t);
                  if(arg < 3){
                          PDEBUG ("Error: Invalid AI Segment Argument: convert delay divisor too short\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiConvertDelayDivisor = arg;
                  break;
          case AI_CONVERT_OUTPUT_SELECT :
                  arg = va_arg(list_ptr, ai_convert_output_select_t);
                  if(arg > AI_CONVERT_OUTPUT_SELECT_ACTIVE_HIGH){
                          PDEBUG ("Error: Invalid AI Segment Argument: convert output select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiConOutputSelect = arg;
                  break;
          case AI_SIP_OUTPUT_SELECT :
                  arg = va_arg(list_ptr, ai_sip_output_select_t);
                  if(arg > AI_SIP_OUTPUT_SELECT_ACTIVE_HIGH){
                          PDEBUG ("Error: Invalid AI Segment Argument: SIP output select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiScanInProgOutputSelect = arg;
                  break;
          case AI_CONVERT_PULSE_WIDTH :
                  arg = va_arg(list_ptr, ai_convert_pulse_width_t);
                  if(arg > AI_CONVERT_PULSE_WIDTH_1_ClOCK_PERIOD){
                          PDEBUG ("Error: Invalid AI Segment Argument: convert pulse width\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiConPulseWidth = arg;
                  break;
          case AI_EXTERNAL_MUX_PRESENT :
                  arg = va_arg(list_ptr, ai_external_mux_present_t);
                  if(arg > AI_EXTERNAL_MUX_PRESENT_DIV_TC_CONVERTS){
                          PDEBUG ("Error: Invalid AI Segment Argument: external mux presesnt\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiExtMUXpresent = arg;
                  break;
          case AI_EXTMUX_OUTPUT_SELECT :
                  arg = va_arg(list_ptr, ai_extmux_output_select_t);
                  if(arg > AI_EXTMUX_OUTPUT_SELECT_ACTIVE_HIGH){
                          PDEBUG ("Error: Invalid AI Segment Argument: extmux output select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiExtMUXCLKoutputSelect = arg;
                  break;
          case AI_EXTERNAL_GATE_SELECT :
                  arg = va_arg(list_ptr, ai_external_gate_select_t);
                  if(arg > AI_EXTERNAL_GATE_SELECT_LOW){
                          PDEBUG ("Error: Invalid AI Segment Argument: external gate select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiExtGateSelect = arg;
                  break;
          case AI_START_SELECT :
                  arg = va_arg(list_ptr, ai_start_select_t);
                  if(arg > AI_START_SELECT_LOW){
                          PDEBUG ("Error: Invalid AI Segment Argument: start select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiStartSource = arg;
                  break;
          case AI_START_POLARITY :
                  arg = va_arg(list_ptr, ai_start_polarity_t);
                  if(arg > AI_START_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE){
                          PDEBUG ("Error: Invalid AI Segment Argument: start polarity\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiStartPolarity = arg;
                  break;
          case AI_REFERENCE_SELECT :
                  arg = va_arg(list_ptr, ai_reference_select_t);
                  if(arg > AI_REFERENCE_SELECT_LOW){
                          PDEBUG ("Error: Invalid AI Segment Argument: reference select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiRefSource = arg;
                  /* TODO: why always true? */
                  ai_conf->aiArmSC = kAI_Arm_SC_Enable;
                  break;
          case AI_REFERENCE_POLARITY :
                  arg = va_arg(list_ptr, ai_reference_polarity_t);
                  if(arg > AI_REFERENCE_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE){
                          PDEBUG ("Error: Invalid AI Segment Argument: reference polarity\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiRefPolarity = arg;
                  break;
          case AI_STOP_SELECT :
                  arg = va_arg(list_ptr, ai_stop_select_t);
                  if(arg > AI_STOP_SELECT_LOW){
                          PDEBUG ("Error: Invalid AI Segment Argument: stop select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiStopSource = arg;
                  break;
          case AI_STOP_POLARITY :
                  arg = va_arg(list_ptr, ai_stop_polarity_t);
                  if(arg > AI_STOP_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE){
                          PDEBUG ("Error: Invalid AI Segment Argument: stop polarity\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiStopPolarity = arg;
                  break;
          case AI_SAMPLE_SELECT :
                  arg = va_arg(list_ptr, ai_sample_select_t);
                  if(arg > AI_SAMPLE_SELECT_LOW){
                          PDEBUG ("Error: Invalid AI Segment Argument: sample select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiSampleClkSource = arg;
                  if (ai_conf->aiSampleClkSource == AI_SAMPLE_SELECT_SI_TC) {
                          ai_conf->aiArmSI = kAI_Arm_SI_Enable;
                  } else {
                          ai_conf->aiArmSI = kAI_Arm_SI_Disable;
                  }
                break;
          case AI_SAMPLE_POLARITY :
                  arg = va_arg(list_ptr, ai_sample_polarity_t);
                  if(arg > AI_SAMPLE_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE){
                          PDEBUG ("Error: Invalid AI Segment Argument: sample polarity\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiSampleClkPolarity = arg;
                  break;
          case AI_CONVERT_SELECT :
                  arg = va_arg(list_ptr, ai_convert_select_t);
                  if(arg > AI_CONVERT_SELECT_LOW){
                          PDEBUG ("Error: Invalid AI Segment Argument: convert select\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiConvertClkSource = arg;
                  if (ai_conf->aiConvertClkSource == AI_CONVERT_SELECT_SI2TC) {
                          ai_conf->aiArmSI2 = kAI_Arm_SI2_Enable;
                  } else {
                          ai_conf->aiArmSI2 = kAI_Arm_SI2_Disable;
                  }
                  break;
          case AI_CONVERT_POLARITY :
                  arg = va_arg(list_ptr, ai_convert_polarity_t);
                  if(arg > AI_CONVERT_POLARITY_RISING_EDGE){
                          PDEBUG ("Error: Invalid AI Segment Argument: convert polarity\n");
                          errno = EINVAL; return -1;
                  }
                  ai_conf->aiConvertClkPolarity = arg;
                  break;
          default:
                  errno = EINVAL; return -1;
        }

        /* Clean-up arg list */
        va_end(list_ptr);

        return 0;
}

__must_check int pxi6259_set_ao_attribute(pxi6259_ao_conf_t *ao_conf,
                enum pxi6259_segment_attribute attribute, ...)
{
        /* This variable handles arguments */
        va_list list_ptr;
        uint32_t arg = 0;

        va_start(list_ptr, attribute);

        if (ao_conf == NULL || ao_conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        switch (attribute) {
          case AO_SIGNAL_GENERATION :
                  arg = va_arg(list_ptr, ao_signal_generation_t);
                  if(arg > AO_SIGNAL_GENERATION_WAVEFORM) {
                          PDEBUG ("Error: Invalid AO Segment Argument: signal generation\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->signalGeneration = arg;
                  break;
          case AO_CONTINUOUS :
                  arg = va_arg(list_ptr, ao_continuous_t);
                  if(arg > AO_CONTINUOUS_IGNORE_BC_TC) {
                          PDEBUG ("Error: Invalid AO Segment Argument: continuous\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->continuous = arg;
                  break;
          case AO_START1_SOURCE_SELECT :
                  arg = va_arg(list_ptr, ao_start1_select_t);
                  if(arg > AO_START1_SELECT_LOW) {
                          PDEBUG ("Error: Invalid AO Segment Argument: start1 source select\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->start1Source = arg;
                  break;
          case AO_START1_POLARITY_SELECT :
                  arg = va_arg(list_ptr, ao_start1_polarity_t);
                  if(arg > AO_START1_POLARITY_FALLING_EDGE) {
                          PDEBUG ("Error: Invalid AO Segment Argument: start1 polarity select\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->start1Polarity = arg;
                  break;
          case AO_UPDATE_SOURCE_SELECT :
                  arg = va_arg(list_ptr, ao_update_source_select_t);
                  if(arg > AO_UPDATE_SOURCE_SELECT_LOW) {
                          PDEBUG ("Error: Invalid AO Segment Argument: update source select\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->updateSource = arg;
                  break;
          case AO_UPDATE_POLARITY_SELECT :
                  arg = va_arg(list_ptr, ao_update_source_polarity_t);
                  if(arg > AO_UPDATE_SOURCE_POLARITY_FALLING_EDGE) {
                          PDEBUG ("Error: Invalid AO Segment Argument: update polarity select\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->updatePolarity = arg;
                  break;
          case AO_UI_SOURCE_SELECT :
                  arg = va_arg(list_ptr, ao_ui_source_select_t);
                  if(arg > AO_UI_SOURCE_SELECT_LOW) {
                          PDEBUG ("Error: Invalid AO Segment Argument: ui source source select\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->uiSourceSelect = arg;
                  break;
          case AO_UI_SOURCE_POLARITY_SELECT :
                  arg = va_arg(list_ptr, ao_ui_source_polarity_t);
                  if(arg > AO_UI_SOURCE_POLARITY_FALLING_EDGE) {
                          PDEBUG ("Error: Invalid AO Segment Argument: ui source polarity select\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->uiSourcePolarity = arg;
                  break;
          case AO_UPDATE_PERIOD_DIVISOR :
                  ao_conf->updatePeriodDivisor = va_arg(list_ptr, uint32_t);
                  break;
          case AO_NUMBER_OF_SAMPLES :
                  ao_conf->numberOfSamples = va_arg(list_ptr, uint32_t);
                  break;
          case AO_NUMBER_OF_BUFFERS :
                  ao_conf->numberOfBuffers = va_arg(list_ptr, uint32_t);
                  break;
          case AO_FIFO_RETRANSMIT :
                  arg = va_arg(list_ptr, ao_fifo_retransmit_t);
                  if(arg > AO_FIFO_RETRANSMIT_ENABLE) {
                          PDEBUG ("Error: Invalid AO Segment Argument: fifo retransmit\n");
                          errno = EINVAL; return -1;
                  }
                  ao_conf->fifoRetransmitEnable = arg;
                  break;
          default:
                  errno = EINVAL; return -1;
        }

        /* Clean-up arg list */
        va_end(list_ptr);

        return 0;
}

__must_check int pxi6259_set_dio_attribute(pxi6259_dio_conf_t *conf,
                enum pxi6259_segment_attribute attribute, ...)
{
        errno = EINVAL; return -1;
}

__must_check int pxi6259_set_gpc_attribute(pxi6259_gpc_conf_t *gpc_conf,
                enum pxi6259_segment_attribute attribute, ...)
{
        /* This variable handles arguments */
        va_list list_ptr;
        uint32_t arg = 0;

        va_start(list_ptr, attribute);

        if (gpc_conf == NULL || gpc_conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        switch (attribute) {
          case GPC_INITIAL_VALUE:
                  gpc_conf->initialValue = va_arg(list_ptr, uint32_t);
                  break;
          case GPC_SOURCE_SELECT:
                  arg = va_arg(list_ptr, gpc_source_select_t);
                  if(arg > GPC_SOURCE_SELECT_LOGIC_LOW) {
                          PDEBUG ("Error: Invalid GPC Segment Argument: source select\n");
                          errno = EINVAL; return -1;
                  }
                  gpc_conf->source = arg;
                  break;
          case GPC_COUNT_DIRECTION:
                  arg = va_arg(list_ptr, gpc_count_direction_t);
                  if(arg > GPC_COUNT_DIRECTION_HARDWARE_GATE) {
                          PDEBUG ("Error: Invalid GPC Segment Argument: count direction\n");
                          errno = EINVAL; return -1;
                  }
                  gpc_conf->countDirection = arg;
                  break;
          case GPC_SELECT_TYPE:
                gpc_conf->gpcInfo = va_arg(list_ptr, gpc_task_type_t);
                break;
          case GPC_GATE_SELECT:
                  arg = va_arg(list_ptr, gpc_gate_select_t);
                  if(arg > GPC_GATE_SELECT_LOGIC_LOW) {
                          PDEBUG ("Error: Invalid GPC Segment Argument: gate select\n");
                          errno = EINVAL; return -1;
                  }
                  gpc_conf->gateSource = arg;
                  break;
          default:
                  errno = EINVAL; return -1;
        }

        /* Clean-up arg list */
        va_end(list_ptr);

        return 0;
}

__must_check int pxi6259_get_ai_attribute(pxi6259_ai_conf_t *ai_conf,
                enum pxi6259_segment_attribute attribute, ...)
{
        return -1;
}

__must_check int pxi6259_set_ai_number_of_samples(pxi6259_ai_conf_t *conf,
                uint32_t post_trigger_samples, uint32_t pre_trigger_samples, uint8_t continuous)
{
        int retval;

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = pxi6259_set_ai_attribute(conf,
                        AI_NO_OF_POST_TRIG_SAMPLES, post_trigger_samples);
        if (retval) return retval;

        retval = pxi6259_set_ai_attribute(conf,
                        AI_NO_OF_PRE_TRIG_SAMPLES, pre_trigger_samples);
        if (retval) return retval;

        retval = pxi6259_set_ai_attribute(conf, AI_CONTINUOUS, continuous);
        return retval;
}

__must_check int pxi6259_set_ai_convert_clk(pxi6259_ai_conf_t *conf, uint32_t period_divisor,
                uint32_t delay_divisor, ai_convert_select_t source, ai_convert_polarity_t polarity)
{
        int retval;

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        if (source > AI_CONVERT_SELECT_LOW) {
                errno = EINVAL; return -1;
        }

        if (polarity > AI_CONVERT_POLARITY_RISING_EDGE) {
                errno = EINVAL; return -1;
        }

        retval = pxi6259_set_ai_attribute(conf, AI_CONVERT_SELECT, source);
        if (retval) return retval;

        retval = pxi6259_set_ai_attribute(conf, AI_CONVERT_POLARITY, polarity);
        if (retval) return retval;

        retval = pxi6259_set_ai_attribute(conf,
                        AI_CONVERT_DELAY_DEVISOR, delay_divisor);
        if (retval) return retval;

        retval = pxi6259_set_ai_attribute(conf,
                       AI_CONVERT_PERIOD_DEVISOR, period_divisor);
        return retval;

}

__must_check int pxi6259_set_ai_sample_clk(pxi6259_ai_conf_t *conf, uint32_t period_divisor,
                uint32_t delay_divisor, ai_sample_select_t source, ai_sample_polarity_t polarity)
{
        int retval;

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = pxi6259_set_ai_attribute(conf,
                        AI_SAMPLE_SELECT, source);
        if (retval) return retval;

        retval = pxi6259_set_ai_attribute(conf,
                        AI_SAMPLE_POLARITY, polarity);
        if (retval) return retval;

        retval = pxi6259_set_ai_attribute(conf,
                        AI_SAMPLE_DELAY_DEVISOR, delay_divisor);
        if (retval) return retval;

        retval = pxi6259_set_ai_attribute(conf,
                        AI_SAMPLE_PERIOD_DEVISOR, period_divisor);
        return retval;
}


__must_check int pxi6259_generate_ai_pulse(int fd, uint32_t num_pulses)
{
        return ioctl(fd, PXI6259_IOC_GENERATE_PULSE, &num_pulses);

}


__must_check int pxi6259_set_ao_count(pxi6259_ao_conf_t *conf, uint32_t number_of_samples,
                uint32_t number_of_buffers, uint8_t continuous)
{
        int retval;

        if (conf == NULL || conf->key != CONFIG_KEY) {
                return -PXI6259_ERROR_INVALID_ARGUMENT;
        }

        if (number_of_samples == 0) {
                PDEBUG ("Number of samples is zero!\n");
                errno = EINVAL; return -1;
        }

        if (number_of_buffers == 0) {
                PDEBUG ("Number of buffers is zero!\n");
                errno = EINVAL; return -1;
        }

        retval = pxi6259_set_ao_attribute(conf, AO_NUMBER_OF_SAMPLES, number_of_samples);
        if (retval) return retval;
        retval = pxi6259_set_ao_attribute(conf, AO_NUMBER_OF_BUFFERS, number_of_buffers);
        if (retval) return retval;
        retval = pxi6259_set_ao_attribute(conf, AO_CONTINUOUS, continuous);
        if (retval) return retval;

        return 0;
}

__must_check int pxi6259_set_ao_update_clk(pxi6259_ao_conf_t *conf, ao_update_source_select_t source,
                ao_update_source_polarity_t polarity, uint32_t period_devisor)
{
        int retval;

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = pxi6259_set_ao_attribute(conf, AO_UPDATE_SOURCE_SELECT, source);
        if (retval) return retval;
        retval = pxi6259_set_ao_attribute(conf, AO_UPDATE_POLARITY_SELECT, polarity);
        if (retval) return retval;
        retval = pxi6259_set_ao_attribute(conf, AO_UPDATE_PERIOD_DIVISOR, period_devisor);
        if (retval) return retval;

        return 0;
}

__must_check int pxi6259_set_ao_waveform_generation(pxi6259_ao_conf_t *conf, uint8_t retransmit)
{
        int retval;

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        retval = pxi6259_set_ao_attribute(conf, AO_FIFO_RETRANSMIT, retransmit);
        if (retval) return retval;

        conf->signalGeneration = AO_SIGNAL_GENERATION_WAVEFORM;
        conf->continuous = AO_CONTINUOUS_IGNORE_BC_TC;
        conf->start1Source = AO_START1_SELECT_PULSE;
        conf->start1Polarity = AO_START1_POLARITY_RISING_EDGE;
        conf->updateSource = AO_UPDATE_SOURCE_SELECT_UI_TC;
        conf->updatePolarity = AO_UPDATE_SOURCE_POLARITY_RISING_EDGE;
        conf->uiSourceSelect = AO_UI_SOURCE_SELECT_IN_TIMEBASE1;
        conf->uiSourcePolarity = AO_UI_SOURCE_POLARITY_RISING_EDGE;
        conf->fifoRetransmitEnable = retransmit;

        return 0;
}


/******************************************************/
/***        Segment Manipulation                    ***/
/******************************************************/

__must_check int pxi6259_start_ai(int fd)
{
        return ioctl(fd, PXI6259_IOC_START_AI);
}

__must_check int pxi6259_start_ao(int fd)
{
        return ioctl(fd, PXI6259_IOC_START_AO);
}

__must_check int pxi6259_start_dio(int fd)
{
        return 0;
}

__must_check int pxi6259_start_gpc0(int fd)
{
        return ioctl(fd, PXI6259_IOC_START_GPC0);
}

__must_check int pxi6259_start_gpc1(int fd)
{
        return ioctl(fd, PXI6259_IOC_START_GPC1);
}


__must_check int pxi6259_stop_ai(int fd)
{
        return ioctl(fd, PXI6259_IOC_STOP_AI);
}

__must_check int pxi6259_stop_ao(int fd)
{
        return ioctl(fd, PXI6259_IOC_STOP_AO);
}

__must_check int pxi6259_stop_dio(int fd)
{
        return 0;
}

__must_check int pxi6259_stop_gpc0(int fd)
{
        return ioctl(fd, PXI6259_IOC_STOP_GPC0);
}

__must_check int pxi6259_stop_gpc1(int fd)
{
        return ioctl(fd, PXI6259_IOC_STOP_GPC1);
}


/* CHANNEL CONFIGURATION AND CREATION FUNCTIONS */

__must_check int pxi6259_add_ai_channel(pxi6259_ai_conf_t *ai_conf, uint8_t channel,
                ai_polarity_t polarity, uint8_t gain, ai_channel_type_t channel_type, uint8_t dither)
{
       PDEBUG("Creating AI channel %d...\n", channel);

       if (ai_conf == NULL || ai_conf->key != CONFIG_KEY) {
               errno = EINVAL; return -1;
       }

       if (channel > PXI6259_MAX_AI_CHANNEL) {
               PDEBUG ("Wrong channel for AI\n");
               errno = EINVAL; return -1;
       }

       if (polarity > AI_POLARITY_UNIPOLAR) {
               errno = EINVAL; return -1;
       }

       if (gain > 7) {
               errno = EINVAL; return -1;
       }

       if ((channel_type > AI_CHANNEL_TYPE_RSE) && (channel_type != AI_CHANNEL_TYPE_AUX
                       || channel_type != AI_CHANNEL_TYPE_GHOST)) {
               errno = EINVAL; return -1;
       }

       /* There should be two terminals taken */
       if (channel_type == AI_CHANNEL_TYPE_DIFFERENTIAL) {
               if ((channel >= 8 && channel <= 15)
                               || (channel >= 24 && channel <= 31)) {
                       errno = EINVAL; return -1;
               }

               if (ai_conf->channel_mask & (((uint64_t)1) << (channel + 8)))
                       return -1;
       } else {
               if ((channel >= 8 && channel <= 15)
                               || (channel >= 24 && channel <= 31)) {
                       if ((ai_conf->channel_mask & (((uint64_t)1) << (channel - 8)))
                                       && (ai_conf->channel[channel - 8].channelType
                                                       == AI_CHANNEL_TYPE_DIFFERENTIAL))
                               return -1;
               }
       }

       ai_conf->channel[channel].terminal = channel;
       /* Set some default args */
       ai_conf->channel[channel].channelType = channel_type;
       ai_conf->channel[channel].polarity = polarity;
       ai_conf->channel[channel].gain = gain;
       ai_conf->channel[channel].dither = dither;

       /* Take a slot */
       ai_conf->channel_mask |= (((uint64_t)1) << channel);
       ai_conf->channel[channel].enabled = 1;

       return 0;
}

__must_check int pxi6259_remove_ai_channel(pxi6259_ai_conf_t *ai_conf, uint8_t channel)
{
        if (ai_conf == NULL || ai_conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }
        if (channel > PXI6259_MAX_AI_CHANNEL) {
                errno = EINVAL; return -1;
        }

        ai_conf->channel_mask &= ~(((uint64_t)1) << channel);
        ai_conf->channel[channel].enabled = 0;
        memset(&ai_conf->channel[channel], 0, sizeof(ai_conf->channel[channel]));

        return 0;
}

__must_check int pxi6259_add_ao_channel(pxi6259_ao_conf_t *ao_conf,
                uint8_t channel, ao_dac_polarity_t polarity)
{
        PDEBUG("Creating AO channel %d...\n", channel);

        if (ao_conf == NULL || ao_conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }
        if ((polarity < AO_DAC_POLARITY_UNIPOLAR) || (polarity > AO_DAC_POLARITY_BIPOLAR)) {
                errno = EINVAL; return -1;
        }
        if (channel > PXI6259_MAX_AO_CHANNEL) {
                errno = EINVAL; return -1;
        }

        /* Take a slot */
        ao_conf->channel[channel].enabled = 1;

        if (ao_conf->signalGeneration == AO_SIGNAL_GENERATION_WAVEFORM) {
                ao_conf->channel[channel].polarity = polarity;
                ao_conf->channel[channel].referenceSelect = 0;
                ao_conf->channel[channel].offsetSelect = 0;
                ao_conf->channel[channel].updateMode = AO_UPDATE_MODE_TIMED; // AO_UPDATE_MODE_IMMEDIATE;AO_UPDATE_MODE_TIMED
        } else {
                ao_conf->channel[channel].polarity = polarity;
                ao_conf->channel[channel].referenceSelect = 0;
                ao_conf->channel[channel].offsetSelect = 0;
                ao_conf->channel[channel].updateMode = AO_UPDATE_MODE_IMMEDIATE;
        }

        return 0;
}

__must_check int pxi6259_remove_ao_channel(pxi6259_ao_conf_t *ao_conf, uint8_t channel)
{
        PDEBUG("Removing AO channel %d...\n", channel);

        if (ao_conf == NULL || ao_conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        if (channel > PXI6259_MAX_AO_CHANNEL) {
                errno = EINVAL; return -1;
        }

        ao_conf->channel[channel].enabled = 0;
        memset(&ao_conf->channel[channel], 0, sizeof(ao_conf->channel[channel]));

        return 0;
}

__must_check int pxi6259_add_dio_channel(pxi6259_dio_conf_t *dio_conf,
                uint8_t port_number, uint32_t port_mask)
{
        PDEBUG("Adding DIO PORT%d...\n", port_number);

        if (dio_conf == NULL || dio_conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }
        if (port_number > PXI6259_MAX_DIO_PORT) {
                errno = EINVAL; return -1;
        }
        if ((port_number > 0) && (port_mask > 0xFF)) {
                errno = EINVAL; return -1;
        }

        dio_conf->channel[port_number].enabled = 1;
        dio_conf->channel[port_number].mask = port_mask;

        return 0;
}

__must_check int pxi6259_remove_dio_channel(pxi6259_dio_conf_t *dio_conf, uint8_t port_number)
{
        PDEBUG("Removing DIO PORT%d...\n", port_number);

        if (dio_conf == NULL || dio_conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        if (port_number > PXI6259_MAX_DIO_PORT) {
                errno = EINVAL; return -1;
        }

        dio_conf->channel[port_number].enabled = 0;
        memset(&dio_conf->channel[port_number], 0, sizeof(dio_conf->channel[port_number]));

        return 0;
}


/* SIGNAL ROUTING */

__must_check int pxi6259_connect_trigger_terminals(int fd,
                int src_terminal, int dest_terminal, int32_t signal_modifiers)
{
        return -1;
}


__must_check int pxi6259_disconnect_trigger_terminals(int fd,
                int src_terminal, int dest_terminal)
{
        struct pxi6259_trigger_connection trigger_connection;

        trigger_connection.source = src_terminal;
        trigger_connection.destination = dest_terminal;

        return ioctl(fd, PXI6259_IOC_DISCONNECT_TRIGGER, &trigger_connection);
}


/**
 * TODO:
 *      -check destination terminal
 */
__must_check int pxi6259_export_ai_signal(pxi6259_ai_conf_t *conf,
                enum pxi6259_signal signal, enum pxi6259_terminal dest_terminal)
{
        PDEBUG("Exporting AI signal %d to terminal %d...\n", signal, dest_terminal);

        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        switch(signal) {
          case PXI6259_AI_SAMPLE_CLOCK :
                conf->aiExportSampleClockSig = dest_terminal;
                break;
          case PXI6259_AI_CONVERT_CLOCK :
                conf->aiExportConvertClockSig = dest_terminal;
                break;
          case PXI6259_AI_START_TRIGGER :
                conf->aiExportStartTrigSig = dest_terminal;
                break;
          case PXI6259_AI_REFERENCE_TRIGGER :
                conf->aiExportReferenceTrigSig = dest_terminal;
                break;
          default:
                errno = EINVAL; return -1;
        }


        return 0;
}

// TODO implement this
__must_check int pxi6259_export_ao_signal(pxi6259_ao_conf_t *conf,
                enum pxi6259_signal signal, enum pxi6259_terminal dest_terminal)
{
        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        switch(signal) {
          case PXI6259_AO_SAMPLE_CLOCK:
                break;
          case PXI6259_AO_START_TRIGGER:
                break;
          default:
                errno = EINVAL; return -1;
        }

        return 0;
}

__must_check int pxi6259_export_gpc_signal(pxi6259_gpc_conf_t *conf,
                enum pxi6259_signal signal, enum pxi6259_terminal dest_terminal)
{
        if (conf == NULL || conf->key != CONFIG_KEY) {
                errno = EINVAL; return -1;
        }

        switch(signal) {
          case PXI6259_GPC0_OUTPUT :
          case PXI6259_GPC1_OUTPUT :
                conf->gpcExportGPCOutSig = dest_terminal;
                break;
          default:
                errno = EINVAL; return -1;
        }

        return 0;
}


/******************************************************/
/***                 Read & Write Data              ***/
/******************************************************/

/* Gain divider lookup table */
static float gain_divider[] = {1.0, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0};

static void ai_polynomial_scaler(int16_t *raw, float *scaled,
                uint32_t num_samples, ai_scaling_coefficients_t *scaling, float divider)
{
        int32_t i, j;
        float rawf;

        for (i = 0; i < num_samples; i++) {
                rawf = (float)raw[i];
                scaled[i] = scaling->value[scaling->order].f;

                for (j = scaling->order-1 ; j >= 0 ; j--) {
                    scaled[i] *= rawf;
                    scaled[i] += scaling->value[j].f;
                }

                scaled[i] /= divider;
        }
}

ssize_t pxi6259_read_ai(int chanfd, float *buffer_scaled, size_t num_samples)
{
        int retval = 0;
        ssize_t bytes_read;
        struct stat stat;
        int channel, cacheidx;
        unsigned int minnum;
        struct timeval current_time;
        long int fdnsec;

        PDEBUG("Read Scaled AI Samples!\n");

        if (num_samples <= 0) {
                PDEBUG ("Buffer length zero!\n");
                return 0;
        }

        bytes_read = read(chanfd, &buffer_scaled[num_samples/2], num_samples*2);
        if (bytes_read <= 0)
                return bytes_read;

        retval = fstat(chanfd, &stat);
        if (retval) return -1;

        minnum = minor(stat.st_rdev);
        channel = (minnum % DEVICE_MINOR_STEP) - 2;
        cacheidx = minnum/DEVICE_MINOR_STEP;
        fdnsec = stat.st_mtim.tv_sec * (long)1000000000 + stat.st_mtim.tv_nsec;

        retval = pthread_mutex_lock(&ai_cache_mutex);
        if (retval) {
                errno = retval;
                return -1;
        }

        /* Cache is not jet initialized */
        if (!ai_initialized) {
                memset(aichanmtime, 0, sizeof(aichanmtime));
                ai_initialized = 1;
        }

        if (aichanmtime[cacheidx] < fdnsec) {
                gettimeofday(&current_time, NULL);

                retval = ioctl(chanfd, PXI6259_IOC_READ_AI_CONF, &ai_confs[cacheidx]);
                if (retval) return -1;

                retval = get_ai_scaling_coefficient(chanfd, &ai_coefs[cacheidx]);
                if (retval) {
                        PDEBUG ("Cannot get scaling coefficients for AI segment!\n");
                        return -1;
                }

                aichanmtime[cacheidx] = current_time.tv_sec * (long)1000000000 + current_time.tv_usec * 1000;
        }

        retval = pthread_mutex_unlock(&ai_cache_mutex);
        if (retval) {
                errno = retval;
                return -1;
        }

        ai_polynomial_scaler((int16_t *)&buffer_scaled[num_samples/2],
                        buffer_scaled, bytes_read/2, &ai_coefs[cacheidx],
                        gain_divider[ai_confs[cacheidx].channel[channel].gain]);

        return bytes_read/2;
}

ssize_t pxi6259_read_ai_exact(int chanfd, float *buffer_scaled, size_t num_samples, uint32_t timeout, uint32_t read_delay){
        int samples_read=0,rc=0;
        struct timeval start_time,elapsed_time;
        static struct pollfd pfd[1];

        /* Set up pollfd */
        pfd[0].fd=chanfd;
        pfd[0].events = POLLIN | POLLPRI;

        gettimeofday(&start_time, NULL);


        /* Start reading samples */
        while(samples_read<num_samples){
            /* Wait for change on FD */
            poll(&pfd,1,(read_delay/1000)+1); //+1 to force it into very small delay (if user sets read_delay to 0)
                                       //ignore timeout errors since we have our own timeout logic

            /* Read as many samples as possible */
            rc = pxi6259_read_ai(chanfd, &buffer_scaled[samples_read], num_samples-samples_read);

            if( rc < 0 ){
                if(errno == EAGAIN){
                    continue;
                }
                else
                    return -1;
            }

            samples_read+=rc;

            if( read_delay > 0 )
                usleep(read_delay);

            /* Check for timeout */
            if( timeout > 0 ){
                gettimeofday(&elapsed_time, NULL);
                if( (elapsed_time.tv_sec-start_time.tv_sec)*1000+(elapsed_time.tv_usec-start_time.tv_usec)/1000 > timeout ){
                    errno = ETIMEDOUT;
                    printf("read_ai_exact timeout!\n");
                    return -1;
                }
            }
        }

        return samples_read;
}


static const float range_lookup[2][2] = {{-10.0, 10.0}, {-5.0, 5.0}};

static inline int ao_linear_scaler(int16_t *raw, float *scaled, uint32_t num_samples,
                scale_converter_select_t converter, ao_scaling_coefficients_t *scaling,
                short check_range, short reference)
{
        uint32_t i;
        float tmp;
        float add = scaling->interval[converter].gain.f
                        + scaling->interval[converter].offset.f;

        for (i=0; i<num_samples; i++) {
                tmp = scaled[i];
                if (likely(check_range)) {
                        if(unlikely(tmp < range_lookup[reference][0] || tmp > range_lookup[reference][1])) {
                                errno = ERANGE;
                                return -1;
                        }
                }
                raw[i] = (int32_t)(tmp * add);
                PDEBUG("Raw value for %f is %x [gain %f offset %f]\n", tmp, raw[i],
                                scaling->interval[converter].gain.f, scaling->interval[converter].offset.f);
        }

        return 0;
}

#define MAX_RAW_SAMPLES 2048

// TODO too many samples are scaled. This can hurt the performance.
ssize_t pxi6259_write_ao(int chanfd, float* buf, size_t num_samples)
{
        int retval, channel, cacheidx;
        unsigned int minn;
        ssize_t num_bytes;
        int16_t raw[MAX_RAW_SAMPLES];
        struct stat stat;
        struct timeval current_time;
        long int fdnsec;
        short check_range, reference;

        if (buf == NULL) {
                errno = EINVAL;
                return -1;
        }

        if (num_samples <= 0)
                return 0;

        retval = fstat(chanfd, &stat);
        if (retval) return -1;

        minn = minor(stat.st_rdev);
        channel = (minn % DEVICE_MINOR_STEP) - (PXI6259_MAX_AI_CHANNEL + 1 + 2) - 1;
        if (channel < 0 || channel > PXI6259_MAX_AO_CHANNEL) {
                errno = EINVAL; return -1;
        }
        cacheidx = minn/DEVICE_MINOR_STEP;
        fdnsec = stat.st_mtim.tv_sec * (long)1000000000 + stat.st_mtim.tv_nsec;

        retval = pthread_mutex_lock(&ao_cache_mutex);
        if (retval) {
                errno = retval;
                return -1;
        }

        /* Cache is not yet initialized */
        if (!ao_initialized) {
                memset(aochanmtime, 0, sizeof(aochanmtime));
                ao_initialized = 1;
        }

        if (aochanmtime[cacheidx] < fdnsec) {
                gettimeofday(&current_time, NULL);

                retval = ioctl(chanfd, PXI6259_IOC_GET_AO_SCALING_COEF, &ao_coefs[cacheidx]);
                if (retval) return retval;

                aochanmtime[cacheidx] = current_time.tv_sec * (long)1000000000 + current_time.tv_usec * 1000;

                retval = ioctl(chanfd, PXI6259_IOC_READ_AO_CONF, &ao_confs[cacheidx]);
                if (retval) return retval;
        }

        retval = pthread_mutex_unlock(&ao_cache_mutex);
        if (retval) {
                errno = retval;
                return -1;
        }

        if (num_samples > MAX_RAW_SAMPLES)
                num_samples = MAX_RAW_SAMPLES;

        reference = ao_confs[cacheidx].channel[channel].referenceSelect;
        check_range = (reference < 2);
        retval = ao_linear_scaler(raw, buf, num_samples, channel, &ao_coefs[cacheidx], check_range, reference);
        if (unlikely(retval))
                return -1;

        num_bytes = write(chanfd, raw, (num_samples * 2));
        if (num_bytes <= 0) return num_bytes;

        return num_bytes/2;
}

/*
 * Kernel does not support floating point directly...
 */
__must_check int get_ai_scaling_coefficient(int fd, ai_scaling_coefficients_t *coeff)
{
        int retval, i;

        if ((fd < 0) || (fd > 1024)) {
                PDEBUG ("Illegal file descriptor value!\n");
                errno = EINVAL; return -1;
        }

        retval = ioctl(fd, PXI6259_IOC_GET_AI_SCALING_COEF, coeff);
        if (retval) return retval;

        for (i=0; i <= coeff->order; i++)
        {
                coeff->value[i].f = (coeff->mode.value[i].f * coeff->interval.gain.f);
                if (i == 0) {
                        coeff->value[i].f = coeff->value[i].f + coeff->interval.offset.f;
                }
        }

        return 0;
}

__must_check int pxi6259_set_gpc_trigger_frequency(pxi6259_gpc_conf_t *conf, uint32_t frequency)
{
        uint32_t retval = 0;
        uint32_t period = 0;
        uint32_t timebase1 = 20000000;  // Hz = 20MHz

        PDEBUG("Configuring counter\n");

        if (conf == NULL) {
                PDEBUG("Error: Task pointer is NULL.\n");
                errno = EINVAL; return -1;
        }

        if (frequency > (timebase1 / 2)) {
                PDEBUG("Error: Frequency to large.\n");
                errno = EINVAL; return -1;
        }

        period = timebase1 / frequency;

        retval = pxi6259_set_gpc_attribute(conf,
                        GPC_INITIAL_VALUE,
                        period);
        retval |= pxi6259_set_gpc_attribute(conf,
                        GPC_SOURCE_SELECT,
                        GPC_SOURCE_SELECT_TIMEBASE1);
        retval |= pxi6259_set_gpc_attribute(conf,
                        GPC_COUNT_DIRECTION,
                        GPC_COUNT_DIRECTION_SOFTWARE_DOWN);
        retval |= pxi6259_set_gpc_attribute(conf,
                        GPC_SELECT_TYPE,
                        GPC_TASK_TYPE_SIMPLE_COUNTER);

        if(retval)
                PDEBUG("Error: Configuring counter failed\n");

        return retval;
}









