/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/examples/common.h $
 * $Id: common.h 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : Common definitions.
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

#include <termios.h>
#include <sys/select.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>

#ifndef __PXI6259_EXAMPLES_COMMON_H
#define __PXI6259_EXAMPLES_COMMON_H

/**
  * Check whether user entered any character - without pressing ENTER.
  *
  * Return immediatelly with success if user entered anything. Otherwise
  * wait at most the specified amount of time and return false otherwise.
  */
int checkForKeyPress(int timeout_us) {
        struct termios ttystate;
        struct timeval tv = { INT_MAX, 0 };
        fd_set fds;
        tcgetattr(STDIN_FILENO, &ttystate);
        int kbhit;

        if (timeout_us > 0) {
                tv.tv_sec  = timeout_us / 1000000;
                tv.tv_usec = timeout_us % 1000000;
        }

        // Disable canonical mode - don't expect user to hit ENTER to confirm input
        ttystate.c_lflag &= ~ ICANON;
        ttystate.c_cc[VMIN] = 1;
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

        // Is there anything waiting on stdin?
        FD_SET(STDIN_FILENO, &fds);
        do {
                select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
                kbhit = FD_ISSET(STDIN_FILENO, &fds);
                if (timeout_us > 0) break;
        } while (!kbhit);

        // Enable canonical mode
        ttystate.c_lflag |= ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

        return kbhit;
}

void print_time ()
{
	struct timeval currTime;
	gettimeofday(&currTime, NULL);
	printf ("[%d.%06d] ", currTime.tv_sec, currTime.tv_usec);
}

#endif
