/*************************************************************************
This file is part of libresourceqt

Copyright (C) 2010 Nokia Corporation.

This library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation
version 2.1 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
USA.
*************************************************************************/

#include <math.h>
#include "time-stat.h"

static struct timespec start_time;

int start_timer(void)
{
    int r;
    r = clock_gettime(CLOCK_REALTIME, &start_time);

    if (r == 0)
        return 1;
    else
        return 0;
}

long int stop_timer(void)
{
    struct timespec end_time;
    int r;
    double milliseconds = 0.0;

    r = clock_gettime(CLOCK_REALTIME, &end_time);

    if (r == 0) {
        milliseconds = 1000.0 * (end_time.tv_sec - start_time.tv_sec) +
                       (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
    }
    start_time.tv_sec = 0;
    start_time.tv_nsec = 0;

    return lround(milliseconds);
}

